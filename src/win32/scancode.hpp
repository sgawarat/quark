// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file scancode.hpp
 * @brief PS/2 Set1 スキャンコード
 */
#include <cstdint>

namespace tmk_desktop::inline win32 {
uint16_t keycode_to_scancode(uint8_t keycode) noexcept;
}  // namespace tmk_desktop::inline win32
