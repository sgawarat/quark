// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief Win32のキーイベント
 */
#pragma once

#include <Windows.h>

#include "../win32/key.hpp"

namespace quark::inline win32 {
/**
 * @brief キーイベントを格納するクラス
 */
class KeyEvent final {
public:
  KeyEvent() = default;

  KeyEvent(const KBDLLHOOKSTRUCT& info) noexcept
      : key_{make_key(static_cast<uint16_t>(info.scanCode), !!(info.flags & LLKHF_EXTENDED))},
        pressed_{!(info.flags & LLKHF_UP)} {}

  Key key() const noexcept {
    return key_;
  }

  bool is_pressed() const noexcept {
    return pressed_;
  }

private:
  Key key_{};
  bool pressed_{};
};
}  // namespace quark::inline win32
