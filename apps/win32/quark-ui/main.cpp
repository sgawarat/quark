// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief Win32 GUIアプリケーション
 */
#include <Windows.h>
#include <sddl.h>      // SDDL conversion
#include <shellapi.h>  // NotifyIcon
#include <windowsx.h>  // GET_X_PARAM, etc.

#include <exception>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <vector>

#include "quark/logging.hpp"
#include "quark/utility.hpp"
#include "quark/win32/utility.hpp"
#include "resource.h"

namespace quark::inline win32 {
namespace {
using Icon = UniqueHandle<HICON, DestroyIcon>;
using Menu = UniqueHandle<HMENU, DestroyMenu>;

constexpr UINT WM_NOTIFY_ICON = WM_USER + 0x10000;    ///< NotifyIconから来るメッセージのID
constexpr UINT WM_CHILD_PROCESS = WM_USER + 0x20000;  ///< 子プロセスに関するメッセージのID

/**
 * @brief アプリケーション
 */
class Application : Pinned {
public:
  Application() {
    auto* const instance = GetModuleHandle(nullptr);

    const WNDCLASSW wndclass{
      .lpfnWndProc = dispatch_wndproc,
      .hInstance = instance,
      .lpszClassName = TITLE,
    };
    s_window_class.reset(MAKEINTATOM(RegisterClass(&wndclass)));
    if (!s_window_class) throw Win32Error("Failed to RegisterClass");

    s_window.reset(CreateWindow(s_window_class.get(), TITLE, WS_OVERLAPPED, CW_USEDEFAULT,
        CW_USEDEFAULT, 1, 1, nullptr, nullptr, instance, nullptr));
    if (!s_window) throw Win32Error("Failed to CreateWindow");
  }

  ~Application() noexcept {
    s_window = {};
    s_window_class = {};
    s_wndproc = {};
    s_exception_ptr_from_wndproc = {};
  }

  /**
   * @brief メインループ
   *
   * @param wndproc ウィンドウプロシージャ
   */
  template <typename WndProc>
  static int run(WndProc&& wndproc) {
    s_wndproc = std::forward<WndProc>(wndproc);
    s_exception_ptr_from_wndproc = {};
    MSG msg{};
    while (true) {
      const auto result = GetMessage(&msg, nullptr, 0, 0);
      if (result == 0) break;  // WM_QUITを受け取った
      if (result < 0) throw std::runtime_error("Failed to GetMessage");
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    if (s_exception_ptr_from_wndproc) {
      std::rethrow_exception(s_exception_ptr_from_wndproc);
    }
    return static_cast<int>(msg.wParam);
  }

  static HWND window() noexcept {
    return s_window.get();
  }

  /**
   * @brief システムのカラーテーマがダークモードか調べる
   */
  static bool is_dark_mode() noexcept {
    DWORD value = 1;
    DWORD size = sizeof(value);
    const LSTATUS ret = RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (ret != ERROR_SUCCESS) return false;
    return value == 0;
  }

private:
  using WindowClass =
      UniqueHandle<LPCTSTR, [](LPCTSTR h) { UnregisterClass(h, GetModuleHandle(nullptr)); }>;
  using Window = UniqueHandle<HWND, DestroyWindow>;

  static inline const wchar_t* const TITLE = L"Quark: Virtual Keyboard";

  static LRESULT CALLBACK dispatch_wndproc(
      HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    if (s_wndproc) {
      try {
        return s_wndproc(window, message, wparam, lparam);
      } catch (...) {
        s_exception_ptr_from_wndproc = std::current_exception();
        PostQuitMessage(EXIT_FAILURE);
      }
    }
    return DefWindowProc(window, message, wparam, lparam);
  }

  static inline WindowClass s_window_class;
  static inline std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> s_wndproc;
  static inline std::exception_ptr s_exception_ptr_from_wndproc;
  static inline Window s_window;
};

/**
 * @brief システムトレイアイコン
 */
class NotifyIcon : MoveOnly {
public:
  explicit NotifyIcon(HWND window, HICON icon) : window_{window}, icon_{icon} {
    if (!window_) throw std::invalid_argument("Invalid HWND");
    if (!icon_) throw std::invalid_argument("Invalid HICON");
    reset();
  }

