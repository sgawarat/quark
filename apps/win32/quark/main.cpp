// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief Win32バックグラウンドアプリケーション
 */
#include <sddl.h>     // IWYU pragma: keep
#include <timeapi.h>  // timeBeginPeriod
#include <windows.h>

#include <cstdlib>
#include <filesystem>
#include <span>
#include <stdexcept>

#include "quark/engine.hpp"
#include "quark/logging.hpp"
#include "quark/utility.hpp"
#include "quark/win32/utility.hpp"

namespace quark::inline win32 {
namespace {
/**
 * @brief タイマーの分解能を変更するためのクラス
 */
class TimePeriod final : Pinned {
public:
  TimePeriod() : result_{timeBeginPeriod(1)} {
    if (result_ != TIMERR_NOERROR) {
      throw std::runtime_error("Failed to timeBeginPeriod");
    }
  }

  ~TimePeriod() noexcept {
    if (result_ != TIMERR_NOERROR) return;
    timeEndPeriod(1);
  }

private:
  MMRESULT result_{TIMERR_NOCANDO};
};

/**
 * @brief 入力デスクトップの遷移を監視するためのクラス
 */
class EventHook final : Pinned {
public:
  explicit EventHook(Engine& engine) {
    s_engine = &engine;
    s_handle.reset(SetWinEventHook(
        EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, nullptr,
        [](HWINEVENTHOOK, DWORD event, HWND, LONG, LONG, DWORD, DWORD) {
          if (event == EVENT_SYSTEM_DESKTOPSWITCH) {
            // 現在のInputDesktopハンドルを取得できるかによって、
            // 高い権限を必要とする画面（UAC画面やロック画面）に遷移したことを確認する
            HDESK desk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
            if (desk) {
              CloseDesktop(desk);
              if (s_engine) s_engine->send(EngineSignal::SCREEN_UNLOCKED);
            } else {
              if (s_engine) s_engine->send(EngineSignal::SCREEN_LOCKED);
            }
          }
        },
        0, 0, WINEVENT_OUTOFCONTEXT));
    if (!s_handle) throw std::runtime_error("Failed to SetWinEventHook");
  }

  ~EventHook() noexcept {
    s_handle = {};
    s_engine = nullptr;
  }

private:
  static inline Engine* s_engine{};
  static inline UniqueHandle<HWINEVENTHOOK, UnhookWinEvent> s_handle;
};

/**
 * @brief 現在のプロセスにセキュリティ設定を付与する
 */
bool protect_current_process() {
#ifdef QUARK_ENABLE_DEBUGGING_WORKAROUNDS
#ifdef NDEBUG
#warning "Debugging Workarounds are enabled on Release Build"
#endif
  QUARK_LOG_WARN("Debugging Workarounds are enabled");
#else
  // プロセスに対して、外部からのTerminateとSynchronizeのみを許可するよう設定する
  {
    PSECURITY_DESCRIPTOR process_sd{};
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:P(A;;0x00100001;;;PS)", SDDL_REVISION_1, &process_sd, nullptr) == 0) {
      throw Win32Error("Failed to ConvertStringSecurityDescriptorToSecurityDescriptor");
    }
    const LocalMemory process_sd_ptr{process_sd};

    if (SetKernelObjectSecurity(GetCurrentProcess(), DACL_SECURITY_INFORMATION, process_sd) == 0) {
      throw Win32Error("Failed to SetKernelObjectSecurity");
    }
  }

  // メインスレッドに対して、外部からの一切を禁止するよう設定する
  {
    PSECURITY_DESCRIPTOR thread_sd{};
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:P", SDDL_REVISION_1, &thread_sd, nullptr) == 0) {
      throw Win32Error("Failed to ConvertStringSecurityDescriptorToSecurityDescriptor");
    }
    const LocalMemory thread_sd_ptr{thread_sd};

    if (SetKernelObjectSecurity(GetCurrentThread(), DACL_SECURITY_INFORMATION, thread_sd) == 0) {
      throw Win32Error("Failed to SetKernelObjectSecurity");
    }
  }

  // 不正なハンドル操作で例外を投げるようにする
  {
    PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY pmp{
      .HandleExceptionsPermanentlyEnabled = 1,
      .RaiseExceptionOnInvalidHandleReference = 1,
    };
    if (SetProcessMitigationPolicy(ProcessStrictHandleCheckPolicy, &pmp, sizeof(pmp)) == 0) {
      throw Win32Error("Failed to SetProcessMitigationPolicy");
    }
  }

