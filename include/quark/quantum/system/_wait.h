// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file wait.h
 * @brief 待機関数
 */
#pragma once

#include <stdint.h>

/**
 * @brief ミリ秒単位で待機する
 *
 * @param ms 待機する秒数（ミリ秒）
 */
void wait_ms(uintptr_t millisec);

/**
 * @brief マイクロ秒単位で待機する
 *
 * OSの構造上、マイクロ秒単位で細かく待機することが難しいので、ミリ秒以下は事実上待機しないと解釈する。
 *
 * @param us 待機する秒数（マイクロ秒）
 */
#define wait_us(microsec) wait_ms((microsec) / 1000)

/**
 * @brief 入力ピンを切り替えるために待機する
 *
 * matric_common.cで使われている。
 *
 * ハードウェアに関連するものなので、ソフトウェア実装では必要ない。
 */
#define waitInputPinDelay(...) ((void)0)
