// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.hpp
 * @brief キー
 */
#pragma once

extern "C" {
#include <keyboard.h>
}  // extern "C"

#ifdef _WIN32
#include "win32/key.hpp"
#endif

namespace tmk_desktop {
bool is_tapping_key(enum Key key) noexcept;
keypos_t key_to_keypos(enum Key key) noexcept;
}  // namespace tmk_desktop
