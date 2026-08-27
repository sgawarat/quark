// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file wait.h
 * @brief 待機関数
 */
#pragma once

#include <stdint.h>

#include <intrin.h>

/**
 * @brief ミリ秒単位で待機する
 *
 * @param ms 待機する秒数（ミリ秒）
 */
void wait_ms(uintptr_t ms);

/**
 * @brief マイクロ秒単位で待機する
 *
 * OSの構造上、マイクロ秒単位で細かく待機することが難しいので、ミリ秒以下は事実上待機しないと解釈する。
 *
 * @param us 待機する秒数（マイクロ秒）
 */
#define wait_us(us) wait_ms(us / 1000)

#define waitInputPinDelay(...) ((void)0)
