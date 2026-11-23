// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file thread.hpp
 * @brief スレッド
 */
#pragma once

#include <thread>

namespace quark::inline evdev {
using Thread = std::jthread;
}  // namespace quark::inline evdev