  // 動的なコード生成を禁止する
  {
    PROCESS_MITIGATION_DYNAMIC_CODE_POLICY pmp{
      .ProhibitDynamicCode = 1,
    };
    if (SetProcessMitigationPolicy(ProcessDynamicCodePolicy, &pmp, sizeof(pmp)) == 0) {
      throw Win32Error("Failed to SetProcessMitigationPolicy");
    }
  }

  // Microsoft署名入りバイナリ以外のロードを禁止する
  {
    PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY pmp{
      .MicrosoftSignedOnly = 1,
    };
    if (SetProcessMitigationPolicy(ProcessSignaturePolicy, &pmp, sizeof(pmp)) == 0) {
      throw Win32Error("Failed to SetProcessMitigationPolicy");
    }
  }
#endif

  return true;
}

/**
 * @brief コマンドオプションを処理するクラス
 */
struct ArgList {
  explicit ArgList(std::span<const std::string_view> args) {
    for (const auto& arg : args) {
      if (arg.empty()) continue;
      if (arg == "--log-level=info") log_level = LogSeverity::INFO;
      if (arg == "--log-file") log_file = true;
    }
  }

  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  LogSeverity log_level = DEFAULT_LOG_LEVEL;
  bool log_file{};
  // NOLINTEND(misc-non-private-member-variables-in-classes)
};

/**
 * @brief コンソールのシグナルハンドラを管理するクラス
 */
class ConsoleCtrlHandler final : Pinned {
public:
  ConsoleCtrlHandler() {
    s_main_thread_id = GetCurrentThreadId();
    if (SetConsoleCtrlHandler(
            [](DWORD type) -> BOOL {
              switch (type) {
                case CTRL_C_EVENT:
                  // スレッドに終了シグナルを出して安全に終了させる
                  QUARK_LOG_INFO("Received a Ctrl+C");
                  PostThreadMessage(s_main_thread_id, WM_QUIT, EXIT_SUCCESS, 0);
                  return TRUE;
                case CTRL_BREAK_EVENT:
                  // スレッドに終了シグナルを出して安全に終了させる
                  QUARK_LOG_INFO("Received a Ctrl+Break");
                  PostThreadMessage(s_main_thread_id, WM_QUIT, EXIT_SUCCESS, 0);
                  return TRUE;
              }

              // CTRL_CLOSE_EVENTなどの場合、ここを抜けるとプロセスが強制終了する
              return TRUE;
            },
            TRUE) == 0) {
      throw Win32Error("Failed to SetConsoleCtrlHandler");
    }
  }

  ~ConsoleCtrlHandler() noexcept {
    SetConsoleCtrlHandler(nullptr, FALSE);
  }

private:
  static inline DWORD s_main_thread_id{};
};
}  // namespace
}  // namespace quark::inline win32

int main(int argc, char* argv[]) {
  using namespace quark;
  try {
    Logger::set_module("main");

    // プロセスとメインスレッドにセキュリティ設定を適用する
    if (!protect_current_process()) {
      QUARK_LOG_FATAL("Process is not secure");
      return EXIT_SUCCESS;
    }

// ここだけはどうしようもないので警告をオフにする
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    // argvを安全なコンテナにラップする
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const std::vector<std::string_view> args{&argv[1], &argv[argc]};
#ifdef __clang__
#pragma clang diagnostic pop
#endif

    // コマンド引数
    const ArgList arg_list{args};

    // ロガー
    const auto log_path =
        std::filesystem::path{get_current_module_path()}.parent_path() / L"quark.log";
    const Logger logger{arg_list.log_level, arg_list.log_file ? log_path : std::filesystem::path{}};

    // Ctrl+Cハンドラ
    const ConsoleCtrlHandler ctrl_handler;

    // タイマー分解能
    const TimePeriod time_period;

    // キーボードエンジン
    quark::Engine engine{
      [id = GetCurrentThreadId()] noexcept { PostThreadMessage(id, WM_QUIT, EXIT_SUCCESS, 0); }};

    // ロック画面検知
    const EventHook event_hook{engine};

    // 開始
    engine.start();

    // メッセージループ
    MSG msg{};
    while (true) {
      const auto ret = GetMessage(&msg, nullptr, 0, 0);
      if (ret < 0) throw std::runtime_error("Failed to GetMessage");
      if (ret == 0) break;  // WM_QUITを受け取った

      // EventHookを処理するため、メッセージを送り出す必要がある
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
  } catch (const std::exception& e) {
    QUARK_LOG_FATAL("Main thread crashed: {}", e.what());
    return EXIT_FAILURE;
  } catch (...) {
    QUARK_LOG_FATAL("Main thread crashed: Unknown error");
    return EXIT_FAILURE;
  }
}
