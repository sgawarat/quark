// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.hpp
 * @brief Quantumのキー
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

extern "C" {
#include <keyboard.h>  // IWYU pragma: export
#include <keycodes.h>  // IWYU pragma: export
}

namespace quark::inline quantum {
/**
 * @brief Quantumのキー表現
 */
class KeyCode {
public:
  constexpr KeyCode() = default;

  // NOLINTNEXTLINE(*-explicit-constructor)
  constexpr KeyCode(qk_keycode_defines keycode) noexcept : value_{static_cast<uint8_t>(keycode)} {}

  constexpr explicit KeyCode(uint8_t value) noexcept : value_{value} {}

  constexpr uint8_t operator+() const noexcept {
    return value_;
  }

  friend auto operator<=>(const KeyCode& lhs, const KeyCode& rhs) noexcept = default;

private:
  uint8_t value_{};
};

/**
 * @brief Quantumのキー表現の値型
 */
using KeyCodeValue = decltype(+KeyCode{});

/**
 * @brief Quantumのキーコードの個数
 */
static constexpr size_t KEYCODE_COUNT = QK_BASIC_MAX - QK_BASIC + 1;

/**
 * @brief 修飾キーのキーコードの先頭
 */
static constexpr size_t KEYCODE_MOD_BEGIN = KC_LEFT_CTRL;

/**
 * @brief 修飾キーのキーコードの終端
 */
static constexpr size_t KEYCODE_MOD_END = KEYCODE_MOD_BEGIN + 8;

/**
 * @brief キーに設定された情報の種類を表すタグ型
 *
 * @tparam T 設定された情報の型
 */
template <typename T>
struct KeyPropertyTag {};

/**
 * @brief キーコードに対するmatrix上の位置を取得する。
 *
 * キーボードまたはキーマップによって実装される。
 *
 * @param keycode キーコード
 * @return キーコードに対するmatrix上の位置。対応するキーがなければ、matrix範囲外の位置を返す。
 */
extern keypos_t get_key_property(KeyCode keycode, KeyPropertyTag<keypos_t>);

/**
 * @brief 長押しできないキーのプロパティ値の型
 */
enum class NonHoldable : bool {
  NO = false,
  YES = true,
};

/**
 * @brief 長押しできないキーかを取得する。
 *
 * キーボードまたはキーマップによって実装される。
 *
 * @param keycode キーコード
 */
extern NonHoldable get_key_property(KeyCode keycode, KeyPropertyTag<NonHoldable>);

/**
 * @brief キーに設定された情報を取得する
 *
 * @tparam T 設定された情報の型
 * @param keycode キーコード
 * @return T 設定された情報の値
 */
template <typename T>
T get_key_property(KeyCode keycode) {
  return get_key_property(keycode, KeyPropertyTag<T>{});
}

/**
 * @brief キーコードの配列からkeyposの配列に変換する
 */
consteval auto define_layout(
    const std::array<std::array<KeyCode, MATRIX_COLS>, MATRIX_ROWS>& keycodes_list) noexcept {
  std::array<keypos_t, KEYCODE_COUNT> out{};
  for (auto& v : out) {
    v = {.col = UINT8_MAX, .row = UINT8_MAX};
  }
  for (uint8_t row = 0; const auto& keycodes : keycodes_list) {
    for (uint8_t col = 0; const auto& keycode : keycodes) {
      if (keycode != KC_NO) {
        out.at(+keycode) = {
          .col = col,
          .row = row,
        };
      }
      col++;
    }
    row++;
  }
  return out;
}
}  // namespace quark::inline quantum
