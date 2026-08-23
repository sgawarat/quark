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

namespace quark {
/**
 * @brief 押した後すぐに離した扱いにするキーかを取得する。
 *
 * レイアウトまたはキーマップによって実装される。
 *
 * @param key キー
 */
bool is_tapping_key(enum Key key) noexcept;

/**
 * @brief キーに対するmatrix上の位置を取得する。
 *
 * レイアウトまたはキーマップによって実装される。
 *
 * @param key キー
 * @return キーに対するmatrix上の位置。対応するキーがなければ、matrix範囲外の位置を返す。
 */
keypos_t key_to_keypos(enum Key key) noexcept;
}  // namespace quark
