// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sender.hpp
 * @brief Win32の入力イベントを送信するやつ
 */
#pragma once

#include <Windows.h>

#include <quark/bitset.hpp>

#include "injected.hpp"
#include "scancode.hpp"

namespace quark::inline win32 {
/**
 * @brief キー入力のためのINPUT
 */
struct KeyInput final : INPUT {
  KeyInput() noexcept : INPUT{} {}

  explicit KeyInput(uint8_t keycode) noexcept : KeyInput{keycode_to_scancode(keycode)} {}

  explicit KeyInput(Scancode scancode) noexcept
      : INPUT{
            .type = INPUT_KEYBOARD,
            .ki{
                .wScan = static_cast<uint8_t>(static_cast<uint16_t>(scancode) & 0xff),
                .dwFlags = KEYEVENTF_SCANCODE,
            },
        } {
    if (static_cast<uint16_t>(scancode) > 0xff) ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    add_injected(ki);

    // Pauseは正確なスキャンコードが分からないためVKで送る
    if (scancode == Scancode{0x45}) {
      ki.wVk = VK_PAUSE;
      ki.dwFlags &= ~static_cast<DWORD>(KEYEVENTF_SCANCODE);
    } else if (scancode == Scancode{0xe045}) {
      ki.wVk = VK_NUMLOCK;
      ki.dwFlags &= ~static_cast<DWORD>(KEYEVENTF_SCANCODE);
    }
  }

  void send_keydown() noexcept {
    if (ki.wScan == 0) return;
    ki.dwFlags &= ~DWORD{KEYEVENTF_KEYUP};
    SendInput(1, this, sizeof(INPUT));
  }

  void send_keyup() noexcept {
    if (ki.wScan == 0) return;
    ki.dwFlags |= KEYEVENTF_KEYUP;
    SendInput(1, this, sizeof(INPUT));
  }

  void send_tap() noexcept {
    if (ki.wScan == 0) return;
    ki.dwFlags &= ~DWORD{KEYEVENTF_KEYUP};
    SendInput(1, this, sizeof(INPUT));
    ki.dwFlags |= KEYEVENTF_KEYUP;
    SendInput(1, this, sizeof(INPUT));
  }
};

class EventSender final {
public:
  void enable() noexcept {}

  void disable() noexcept {}

  /**
   * @brief 押すイベントを送信する
   */
  void send_key_press(uint8_t keycode) noexcept {
    if (keycode == KC_NO) return;
    last_key_ = keycode;
    last_key_input_ = KeyInput{keycode};
    last_key_input_.send_keydown();
  }

  /**
   * @brief 離すイベントを送信する
   */
  void send_key_release(uint8_t keycode) noexcept {
    if (keycode == KC_NO) return;
    KeyInput{keycode}.send_keyup();
    if (keycode == last_key_) last_key_ = KC_NO;
  }

  /**
   * @brief 押してすぐ離すイベントを送信する
   */
  void send_key_tap(uint8_t keycode) noexcept {
    if (keycode == KC_NO) return;
    KeyInput{keycode}.send_tap();
  }

  /**
   * @brief イベントをそのまま送信する
   */
  void send_native_event(INPUT input) noexcept {
    SendInput(1, &input, sizeof(INPUT));
  }

  /**
   * @brief キーリピートを表すイベントを送信する
   */
  void send_key_repeat() noexcept {
    if (last_key_ == KC_NO) return;
    last_key_input_.send_keydown();
  }

  /**
   * @brief マウスボタンを押すイベントを送信する
   *
   * @param buttons 押したボタンのビットセット
   */
  void send_mouse_buttons_press(Bitset<5, uint8_t> buttons) noexcept {
    INPUT i{
        .type = INPUT_MOUSE,
        .mi = {},
    };
    add_injected(i.mi);

    i.mi.dwFlags = 0;
    if (buttons[0]) i.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
    if (buttons[1]) i.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
    if (buttons[2]) i.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
    if (i.mi.dwFlags) SendInput(1, &i, sizeof(INPUT));

    if (buttons[3]) {
      i.mi.mouseData = XBUTTON1;
      i.mi.dwFlags = MOUSEEVENTF_XDOWN;
      SendInput(1, &i, sizeof(INPUT));
    }

    if (buttons[4]) {
      i.mi.mouseData = XBUTTON2;
      i.mi.dwFlags = MOUSEEVENTF_XDOWN;
      SendInput(1, &i, sizeof(INPUT));
    }
  }

  /**
   * @brief マウスボタンを離すイベントを送信する
   *
   * @param buttons 離したボタンのビットセット
   */
  void send_mouse_buttons_release(Bitset<5, uint8_t> buttons) noexcept {
    INPUT i{
        .type = INPUT_MOUSE,
        .mi = {},
    };
    add_injected(i.mi);

    i.mi.dwFlags = 0;
    if (buttons[0]) i.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
    if (buttons[1]) i.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
    if (buttons[2]) i.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
    if (i.mi.dwFlags) SendInput(1, &i, sizeof(INPUT));

    if (buttons[3]) {
      i.mi.mouseData = XBUTTON1;
      i.mi.dwFlags = MOUSEEVENTF_XUP;
      SendInput(1, &i, sizeof(INPUT));
    }

    if (buttons[4]) {
      i.mi.mouseData = XBUTTON2;
      i.mi.dwFlags = MOUSEEVENTF_XUP;
      SendInput(1, &i, sizeof(INPUT));
    }
  }

  /**
   * @brief マウスホイールを動かすイベントを送信する
   *
   * @param v 縦の移動量
   * @param h 横の移動量
   */
  void send_mouse_wheel_move(mouse_hv_report_t v, mouse_hv_report_t h) noexcept {
    INPUT i{
        .type = INPUT_MOUSE,
        .mi = {},
    };
    add_injected(i.mi);

    if (v != 0) {
      i.mi.mouseData = static_cast<DWORD>(v * WHEEL_DELTA);
      i.mi.dwFlags = MOUSEEVENTF_WHEEL;
      SendInput(1, &i, sizeof(INPUT));
    }

    if (h != 0) {
      i.mi.mouseData = static_cast<DWORD>(h * WHEEL_DELTA);
      i.mi.dwFlags = MOUSEEVENTF_HWHEEL;
      SendInput(1, &i, sizeof(INPUT));
    }
  }

  /**
   * @brief マウスを動かすイベントを送信する
   *
   * @param x 横の移動量
   * @param y 縦の移動量
   */
  void send_mouse_move(mouse_xy_report_t x, mouse_xy_report_t y) noexcept {
    INPUT i{
        .type = INPUT_MOUSE,
        .mi =
            {
                .dx = x,
                .dy = y,
                .dwFlags = MOUSEEVENTF_MOVE,
            },
    };
    add_injected(i.mi);

    if (x != 0 || y != 0) SendInput(1, &i, sizeof(INPUT));
  }

  /**
   * @brief 状態を初期値に戻す
   */
  void reset() noexcept {
    last_key_ = KC_NO;
  }

private:
  uint8_t last_key_ = KC_NO;   ///< 最後に押したキー
  KeyInput last_key_input_{};  ///< 最後に押したキーのイベント
};
}  // namespace quark::inline win32