  NotifyIcon(NotifyIcon&& other) noexcept : window_{other.window_}, id_{other.id_} {
    other.window_ = nullptr;
    other.id_ = 0;
  }

  NotifyIcon& operator=(NotifyIcon&& other) noexcept {
    NotifyIcon tmp{std::move(other)};
    swap(*this, tmp);
    return *this;
  }

  ~NotifyIcon() noexcept {
    if (!window_) return;
    NOTIFYICONDATAW data{
      .cbSize = sizeof(data),
      .hWnd = window_,
      .uID = id_,
    };
    Shell_NotifyIconW(NIM_DELETE, &data);
  }

  explicit operator bool() const noexcept {
    return window_ != nullptr;
  }

  /**
   * @brief アイコンを変更する
   */
  void modify(HICON icon) {
    if (!window_) return;
    NOTIFYICONDATA data{
      .cbSize = sizeof(data),
      .hWnd = window_,
      .uID = id_,
      .uFlags = NIF_ICON | NIF_SHOWTIP,
      .hIcon = icon,
    };
    if (Shell_NotifyIcon(NIM_MODIFY, &data) == 0) {
      throw Win32Error("Failed to Shell_NotifyIcon(NIM_MODIFY, NIF_ICON)");
    }
  }

  /**
   * @brief トレイアイコンを再登録する
   */
  void reset() {
    NOTIFYICONDATAW data{
      .cbSize = sizeof(data),
      .hWnd = window_,
      .uID = id_,
      .uFlags = UINT{NIF_MESSAGE} | NIF_ICON | NIF_TIP | NIF_SHOWTIP,
      .uCallbackMessage = WM_NOTIFY_ICON,
      .hIcon = icon_,
      .szTip = L"Quark: Virtual Keyboard",
      .uVersion = NOTIFYICON_VERSION_4,
    };

    // 登録済みのアイコンがあれば、それを使い回す
    if (Shell_NotifyIconW(NIM_MODIFY, &data) != 0) return;

    // システムトレイにアイコンを登録する
    // 高負荷などが原因で登録に失敗することがあるらしいので、成功するまで何回か試みる
    constexpr size_t ITERATION_COUNT = 5;
    constexpr DWORD WAIT_MSEC = 100;
    DWORD result = ERROR_SUCCESS;
    for (size_t i = 0; i < ITERATION_COUNT; ++i) {
      // 登録を試みる
      if (Shell_NotifyIconW(NIM_ADD, &data) != 0) {
        result = ERROR_SUCCESS;
        break;
      }

      // タイムアウト以外のエラーは復旧できないので失敗として扱う
      if (result = GetLastError(); result != ERROR_TIMEOUT) {
        throw Win32Error(result, "Failed to Shell_NotifyIcon(NIM_ADD)");
      }

      // タイムアウト後に登録が成功する場合もあるらしいので、少し待ってから成否を判断する
      Sleep(WAIT_MSEC);
      if (Shell_NotifyIconW(NIM_MODIFY, &data) != 0) {
        result = ERROR_SUCCESS;
        break;
      }
    }

    // 何度やっても登録できなかった場合、タイムアウトエラーも失敗として扱う
    if (result != ERROR_SUCCESS) {
      throw Win32Error(result, "Failed to Shell_NotifyIcon(NIM_ADD)");
    }

    // 登録したアイコンに新しいバージョンをセットする
    if (Shell_NotifyIconW(NIM_SETVERSION, &data) == 0) {
      const DWORD tmp = GetLastError();
      Shell_NotifyIconW(NIM_DELETE, &data);
      throw Win32Error(tmp, "Failed to Shell_NotifyIcon(NIM_SETVERSION)");
    }
  }

  friend void swap(NotifyIcon& lhs, NotifyIcon& rhs) noexcept {
    std::swap(lhs.window_, rhs.window_);
    std::swap(lhs.id_, rhs.id_);
  }

private:
  HWND window_{};
  UINT id_{};
  HICON icon_{};
};

/**
 * @brief 子プロセス
 */
class Process : MoveOnly {
public:
  Process() = default;

  Process(Process&& other) noexcept : info_{other.info_}, wait_{other.wait_} {
    other.info_ = {};
    other.wait_ = {};
  }

