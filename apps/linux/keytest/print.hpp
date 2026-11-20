// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file print.hpp
 * @brief print関数
 */

#include <linux/input.h>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace quark::key_test::inline evdev {
std::string_view get_key_name(uint32_t keycode);
void print_key_event(const input_keymap_entry& entry);
void print_key_event(size_t index, const input_event& event);
}  // namespace quark::key_test::inline evdev
