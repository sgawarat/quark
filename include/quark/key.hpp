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
 * @brief キーに設定された情報の種類を表すタグ型
 *
 * @tparam T 設定された情報の型
 */
template <typename T>
struct KeyPropertyTag {};

/**
 * @brief キーに対するmatrix上の位置を取得する。
 *
 * キーボードまたはキーマップによって実装される。
 *
 * @param key キー
 * @return キーに対するmatrix上の位置。対応するキーがなければ、matrix範囲外の位置を返す。
 */
keypos_t get_key_property(Key key, KeyPropertyTag<keypos_t>);

/**
 * @brief 長押しできないキーのプロパティ値の型
 */
enum NonHoldable : bool {};

/**
 * @brief 長押しできないキーかを取得する。
 *
 * キーボードまたはキーマップによって実装される。
 *
 * @param key キー
 */
NonHoldable get_key_property(Key key, KeyPropertyTag<NonHoldable>);

/**
 * @brief キーに設定された情報を取得する
 *
 * @tparam T 設定された情報の型
 * @param key キー
 * @return T 設定された情報の値
 */
template <typename T>
T get_key_property(Key key) {
  return get_key_property(key, KeyPropertyTag<T>{});
}
}  // namespace quark