  Process& operator=(Process&& other) noexcept {
    Process tmp{std::move(other)};
    swap(*this, tmp);
    return *this;
  }

  ~Process() noexcept {
    try {
      terminate();
    } catch (std::exception& e) {
      QUARK_LOG_ERROR("Failed to terminate a child process on cleanup: {}", e.what());
    }
  }

  explicit operator bool() const noexcept {
    return info_.hProcess != nullptr;
  }

  /**
   * @brief プロセスを起動する
   */
  void start(const std::filesystem::path& program, std::wstring_view arg_list = {}) {
    // 外部からのTerminateとSynchronizeのみを許可するセキュリティ記述子を作る
    PSECURITY_DESCRIPTOR process_sd{};
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:P(A;;0x00100001;;;PS)", SDDL_REVISION_1, &process_sd, nullptr) == 0) {
      throw Win32Error("Failed to ConvertStringSecurityDescriptorToSecurityDescriptor");
    }
    const LocalMemory process_sd_ptr{process_sd};

    // 外部からの一切を禁止するセキュリティ記述子を作る
    PSECURITY_DESCRIPTOR thread_sd{};
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:P", SDDL_REVISION_1, &thread_sd, nullptr) == 0) {
      throw Win32Error("Failed to ConvertStringSecurityDescriptorToSecurityDescriptor");
    }
    const LocalMemory thread_sd_ptr{thread_sd};

    // Mitigation Policyを設定するAttribute Listを作る
    SIZE_T attr_list_size{};
    InitializeProcThreadAttributeList(nullptr, 1, 0, &attr_list_size);  // 仕様上エラーを返す

    std::vector<std::byte> attr_list_buffer(attr_list_size);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    auto* const attr_list = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attr_list_buffer.data());
    if (InitializeProcThreadAttributeList(attr_list, 1, 0, &attr_list_size) == 0) {
      throw Win32Error("Failed to InitializeProcThreadAttributeList");
    }
    const UniqueHandle<LPPROC_THREAD_ATTRIBUTE_LIST, DeleteProcThreadAttributeList> attr_list_ptr{
      attr_list};

    DWORD64 mitigation_policies{
      PROCESS_CREATION_MITIGATION_POLICY_STRICT_HANDLE_CHECKS_ALWAYS_ON |
          PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON |
          PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON,
    };
    if (UpdateProcThreadAttribute(attr_list, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY,
            &mitigation_policies, sizeof(mitigation_policies), nullptr, nullptr) == 0) {
      throw Win32Error("Failed to UpdateProcThreadAttribute");
    }

    // プロセスを起動する
    std::vector<wchar_t> cmd;
    cmd.push_back(L'"');
    cmd.append_range(program.wstring());
    cmd.push_back(L'"');
    cmd.push_back(L' ');
    cmd.append_range(arg_list);
    cmd.push_back(L'\0');
    SECURITY_ATTRIBUTES process_sa{
      .nLength = sizeof(process_sa),
      .lpSecurityDescriptor = process_sd,
      .bInheritHandle = FALSE,
    };
    SECURITY_ATTRIBUTES thread_sa{
      .nLength = sizeof(thread_sa),
      .lpSecurityDescriptor = thread_sd,
      .bInheritHandle = FALSE,
    };
    STARTUPINFOEX startup_info{
      .StartupInfo{
        .cb = sizeof(startup_info),
      },
      .lpAttributeList = attr_list,
    };
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    auto* startup_info_ptr = reinterpret_cast<LPSTARTUPINFO>(&startup_info);
    if (CreateProcessW(program.c_str(), cmd.data(), &process_sa, &thread_sa, TRUE,
            CREATE_NO_WINDOW | CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr,
            startup_info_ptr, &info_) == 0) {
      throw Win32Error("Failed to CreateProcess");
    }

    // プロセス終了時にメッセージを送るようにする
    if (RegisterWaitForSingleObject(
            &wait_, info_.hProcess,
            [](PVOID, BOOLEAN) { PostMessage(Application::window(), WM_CHILD_PROCESS, 0, 0); },
            nullptr, INFINITE, WT_EXECUTEONLYONCE) == 0) {
      if (info_.hThread) CloseHandle(info_.hThread);
      if (info_.hProcess) CloseHandle(info_.hProcess);
      info_ = {};
      throw Win32Error("Failed to RegisterWaitForSingleObject");
    }

