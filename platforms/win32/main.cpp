// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief Win32アプリケーション
 */
#include <thread>
#include <utility>

#include <cstdio>

#include <Windows.h>
#include <WtsApi32.h>
#include <windowsx.h>

#include <quark/keyboard.hpp>
#include <quark/sink.hpp>
#include <quark/source.hpp>

#include "resource.h"
#include "utility.hpp"

extern "C" {
#include <debug.h>
}

namespace quark {
namespace {
const WCHAR* const TITLE = L"Quark";                          // タイトル名
const WCHAR* const CLASS_NAME = L"Quark: Keyboard Emulator";  // ウィンドウクラス名
constexpr UINT WM_APP_NOTIFY_ICON = WM_APP + 1;               // 通知アイコンのメッセージID

HMENU context_menu_{};  // コンテキストメニュー
HICON keyboard_on_icon_{};
HICON keyboard_off_icon_{};

DWORD main_thread_id_ = 0;             ///< メインスレッドID
std::exception_ptr main_ep_{};         ///< メインスレッドで投げられた例外
std::future<void> source_future_{};    ///< Sourceスレッドの戻り値
std::future<void> keyboard_future_{};  ///< Keyboardスレッドの戻り値
std::future<void> sink_future_{};      ///< Sinkスレッドの戻り値

/**
 * @brief スコープ終わりに関数を呼び出すクラス
 */
template <typename Dtor>
class Scoped {
public:
  Scoped(Dtor dtor) : dtor_(std::move(dtor)) {}
  ~Scoped() {
    dtor_();
  }

private:
  Dtor dtor_;
};

template <typename Dtor>
Scoped(Dtor) -> Scoped<Dtor>;

bool can_start() noexcept {
  return get_source_status() == SourceStatus::STOPPED && get_keyboard_status() == KeyboardStatus::STOPPED &&
         get_sink_status() == SinkStatus::STOPPED;
}

/**
 * @brief ウィンドウプロシージャ
 */
LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept {
  switch (msg) {
    case WM_COMMAND: {
      const WORD id = LOWORD(wparam);
      switch (id) {
        // 終了
        case ID_EXIT: {
          PostQuitMessage(0);
          break;
        }
        // 有効無効の切り替え
        case ID_ENABLE_DISABLE: {
          try {
            if (can_start()) {
              sink_future_ = start_sink();
              keyboard_future_ = start_keyboard();
              source_future_ = start_source();
              modify_notify_icon(hwnd, 0, keyboard_on_icon_);
            } else {
              stop_source();
              stop_keyboard();
              stop_sink();
              modify_notify_icon(hwnd, 0, keyboard_off_icon_);
            }
          } catch (...) {
            main_ep_ = std::current_exception();
            PostQuitMessage(0);
          }
          break;
        }
        // すべてのキーを離す
        case ID_RESET: {
          try {
            send_to_keyboard(KeyboardSignal::RESET);
          } catch (...) {
            main_ep_ = std::current_exception();
            PostQuitMessage(0);
          }
          break;
        }
        default: break;
      }
      return 0;
    }
    case WM_APP_NOTIFY_ICON: {
      switch (LOWORD(lparam)) {
        // オンオフを切り替える
        case WM_LBUTTONDOWN: {
          try {
            if (can_start()) {
              sink_future_ = start_sink();
              keyboard_future_ = start_keyboard();
              source_future_ = start_source();
              modify_notify_icon(hwnd, 0, keyboard_on_icon_);
            } else {
              stop_source();
              stop_keyboard();
              stop_sink();
              modify_notify_icon(hwnd, 0, keyboard_off_icon_);
            }
          } catch (...) {
            main_ep_ = std::current_exception();
            PostQuitMessage(0);
          }
          break;
        }
        // コンテキストメニューを表示させる
        case WM_CONTEXTMENU: {
          SetForegroundWindow(hwnd);
          TrackPopupMenuEx(context_menu_, TPM_RIGHTBUTTON, GET_X_LPARAM(wparam), GET_Y_LPARAM(wparam), hwnd, nullptr);
          PostMessage(hwnd, WM_NULL, 0, 0);
          break;
        }
        default: break;
      }
      return 0;
    }
    case WM_WTSSESSION_CHANGE: {
      switch (wparam) {
        // ロック画面を行き来するとき
        case WTS_SESSION_LOCK: [[fallthrough]];
        case WTS_SESSION_UNLOCK: {
          // 誤動作を防ぐためにキーをすべて離す
          try {
            send_to_keyboard(KeyboardSignal::CLEAR);
          } catch (...) {
            main_ep_ = std::current_exception();
            PostQuitMessage(0);
          }
          break;
        }
        default: break;
      }
      return 0;
    }
    default: return DefWindowProc(hwnd, msg, wparam, lparam);
  }
}
}  // namespace

void on_source_error() noexcept {
  if (main_thread_id_ > 0) PostThreadMessage(main_thread_id_, WM_QUIT, 0, 0);
}

void on_keyboard_error() noexcept {
  if (main_thread_id_ > 0) PostThreadMessage(main_thread_id_, WM_QUIT, 0, 0);
}

void on_sink_error() noexcept {
  if (main_thread_id_ > 0) PostThreadMessage(main_thread_id_, WM_QUIT, 0, 0);
}

/**
 * @brief メイン関数
 */
extern "C" int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int) {
#ifndef NO_PRINT
  // プリント機能のためにコンソールを出す
  AllocConsole();
  [[maybe_unused]] FILE* conout_fp = nullptr;
  freopen_s(&conout_fp, "CONOUT$", "w", stdout);
  freopen_s(&conout_fp, "CONOUT$", "w", stderr);
#endif

  // 重複起動を防止する
  const HANDLE mutex = CreateMutex(nullptr, TRUE, CLASS_NAME);
  if (mutex == nullptr) return EXIT_FAILURE;
  const Scoped mutex_dtor{[&] { CloseHandle(mutex); }};
  if (GetLastError() == ERROR_ALREADY_EXISTS) return EXIT_SUCCESS;

  main_thread_id_ = GetCurrentThreadId();

  // エラーハンドリング
  const Scoped error_handling{[] {
    try {
      if (main_ep_) std::rethrow_exception(main_ep_);
      if (sink_future_.valid()) sink_future_.get();
      if (keyboard_future_.valid()) keyboard_future_.get();
      if (source_future_.valid()) source_future_.get();
    } catch ([[maybe_unused]] std::exception& e) {
      // TODO: 異常停止を安全にユーザーへ知らせる方法を考える
    }
  }};

  // ウィンドウクラス
  const ATOM cls = [&] {
    const WNDCLASS wc{0,
                      window_proc,
                      0,
                      0,
                      instance,
                      nullptr,
                      LoadCursor(nullptr, IDC_ARROW),
                      HBRUSH(COLOR_WINDOW + 1),
                      nullptr,
                      L"Quark WNDCLASS"};
    return RegisterClass(&wc);
  }();
  if (cls == 0) return EXIT_FAILURE;
  const Scoped cls_dtor{[&] { UnregisterClass(MAKEINTATOM(cls), instance); }};

  // ウィンドウ
  const HWND wnd = CreateWindow(
      MAKEINTATOM(cls), TITLE, WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, nullptr, nullptr, instance, nullptr);
  if (!wnd) return EXIT_FAILURE;
  const Scoped wnd_dtor{[&] { DestroyWindow(wnd); }};

  // アイコン
  const HICON icon = LoadIcon(instance, MAKEINTRESOURCE(IDI_MYAPP));
  if (!icon) return EXIT_FAILURE;
  const Scoped icon_dtor{[&] { DestroyIcon(icon); }};

  keyboard_on_icon_ = LoadIcon(instance, MAKEINTRESOURCE(IDI_KEYBOARD_ON));
  if (!keyboard_on_icon_) return EXIT_FAILURE;
  const Scoped keyboard_on_icon_dtor{[&] { DestroyIcon(keyboard_on_icon_); }};

  keyboard_off_icon_ = LoadIcon(instance, MAKEINTRESOURCE(IDI_KEYBOARD_OFF));
  if (!keyboard_off_icon_) return EXIT_FAILURE;
  const Scoped keyboard_off_icon_dtor{[&] { DestroyIcon(keyboard_off_icon_); }};

  // コンテキストメニュー
  const HMENU menu = LoadMenu(instance, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
  if (!menu) return EXIT_FAILURE;
  const Scoped menu_dtor{[&] { DestroyMenu(menu); }};
  if (!(context_menu_ = GetSubMenu(menu, 0))) return EXIT_FAILURE;

  // 通知アイコン
  if (!add_notify_icon(wnd, 0, WM_APP_NOTIFY_ICON, keyboard_on_icon_, TITLE)) return EXIT_FAILURE;
  const Scoped notify_icon_dtor{[&] { remove_notify_icon(wnd, 0); }};

  // 画面ロック検知
  WTSRegisterSessionNotification(wnd, NOTIFY_FOR_THIS_SESSION);
  const Scoped session_notification_dtor{[&] { WTSUnRegisterSessionNotification(wnd); }};

  // Sink
  sink_future_ = start_sink();
  const Scoped sink_dtor{[] { stop_sink(); }};

  // Keyboard
  keyboard_future_ = start_keyboard();
  const Scoped keyboard_dtor{[] { stop_keyboard(); }};

  // Source
  source_future_ = start_source();
  const Scoped source_dtor{[] { stop_source(); }};

  // メッセージループ
  MSG msg{};
  while (true) {
    const auto result = GetMessage(&msg, nullptr, 0, 0);
    if (result == 0) break;  // WM_QUITを受け取った
    if (result < 0) {
      main_ep_ = std::make_exception_ptr(std::runtime_error("Failed to GetMessage"));
      break;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
    std::this_thread::yield();
  }

  return static_cast<int>(msg.wParam);
}
}  // namespace quark
