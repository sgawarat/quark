// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief Win32アプリケーション
 */
#include <concepts>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

#include <WtsApi32.h>
#include <timeapi.h>
#include <windowsx.h>

#include <quark/engine.hpp>
#include <quark/logging.hpp>

#include "resource.h"
#include "utility.hpp"

namespace quark::inline win32 {
namespace {
/**
 * @brief ハンドルを管理するためのラッパークラス
 *
 * @tparam T ハンドルを表すポインタ型
 * @tparam D ハンドルの破棄を定義する関数オブジェクト
 */
template <typename T, auto D>
  requires(std::is_pointer_v<T> && std::invocable<decltype(D), T>)
using UniqueHandle = std::unique_ptr<std::remove_pointer_t<T>, decltype(D)>;

// Win32のハンドルたち
using WindowClass =
    UniqueHandle<LPCTSTR, [](LPCTSTR h) { UnregisterClass(h, GetModuleHandle(nullptr)); }>;
using Window = UniqueHandle<HWND, [](HWND h) { DestroyWindow(h); }>;
using Icon = UniqueHandle<HICON, [](HICON h) { DestroyIcon(h); }>;
using Menu = UniqueHandle<HMENU, [](HMENU h) { DestroyMenu(h); }>;
using NotificationIcon = UniqueHandle<HWND, [](HWND h) { remove_notify_icon(h, 0); }>;
using SessionNotification = UniqueHandle<HWND, [](HWND h) { WTSUnRegisterSessionNotification(h); }>;

/**
 * @brief タイマー精度の変更を管理するためのクラス
 */
class TimePeriod final : Stationary {
public:
  TimePeriod() noexcept : result_{timeBeginPeriod(1)} {}

  ~TimePeriod() noexcept {
    if (result_ != TIMERR_NOERROR) return;
    timeEndPeriod(1);
  }

  [[nodiscard]] MMRESULT result() const noexcept {
    return result_;
  }

private:
  MMRESULT result_{TIMERR_NOERROR};
};

/**
 * @brief アプリケーション
 */
class App final : private Stationary {
public:
  static constexpr auto* TITLE = L"Quark";                          // タイトル名
  static constexpr auto* CLASS_NAME = L"Quark: Keyboard Emulator";  // ウィンドウクラス名
  static constexpr UINT WM_APP_NOTIFY_ICON = WM_APP + 1;            // 通知アイコンのメッセージID

  App() {
    auto* instance = GetModuleHandle(nullptr);

    // タイマーの分解能を変更できたか確認する
    if (time_period_.result() != TIMERR_NOERROR) {
      warn_log("Failed to timeBeginPeriod");
    }

    // ウィンドウクラス
    window_class_.reset([&] {
      const WNDCLASS wndclass{
        .style = 0,
        .lpfnWndProc = dispatch_wndproc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = instance,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = CLASS_NAME,
      };
      return MAKEINTATOM(RegisterClass(&wndclass));
    }());
    if (!window_class_) throw std::runtime_error("Failed to RegisterClass");

    // ウィンドウ
    window_.reset(CreateWindow(window_class_.get(), TITLE, WS_OVERLAPPED, CW_USEDEFAULT,
        CW_USEDEFAULT, 1, 1, nullptr, nullptr, instance, nullptr));
    if (!window_) throw std::runtime_error("Failed to CreateWindow");
    SetWindowLongPtr(window_.get(), GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(this));  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    // アイコン
    app_icon_.reset(LoadIcon(instance, MAKEINTRESOURCE(IDI_MYAPP)));
    if (!app_icon_) throw std::runtime_error("Failed to LoadIcon(MYAPP)");

    keyboard_on_icon_.reset(LoadIcon(instance, MAKEINTRESOURCE(IDI_KEYBOARD_ON)));
    if (!keyboard_on_icon_) throw std::runtime_error("Failed to LoadIcon(KEYBOARD_ON)");

    keyboard_off_icon_.reset(LoadIcon(instance, MAKEINTRESOURCE(IDI_KEYBOARD_OFF)));
    if (!keyboard_off_icon_) throw std::runtime_error("Failed to LoadIcon(KEYBOARD_OFF)");

    // コンテキストメニュー
    context_menu_.reset(LoadMenu(instance, MAKEINTRESOURCE(IDR_CONTEXT_MENU)));
    if (!context_menu_) throw std::runtime_error("Failed to LoadMenu(CONTEXT_MENU)");
    context_submenu_0_ = GetSubMenu(context_menu_.get(), 0);

    // 通知アイコン
    if (!add_notify_icon(window_.get(), 0, WM_APP_NOTIFY_ICON, keyboard_off_icon_.get(), TITLE)) {
      throw std::runtime_error("Failed to add_notify_icon");
    }
    notification_icon_.reset(window_.get());

    // 画面ロック検知
    if (WTSRegisterSessionNotification(window_.get(), NOTIFY_FOR_THIS_SESSION) == 0) {
      throw std::runtime_error("Failed to WTSRegisterSessionNotification");
    }
    session_notification_.reset(window_.get());

    engine_.init();
  }

