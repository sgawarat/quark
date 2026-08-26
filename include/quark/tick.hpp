// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file tick.hpp
 * @brief tick
 */
#pragma once

#include <chrono>

#ifndef QUARK_TICK_FAST_RATE
/**
 * @brief 高速モードのtickレート（Hz）
 */
#define QUARK_TICK_FAST_RATE 1000
#endif

#ifndef QUARK_TICK_SLOW_RATE
/**
 * @brief 低速モードのtickレート（Hz）
 */
#define QUARK_TICK_SLOW_RATE 100
#endif

#ifndef QUARK_TICK_FAST_TIME
/**
 * @brief 高速モードにいる時間
 */
#define QUARK_TICK_FAST_TIME (std::chrono::milliseconds(500))
#endif
