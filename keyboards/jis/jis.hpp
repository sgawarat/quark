// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file jis.hpp
 * @brief JIS配列
 */
#pragma once

#include <optional>

#include <quark/key.hpp>

namespace quark::jis {
// 言語固有のキー
constexpr Key K_CIRCUMFLEX = Key::K_EQUAL;
constexpr Key K_AT = Key::K_LEFT_BRACKET;
constexpr Key K_LEFT_BRACKET = Key::K_RIGHT_BRACKET;
constexpr Key K_COLON = Key::K_QUOTE;
constexpr Key K_HANKAKU_ZENKAKU = Key::K_GRAVE;
constexpr Key K_RIGHT_BRACKET = Key::K_BACKSLASH;
constexpr Key K_KATAKANA_HIRAGANA = Key::K_INTERNATIONAL_2;
constexpr Key K_BACKSLASH = Key::K_INTERNATIONAL_1;
constexpr Key K_HENKAN = Key::K_INTERNATIONAL_4;
constexpr Key K_MUHENKAN = Key::K_INTERNATIONAL_5;
constexpr Key K_YEN = Key::K_INTERNATIONAL_3;

#if QUARK_KEYBOARD_JIS_EXTRA_ROWS > 0
/**
 * @brief 追加のキーに対するmatrix上の位置を取得する。
 *
 * マルチメディアキーなどの追加のキーが必要な場合に、キーボードまたはキーマップによって実装される。
 *
 * @param key キー
 */
std::optional<keypos_t> extra_key_to_keypos(Key key) noexcept;
#endif
}  // namespace quark::jis
