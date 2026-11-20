// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyboard.h
 * @brief キーマップの宣言のみ
 */
#pragma once

#ifdef QUARK_QUANTUM_KEYMAP_HEADER
#include QUARK_QUANTUM_KEYMAP_HEADER  // IWYU pragma: export
#else
#endif

#include <quantum.h>

#ifndef QUARK_KEYMAP_LAYERS
/**
 * @brief キーマップが定義するレイヤー数
 */
#define QUARK_KEYMAP_LAYERS MAX_LAYER
#endif

// キーマップをライブラリ化するので、ここでは宣言だけ行う
extern uint16_t PROGMEM keymaps[QUARK_KEYMAP_LAYERS][MATRIX_ROWS][MATRIX_COLS];  // NOLINT
