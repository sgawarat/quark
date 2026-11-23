// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief X11キー入力テスト
 */
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <cstdlib>
#include <print>
#include <thread>

int main() {
  try {
    Display* const display = XOpenDisplay(nullptr);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
    const Window window = DefaultRootWindow(display);
#pragma clang diagnostic pop

    XGrabKeyboard(display, window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);

    while (true) {
      XPending(display);

      XEvent event{};
      XNextEvent(display, &event);

      switch (event.type) {
        case KeyPress: {
          std::println("press: {}", event.xkey.keycode);
          break;
        }
        case KeyRelease: {
          std::println("release: {}", event.xkey.keycode);
          break;
        }
      }

      if (event.type == KeyPress &&
          XkbKeycodeToKeysym(display, static_cast<KeyCode>(event.xkey.keycode), 0, 0) ==
              XK_Escape) {
        break;
      }

      std::this_thread::yield();
    }

    XUngrabKeyboard(display, CurrentTime);

    return EXIT_SUCCESS;
  } catch (...) {
    return EXIT_FAILURE;
  }
}
