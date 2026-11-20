// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file print.hpp
 * @brief print関数
 */

#include <Windows.h>

namespace quark::keytest::inline win32 {
void print_key_event(DWORD vk, DWORD scan);
void print_key_event(const KBDLLHOOKSTRUCT& info);
}  // namespace quark::keytest::inline win32
