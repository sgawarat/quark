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
struct Input final : INPUT {
  constexpr Input() noexcept : INPUT{.type = INPUT_HARDWARE} {}

  explicit Input(uint8_t keycode, bool pressed = true) noexcept
      : INPUT{
            .type = INPUT_KEYBOARD,
            .ki =
                {
                    .wScan = keycode_to_scancode(keycode),
                    .dwFlags = static_cast<DWORD>(KEYEVENTF_SCANCODE),
                },
        } {
    if (ki.wScan > 0xff) ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;  // この関係性が本当に正しいかは不明
    if (!pressed) ki.dwFlags |= KEYEVENTF_KEYUP;
    add_injected(ki);
  }

  constexpr void clear() noexcept {
    type = INPUT_HARDWARE;
  }

  constexpr bool is_valid() const noexcept {
    // INPUT_HARDWAREは使わないので無効値として扱う
    return type != INPUT_HARDWARE;
  }

  void send() noexcept {
    if (is_valid()) SendInput(1, this, sizeof(INPUT));
  }

  void send_tap() noexcept {
    if (is_valid()) {
      ki.dwFlags &= ~DWORD{KEYEVENTF_KEYUP};
      SendInput(1, this, sizeof(INPUT));
      ki.dwFlags |= KEYEVENTF_KEYUP;
      SendInput(1, this, sizeof(INPUT));
    }
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
    latest_press_keycode_ = keycode;
    latest_press_input_ = Input(keycode, true);
    latest_press_input_.send();
  }

  /**
   * @brief 離すイベントを送信する
   */
  void send_key_release(uint8_t keycode) noexcept {
    Input(keycode, false).send();
    if (keycode == latest_press_keycode_) {
      latest_press_keycode_ = KC_NO;
      latest_press_input_.clear();
    }
  }

  /**
   * @brief 押してすぐ離すイベントを送信する
   */
  void send_key_tap(uint8_t keycode) noexcept {
    Input(keycode).send_tap();
    latest_press_keycode_ = KC_NO;
    latest_press_input_.clear();
  }

  /**
   * @brief イベントをそのまま送信する
   */
  void send_native_event(INPUT input) noexcept {
    SendInput(1, &input, sizeof(INPUT));
    latest_press_keycode_ = KC_NO;
    latest_press_input_.clear();
  }

  /**
   * @brief キーリピートを表すイベントを送信する
   */
  void send_key_repeat() noexcept {
    latest_press_input_.send();
  }

  /**
   * @brief キーリピート情報をクリアする
   */
  void clear_key_repeat() noexcept {
    latest_press_keycode_ = KC_NO;
    latest_press_input_.clear();
  }

  /**
   * @brief すべてのキー状態をリセットする
   */
  void reset() noexcept {
    INPUT input{.type = INPUT_KEYBOARD, .ki = {}};
    for (WORD i = 1; i < 255; ++i) {
      input.ki.wVk = i;
      input.ki.dwFlags = KEYEVENTF_KEYUP;
      SendInput(1, &input, sizeof(INPUT));
      input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
      SendInput(1, &input, sizeof(INPUT));
    }
  }

private:
  uint8_t latest_press_keycode_ = KC_NO;  ///< 最後に押したキー
  Input latest_press_input_{};            ///< 最後に押したキーイベント
};
}  // namespace quark::inline win32
