// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief Win32のキーイベント
 */
#pragma once

#include <cstdint>
#include <Windows.h>
#include "key.hpp"

namespace tmk_desktop::inline win32 {
/**
 * @brief キーイベントを格納するクラス
 */
class KeyEvent final {
public:
  KeyEvent() = default;

  KeyEvent(const KBDLLHOOKSTRUCT& info) noexcept : vk_(info.vkCode), sc_(info.scanCode), flags_(info.flags) {}

  Key key() const noexcept {
    // HACK: 8ビットより大きなスキャンコードが現れないことを前提としている
    return static_cast<Key>(((flags_ & LLKHF_EXTENDED) ? 0x100 : 0) | (sc_ & 0xff));
  }

  bool is_pressed() const noexcept {
    return !(flags_ & LLKHF_UP);
  }

private:
  [[maybe_unused]] WORD vk_ = 0;
  WORD sc_ = 0;
  DWORD flags_ = 0;
};
}  // namespace tmk_desktop::inline win32
