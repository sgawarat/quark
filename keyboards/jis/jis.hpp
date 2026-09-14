// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file jis.hpp
 * @brief JIS配列
 */
#pragma once

#include <optional>

#include <quark/key.hpp>

namespace quark::inline ascii {
using enum Key;
}  // namespace quark::inline ascii

namespace quark::jis {

// 言語固有のキー
constexpr Key K_CIRCUMFLEX = K_EQUAL;
constexpr Key K_AT = ascii::K_LEFT_BRACKET;
constexpr Key K_LEFT_BRACKET = ascii::K_RIGHT_BRACKET;
constexpr Key K_COLON = K_QUOTE;
constexpr Key K_HANKAKU_ZENKAKU = K_GRAVE;
constexpr Key K_RIGHT_BRACKET = ascii::K_BACKSLASH;
constexpr Key K_KATAKANA_HIRAGANA = K_INTERNATIONAL_2;
constexpr Key K_BACKSLASH = K_INTERNATIONAL_1;
constexpr Key K_HENKAN = K_INTERNATIONAL_4;
constexpr Key K_MUHENKAN = K_INTERNATIONAL_5;
constexpr Key K_YEN = K_INTERNATIONAL_3;
constexpr Key K_EISU = K_CAPS_LOCK;

#if QUARK_KEYBOARD_JIS_EXTRA_ROWS > 0
/**
 * @brief 追加のキーに対するmatrix上の位置を取得する。
 *
 * マルチメディアキーなどの追加のキーが必要な場合に、キーボードまたはキーマップによって実装される。
 *
 * @param key キー
 */
extern std::optional<keypos_t> get_extra_key_property(Key key, KeyPropertyTag<keypos_t>);
#endif

/**
 * @brief 追加のキーに設定された情報を取得する
 *
 * @tparam T 設定された情報の型
 * @param key キー
 * @return std::optional<T> 設定された情報の値
 */
template <typename T>
std::optional<T> get_extra_key_property(Key key) {
  return get_extra_key_property(key, KeyPropertyTag<T>{});
}
}  // namespace quark::jis
