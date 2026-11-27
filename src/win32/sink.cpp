// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.cpp
 * @brief Win32におけるSink処理
 */
#include "quark/win32/sink.hpp"

#include <Windows.h>

#include <array>
#include <chrono>
#include <cstddef>

#include "quark/event.hpp"
#include "quark/logging.hpp"
#include "quark/win32/injected.hpp"
#include "quark/win32/key.hpp"

namespace quark::inline win32 {
namespace {
template <bool KEYUP>
class KeyInput {
public:
  explicit KeyInput(Key key) {
    if (key == Key::NO) return;

    DWORD dwFlags = 0;
    if (KEYUP) dwFlags |= KEYEVENTF_KEYUP;
    if (+key > UINT8_MAX) dwFlags |= KEYEVENTF_EXTENDEDKEY;

    WORD wVk = 0;
    if (key == Key::PAUSE) {
      // Pauseはスキャンコードが特殊なのでVKで送る
      wVk = VK_PAUSE;
    } else {
      // それ以外はスキャンコードで送る
      dwFlags |= KEYEVENTF_SCANCODE;
    }

    inputs_.at(size_++) = {
      .type = INPUT_KEYBOARD,
      .ki{
        .wVk = wVk,
        .wScan = static_cast<uint8_t>(+key),
        .dwFlags = dwFlags,
        .dwExtraInfo = EXTRA_INFO_INJECTED,
      },
    };
  }

  explicit KeyInput(KeyCode keycode) : KeyInput{to_key(keycode)} {}

