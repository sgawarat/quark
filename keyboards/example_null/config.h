/**
 * @file config.h
 * @brief コンフィグ
 * @copyright Copyright 2026 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#pragma once

/**
 * @brief キーボードの状態を格納する2次元配列の列数 (1~32)
 *
 * matrix_row_tの型を決定するのに使われる。
 */
#define MATRIX_COLS 22

/**
 * @brief キーボードの状態を格納する2次元配列の行数
 *
 * matrix_row_tの配列の大きさを決定するのに使われる。
 */
#define MATRIX_ROWS 6
