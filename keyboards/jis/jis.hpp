// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file jis.hpp
 * @brief JIS配列
 */
#pragma once

#include <optional>

#include "quark/quantum/key.hpp"

namespace quark::jis {
#if QUARK_KEYBOARD_JIS_EXTRA_ROWS > 0
/**
 * @brief 追加のキーに対するmatrix上の位置を取得する。
 *
 * マルチメディアキーなどの追加のキーが必要な場合に、キーボードまたはキーマップによって実装される。
 *
 * @param keycode キー
 */
std::optional<keypos_t> get_extra_key_property(KeyCode keycode, KeyPropertyTag<keypos_t>);
#endif

/**
 * @brief 追加のキーに設定された情報を取得する
 *
 * @tparam T 設定された情報の型
 * @param keycode キー
 * @return std::optional<T> 設定された情報の値
 */
template <typename T>
std::optional<T> get_extra_key_property(KeyCode keycode) {
  return get_extra_key_property(keycode, KeyPropertyTag<T>{});
}
}  // namespace quark::jis
