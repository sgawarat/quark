// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file print.hpp
 * @brief print関数
 */
#pragma once

#include <WinUser.h>

namespace quark::key_test {
void print_key_event(DWORD vk, DWORD scan);
void print_key_event(const KBDLLHOOKSTRUCT& info);
}  // namespace quark::key_test
