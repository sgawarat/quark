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

  KeyEvent(const KBDLLHOOKSTRUCT& info) noexcept : vk_{info.vkCode}, sc_{info.scanCode}, flags_{info.flags} {}

  Key key() const noexcept {
    return make_key(vk_, sc_, flags_);
  }

  bool is_pressed() const noexcept {
    return !(flags_ & LLKHF_UP);
  }

private:
  DWORD vk_{};
  DWORD sc_{};
  DWORD flags_{};
};
}  // namespace quark::inline win32
