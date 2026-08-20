// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file wait.h
 * @brief 待機関数
 */
#pragma once

#include <stdint.h>

void wait_ms(uintptr_t ms);
void wait_us(uintptr_t us);
#define waitInputPinDelay(...) ((void)0)
