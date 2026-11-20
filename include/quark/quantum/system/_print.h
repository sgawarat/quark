// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file _print.h
 * @brief print関数
 */
#pragma once

/**
 * @brief Quantumがログを出力するのに使うprint関数のマクロ
 */
#define print_printf(...) quark_log_printf(__VA_ARGS__)

/**
 * @brief printf形式でログを出力する
 */
[[gnu::format(printf, 1, 2)]]
void quark_log_printf(const char* fmt, ...);
