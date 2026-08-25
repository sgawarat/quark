// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file injected.cpp
 * @brief 自前のINJECTEDフラグ
 */
#include "injected.hpp"

namespace quark::inline win32 {
volatile int extra_info_injected = 0;
}  // namespace quark::inline win32