    if (ResumeThread(info_.hThread) > 1) {
      throw Win32Error("Failed to ResumeThread");
    }
  }

  /**
   * @brief プロセスを終了させる
   */
  void terminate() {
    if (!info_.hProcess) return;

    if (still_active()) {
      if (PostThreadMessageW(info_.dwThreadId, WM_QUIT, EXIT_SUCCESS, 0) == 0) {
        throw Win32Error("Failed to PostThreadMessage");
      }

      constexpr DWORD WAIT_MSEC = 1000;
      if (WaitForSingleObject(info_.hProcess, WAIT_MSEC) != WAIT_OBJECT_0) {
        if (TerminateProcess(info_.hProcess, EXIT_FAILURE) == 0) {
          throw Win32Error("Failed to TerminateProcess");
        }
      }
    }

    if (wait_) {
      UnregisterWait(wait_);
      wait_ = {};
    }

    if (info_.hThread) CloseHandle(info_.hThread);
    if (info_.hProcess) CloseHandle(info_.hProcess);
    info_ = {};
  }

  /**
   * @brief プロセスが動作中か調べる
   */
  [[nodiscard]] bool still_active() const noexcept {
    if (!info_.hProcess) return false;

    DWORD code{};
    if (GetExitCodeProcess(info_.hProcess, &code) == 0) return false;
    return code == STILL_ACTIVE;
  }

  friend void swap(Process& lhs, Process& rhs) noexcept {
    std::swap(lhs.info_, rhs.info_);
    std::swap(lhs.wait_, rhs.wait_);
  }

private:
  PROCESS_INFORMATION info_{};
  HANDLE wait_{};
};

/**
 * @brief アイコン
 */
class IconSet : Pinned {
public:
  [[nodiscard]] HICON keyboard(bool enabled) const noexcept {
    if (Application::is_dark_mode()) {
      return enabled ? dark_.keyboard_on.get() : dark_.keyboard_off.get();
    }
    return enabled ? light_.keyboard_on.get() : light_.keyboard_off.get();
  }

private:
  static HICON load(WORD icon_id, const char* what) {
    auto* const handle = static_cast<HICON>(
        LoadImageW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(icon_id), IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));
    if (!handle) throw Win32Error(what);
    return handle;
  }

  struct Light {
    const Icon keyboard_on{load(IDI_KEYBOARD_ON, "Failed to load IDI_KEYBOARD_ON")};
    const Icon keyboard_off{load(IDI_KEYBOARD_OFF, "Failed to load IDI_KEYBOARD_OFF")};
  } light_;
  struct Dark {
    const Icon keyboard_on{load(IDI_DARK_KEYBOARD_ON, "Failed to load IDI_DARK_KEYBOARD_ON")};
    const Icon keyboard_off{load(IDI_DARK_KEYBOARD_OFF, "Failed to load IDI_DARK_KEYBOARD_OFF")};
  } dark_;
};

/**
 * @brief コンテキストメニュー
 */
class ContextMenu : Pinned {
public:
  /**
   * @brief アイテムのテキストを変更する
   */
  void set_text(WORD id, std::wstring_view text) {
    buffer_.assign_range(text);
    buffer_.push_back(L'\0');
    const MENUITEMINFOW info{
      .cbSize = sizeof(info),
      .fMask = MIIM_STRING,
      .dwTypeData = buffer_.data(),
    };
    if (SetMenuItemInfoW(menu_.get(), id, FALSE, &info) == 0) {
      throw Win32Error("Failed to SetMenuItemInfo");
    }
  }

  [[nodiscard]] HMENU handle() const noexcept {
    return popup_;
  }

private:
  HMENU load() {
    auto* const handle = LoadMenu(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDR_CONTEXT_MENU));
    if (!handle) throw Win32Error("Failed to load IDM_CONTEXT_MENU");
    return handle;
  }

  const Menu menu_{load()};
  HMENU const popup_{GetSubMenu(menu_.get(), 0)};  // NOLINT(misc-misplaced-const)
  std::vector<wchar_t> buffer_;
};

