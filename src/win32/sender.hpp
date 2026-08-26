// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sender.hpp
 * @brief Win32の入力イベントを送信するやつ
 */
#pragma once

#include <Windows.h>

#include "injected.hpp"
#include "scancode.hpp"

namespace quark::inline win32 {
/**
 * @brief INPUTのラッパークラス
 */
struct Input final : INPUT {
  constexpr Input() noexcept : INPUT{.type = INPUT_HARDWARE} {}

  explicit Input(uint16_t scancode, DWORD flags) noexcept
      : INPUT{
            .type = INPUT_KEYBOARD,
            .ki{
                .wScan = static_cast<uint8_t>(scancode & 0xff),
                .dwFlags = flags | KEYEVENTF_SCANCODE,
            },
        } {
    if (scancode > 0xff) ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    add_injected(ki);

    // Pauseは正確なスキャンコードが分からないためVKで送る
    if (scancode == 0x45) {
      ki.wVk = VK_PAUSE;
      ki.dwFlags &= ~static_cast<DWORD>(KEYEVENTF_SCANCODE);
    } else if (scancode == 0xe045) {
      ki.wVk = VK_NUMLOCK;
      ki.dwFlags &= ~static_cast<DWORD>(KEYEVENTF_SCANCODE);
    }
  }

  explicit Input(uint8_t keycode) noexcept : Input{keycode_to_scancode(keycode), 0} {}

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
    last_input_ = Input{keycode};
    last_input_.send_keydown();
  }

  /**
   * @brief 離すイベントを送信する
   */
  void send_key_release(uint8_t keycode) noexcept {
    if (keycode == KC_NO) return;
    Input{keycode}.send_keyup();
    if (keycode == last_key_) last_key_ = KC_NO;
  }

  /**
   * @brief 押してすぐ離すイベントを送信する
   */
  void send_key_tap(uint8_t keycode) noexcept {
    if (keycode == KC_NO) return;
    Input{keycode}.send_tap();
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
    last_input_.send_keydown();
  }

  /**
   * @brief 状態を初期値に戻す
   */
  void reset() noexcept {
    last_key_ = KC_NO;
  }

private:
  uint8_t last_key_ = KC_NO;  ///< 最後に押したキー
  Input last_input_{};        ///< 最後に押したキーのイベント
};
}  // namespace quark::inline win32