  bool send() noexcept {
    if (size_ == 0) return true;
    return SendInput(size_, inputs_.data(), sizeof(*inputs_.data())) != 0;
  }

private:
  std::array<INPUT, 1> inputs_{};
  UINT size_{};
};

using KeyPressInput = KeyInput<false>;
using KeyReleaseInput = KeyInput<true>;

template <bool KEYUP>
class MouseButtonInput {
public:
  explicit MouseButtonInput(MouseButtonSet buttons) {
    // 左右中央ボタン
    DWORD dwFlags = 0;
    if (buttons.contains(0)) dwFlags |= (KEYUP ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN);
    if (buttons.contains(1)) dwFlags |= (KEYUP ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN);
    if (buttons.contains(2)) dwFlags |= (KEYUP ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN);
    if (dwFlags != 0) {
      inputs_.at(size_++) = {
        .type = INPUT_MOUSE,
        .mi{
          .mouseData = 0,
          .dwFlags = dwFlags,
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      };
    }

    // 戻るボタン
    if (buttons.contains(3)) {
      inputs_.at(size_++) = {
        .type = INPUT_MOUSE,
        .mi{
          .mouseData = XBUTTON1,
          .dwFlags = KEYUP ? MOUSEEVENTF_XUP : MOUSEEVENTF_XDOWN,
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      };
    }

    // 進むボタン
    if (buttons.contains(4)) {
      inputs_.at(size_++) = {
        .type = INPUT_MOUSE,
        .mi{
          .mouseData = XBUTTON2,
          .dwFlags = KEYUP ? MOUSEEVENTF_XUP : MOUSEEVENTF_XDOWN,
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      };
    }
  }

  bool send() noexcept {
    if (size_ == 0) return false;
    return SendInput(size_, inputs_.data(), sizeof(*inputs_.data())) != 0;
  }

private:
  std::array<INPUT, 3> inputs_{};
  UINT size_{};
};

using MousePressInput = MouseButtonInput<false>;
using MouseReleaseInput = MouseButtonInput<true>;

class MouseWheelInput {
public:
  explicit MouseWheelInput(MouseWheelEvent event) {
    // ホイール縦
    if (event.rel_v != 0) {
      inputs_.at(size_++) = {
        .type = INPUT_MOUSE,
        .mi{
          .mouseData = static_cast<DWORD>(event.rel_v * WHEEL_DELTA),
          .dwFlags = MOUSEEVENTF_WHEEL,
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      };
    }

    // ホイール横
    if (event.rel_h != 0) {
      inputs_.at(size_++) = {
        .type = INPUT_MOUSE,
        .mi{
          .mouseData = static_cast<DWORD>(event.rel_h * WHEEL_DELTA),
          .dwFlags = MOUSEEVENTF_HWHEEL,
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      };
    }
  }

  bool send() noexcept {
    if (size_ == 0) return false;
    return SendInput(size_, inputs_.data(), sizeof(*inputs_.data())) != 0;
  }

private:
  std::array<INPUT, 2> inputs_{};
  UINT size_{};
};

class MouseCursorInput {
public:
  explicit MouseCursorInput(MouseCursorEvent event) {
    // カーソル
    if (event.rel_x != 0 || event.rel_y != 0) {
      inputs_.at(size_++) = {
        .type = INPUT_MOUSE,
        .mi{
          .dx = event.rel_x,
          .dy = event.rel_y,
          .dwFlags = MOUSEEVENTF_MOVE,
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      };
    }
  }

  bool send() noexcept {
    if (size_ == 0) return false;
    return SendInput(size_, inputs_.data(), sizeof(*inputs_.data())) != 0;
  }

private:
  std::array<INPUT, 1> inputs_{};
  UINT size_{};
};

constexpr int64_t KEYBOARD_DELAY_DELTA = 250;
constexpr int64_t KEYBOARD_DELAY_MSEC = 250;
constexpr int64_t KEYBOARD_SPEED_MSEC = 33;
}  // namespace

SinkContext::SinkContext(std::shared_ptr<SinkEventQueue> consumer, const SinkContextDesc&)
  : consumer_{std::move(consumer)} {
  if (!consumer_) throw std::invalid_argument("Invalid SinkEventQueue");

  keyboard_delay_ = std::chrono::milliseconds(KEYBOARD_DELAY_MSEC);
  keyboard_speed_ = std::chrono::milliseconds(KEYBOARD_SPEED_MSEC);

  // レジストリからキーリピート設定を読み出す
  // TODO: アクセシビリティのキーリピート設定にも対応する
  {
    constexpr size_t REG_SZ_BUFFER_SIZE = 32;
    std::array<wchar_t, REG_SZ_BUFFER_SIZE> buffer{};
    DWORD size = sizeof(buffer);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Keyboard", L"KeyboardDelay", RRF_RT_REG_SZ,
            nullptr, buffer.data(), &size) == ERROR_SUCCESS) {
      const auto value = _wtoll(buffer.data());  // [0, 3] => [250ms, 1000ms]
      keyboard_delay_ = std::chrono::milliseconds((value + 1) * KEYBOARD_DELAY_DELTA);
    } else {
      QUARK_LOG_WARN("Failed to RegGetValue from KeyboardDelay");
    }

    buffer = {};
    size = sizeof(buffer);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Keyboard", L"KeyboardSpeed", RRF_RT_REG_SZ,
            nullptr, buffer.data(), &size) == ERROR_SUCCESS) {
      const auto value = _wtoll(buffer.data());  // [0, 31] -> [~2.5hz, ~30hz] -> [~400ms, ~33ms]
      keyboard_speed_ =
          std::chrono::milliseconds(12400 / (31 + (11 * value)));  // NOLINT(*-magic-numbers)
    } else {
      QUARK_LOG_WARN("Failed to RegGetValue from KeyboardSpeed");
    }
  }
}

SinkContext::~SinkContext() noexcept {
  prev_keys_.scan([](KeyCode keycode) { KeyReleaseInput{keycode}.send(); });
  MouseReleaseInput{prev_mouse_buttons_}.send();
}

void SinkContext::run(const std::stop_token& stop_token) {
  while (!stop_token.stop_requested()) {
    const auto packet_opt = [&] {
      // キーを押していなければ、初回のキー入力を無限に待つ
      if (last_key_ == KC_NO) {
        return consumer_->pop(stop_token);
      }
      // キーを押していれば、キーリピートを発生させるために一定時間で制御を戻させる
      return consumer_->pop(stop_token, repeat_period_);
    }();
    if (packet_opt) {
      packet_opt->visit([this](const auto& time, const auto& event) { update(time, event); });
    }
  }
}

void SinkContext::update(EventTime, TickEvent) {
  // キーを押しているなら、キーリピートを表すイベントを発生させる
  if (last_key_ == KC_NO) return;
  if (KeyPressInput{last_key_}.send()) {
    repeat_period_ = EventClock::now() + keyboard_speed_;  // 初回以降はspeedで時間を求める
    QUARK_LOG_DEBUG("Sink: repeat {}", +last_key_);
  } else {
    // 安全のために、送信に失敗したらキーリピートを止める
    QUARK_LOG_DEBUG("Sink: Failed to repeat {}", +last_key_);
    last_key_ = KC_NO;
  }
}

void SinkContext::update(EventTime, KeyPressEvent event) {
  if (KeyPressInput{event.keycode}.send()) {
    prev_keys_.insert(event.keycode);
    last_key_ = event.keycode;
    repeat_period_ = EventClock::now() + keyboard_delay_;  // 初回はdelayで時間を求める
    QUARK_LOG_DEBUG("Sink: press {}", +event.keycode);
  } else {
    QUARK_LOG_DEBUG("Sink: Failed to press {}", +event.keycode);
  }
}

void SinkContext::update(EventTime, KeyReleaseEvent event) {
  if (KeyReleaseInput{event.keycode}.send()) {
    prev_keys_.remove(event.keycode);
    QUARK_LOG_DEBUG("Sink: release {}", +event.keycode);
  } else {
    QUARK_LOG_DEBUG("Sink: Failed to release {}", +event.keycode);
  }
  if (event.keycode == last_key_) last_key_ = KC_NO;
}

void SinkContext::update(EventTime, MousePressEvent event) {
  if (MousePressInput{event.buttons}.send()) {
    prev_mouse_buttons_.unite(event.buttons);
  }
}

void SinkContext::update(EventTime, MouseReleaseEvent event) {
  if (MouseReleaseInput{event.buttons}.send()) {
    prev_mouse_buttons_.subtract(event.buttons);
  }
}

void SinkContext::update(EventTime, MouseWheelEvent event) {
  MouseWheelInput{event}.send();
}

void SinkContext::update(EventTime, MouseCursorEvent event) {
  MouseCursorInput{event}.send();
}

void SinkContext::update(EventTime, EngineSignal signal) {
  switch (signal) {
    case EngineSignal::SCREEN_LOCKED: {
      break;
    }
    case EngineSignal::SCREEN_UNLOCKED: {
      QUARK_LOG_DEBUG("Sink: SCREEN_UNLOCKED signal");
      prev_keys_.scan([](KeyCode keycode) {
        if (!KeyReleaseInput{keycode}.send()) {
          QUARK_LOG_DEBUG("Sink: Failed to clear {}", +keycode);
          return;
        }
        QUARK_LOG_DEBUG("Sink: clear {}", +keycode);
      });
      prev_keys_ = {};
      last_key_ = KC_NO;
      MouseReleaseInput{prev_mouse_buttons_}.send();
      prev_mouse_buttons_ = {};
    }
  }
}
}  // namespace quark::inline win32