/**
 * @brief 子プロセスと親プロセスを紐づけるオブジェクト
 */
class JobObject : Pinned {
public:
  JobObject() {
    if (!handle_) {
      throw Win32Error("Failed to CreateJobObject");
    }

    // 「ジョブオブジェクトを破棄するときに関連プロセスを終了させる」フラグを立てる
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info{
      .BasicLimitInformation{
        .LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE,
      },
    };
    if (SetInformationJobObject(
            handle_.get(), JobObjectExtendedLimitInformation, &job_info, sizeof(job_info)) == 0) {
      throw Win32Error("Failed to SetInformationJobObject");
    }

    // 現在のプロセスをジョブオブジェクトに登録する
    // 以降、起動する子プロセスも同じジョブオブジェクトに自動的に登録される
    if (AssignProcessToJobObject(handle_.get(), GetCurrentProcess()) == 0) {
      throw Win32Error("Failed to AssignProcessToJobObject");
    }
  }

private:
  const Handle handle_{CreateJobObject(nullptr, nullptr)};
};
}  // namespace
}  // namespace quark::inline win32

/**
 * @brief メイン関数
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  using namespace quark;
  try {
    // ロガー
    Logger::set_source("ui");
    const auto log_path = std::filesystem::path{get_current_module_path()} / "quark-ui.log";
    const Logger logger{DEFAULT_LOG_LEVEL, log_path};

    // アプリケーション
    const Application app;

    // アイコン
    const IconSet icons;

    // コンテキストメニュー
    ContextMenu context_menu;

    // 通知アイコン
    NotifyIcon notif{Application::window(), icons.keyboard(true)};

    // タスクバーが再起動したときに送られるメッセージ
    const UINT wm_taskbar_created = RegisterWindowMessage(L"TaskbarCreated");

    // ジョブオブジェクト
    const JobObject job;

    // 子プロセス
    const auto parent_dir = std::filesystem::path{get_current_module_path()}.parent_path();
    const auto child_program = parent_dir / L"quark.exe";
    const auto* const child_arg_list = L"--log-file";
    Process child;
    child.start(child_program, child_arg_list);
    notif.modify(icons.keyboard(true));
    context_menu.set_text(ID_START_STOP, L"Stop");

    // メインループ
    return Application::run([&](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
      switch (msg) {
        case WM_COMMAND: {
          const WORD id = LOWORD(wparam);
          switch (id) {
            // 終了
            case ID_EXIT: {
              PostQuitMessage(EXIT_SUCCESS);
              break;
            }
            // 子プロセスの起動・停止
            case ID_START_STOP: {
              if (child.still_active()) {
                child.terminate();
                notif.modify(icons.keyboard(false));
                context_menu.set_text(ID_START_STOP, L"&Start");
              } else {
                child.start(child_program, child_arg_list);
                notif.modify(icons.keyboard(true));
                context_menu.set_text(ID_START_STOP, L"&Stop");
              }
              break;
            }
          }
          return 0;
        }
        case WM_NOTIFY_ICON: {
          switch (LOWORD(lparam)) {
            // コンテキストメニューを表示させる
            case WM_CONTEXTMENU: {
              SetForegroundWindow(hwnd);
              TrackPopupMenuEx(context_menu.handle(), TPM_RIGHTBUTTON, GET_X_LPARAM(wparam),
                  GET_Y_LPARAM(wparam), hwnd, nullptr);
              PostMessage(hwnd, WM_NULL, 0, 0);
              break;
            }
          }
          return 0;
        }
        case WM_CHILD_PROCESS: {
          // 子プロセスが終了した
          child.terminate();
          notif.modify(icons.keyboard(false));
          context_menu.set_text(ID_START_STOP, L"&Start");
          return 0;
        }
      }

      // タスクバーが再起動したら、再度システムトレイにアイコンを追加する
      if (msg == wm_taskbar_created) {
        notif.reset();
      }

      return DefWindowProc(hwnd, msg, wparam, lparam);
    });
  } catch (const std::exception& e) {
    QUARK_LOG_FATAL("UI thread crashed: {}", e.what());
    return EXIT_FAILURE;
  } catch (...) {
    QUARK_LOG_FATAL("UI thread crashed: Unknown error");
    return EXIT_FAILURE;
  }
}