  ~App() noexcept = default;

  bool enable() noexcept {
    try {
      engine_.start(on_error, window_.get());
      modify_notify_icon(window_.get(), 0, keyboard_on_icon_.get());
      enabled_ = true;
    } catch (const std::exception& e) {
      disable();
      error_log("Failed to start the engine ({})", e.what());
      enabled_ = false;
    }
    return enabled_;
  }

  void disable() noexcept {
    engine_.stop();
    modify_notify_icon(window_.get(), 0, keyboard_off_icon_.get());
    enabled_ = false;
  }

  void toggle() noexcept {
    if (enabled_) {
      disable();
    } else {
      enable();
    }
  }

  void send(EngineSignal signal) noexcept {
    try {
      engine_.send(signal);
    } catch (const std::exception& e) {
      error_log("Failed to send a signal to Engine ({})", e.what());
    }
  }

  LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept {
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
            toggle();
            break;
          }
          // すべてのキーを離す
          case ID_RESET: {
            send(EngineSignal::RESET);
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
            toggle();
            break;
          }
          // コンテキストメニューを表示させる
          case WM_CONTEXTMENU: {
            SetForegroundWindow(hwnd);
            TrackPopupMenuEx(context_submenu_0_, TPM_RIGHTBUTTON, GET_X_LPARAM(wparam),
                GET_Y_LPARAM(wparam), hwnd, nullptr);
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
          case WTS_SESSION_LOCK:
          case WTS_SESSION_UNLOCK: {
            // 誤動作を防ぐためにキーをすべて離す
            send(EngineSignal::CLEAR);
            break;
          }
          default: break;
        }
        return 0;
      }
      default: break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

private:
  static LRESULT CALLBACK dispatch_wndproc(
      HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
    auto* app = reinterpret_cast<App*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if (app) return app->wndproc(window, message, wparam, lparam);
    return DefWindowProc(window, message, wparam, lparam);
  }

  static void on_error(void* data) noexcept {
    // 現状では、複数のスレッドから呼び出されるので注意
    PostMessage(static_cast<HWND>(data), WM_QUIT, 0, 0);
  }

  TimePeriod time_period_;
  WindowClass window_class_;
  Window window_;
  Icon app_icon_;
  Icon keyboard_on_icon_;
  Icon keyboard_off_icon_;
  Menu context_menu_;
  HMENU context_submenu_0_{};
  NotificationIcon notification_icon_;
  SessionNotification session_notification_;
  Engine engine_;
  bool enabled_{false};  ///< Engineが起動しているか
};
}  // namespace
}  // namespace quark::inline win32

/**
 * @brief メイン関数
 *
 * デバッグ向けにCONSOLEでもビルドできるようにしてある。
 */
#ifdef _CONSOLE
int main() {
#else
int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
#endif
  using namespace quark;

  try {
    App app;
    if (!app.enable()) {
      warn_log("Failed to enable the engine on startup");
    }

    // メッセージループ
    MSG msg{};
    while (true) {
      const auto result = GetMessage(&msg, nullptr, 0, 0);
      if (result == 0) break;  // WM_QUITを受け取った
      if (result < 0) throw std::runtime_error("Failed to GetMessage");

      TranslateMessage(&msg);
      DispatchMessage(&msg);
      std::this_thread::yield();
    }

    app.disable();

    return static_cast<int>(msg.wParam);
  } catch (const std::exception& e) {
    critical_log("Main thread crashed ({})", e.what());
    return EXIT_FAILURE;
  }
}
