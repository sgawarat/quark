// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief Win32のキーイベント
 */
#pragma once

#include <cstdint>

#include <Windows.h>

#include "../win32/key.hpp"

namespace quark::inline win32 {
/**
 * @brief キーイベントを格納するクラス
 */
class KeyEvent final {
public:
  KeyEvent() = default;

  KeyEvent(const KBDLLHOOKSTRUCT& info) noexcept : vk_(info.vkCode), sc_(info.scanCode), flags_(info.flags) {}

  Key key() const noexcept {
    return make_key(static_cast<uint16_t>(sc_), !!(flags_ & LLKHF_EXTENDED));
  }

  bool is_pressed() const noexcept {
    return !(flags_ & LLKHF_UP);
  }

private:
  [[maybe_unused]] DWORD vk_ = 0;
  DWORD sc_ = 0;
  DWORD flags_ = 0;
};
}  // namespace quark::inline win32
