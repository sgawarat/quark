// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyset.hpp
 * @brief キーセット
 */
#pragma once

#include <cstdint>
#include <span>
#include <utility>

#include "quark/bitset.hpp"
#include "quark/quantum/key.hpp"
#include "quark/quantum/report.hpp"
#include "report.h"

namespace quark {
/**
 * @brief キーのビットセット
 */
class KeySet {
public:
  /**
   * @brief 全体を0で初期化する
   */
  constexpr KeySet() noexcept = default;

  /**
   * @brief キーの配列から構築する
   */
  constexpr explicit KeySet(const report_keyboard_t& report) noexcept {
    for (const auto key : std::span{report.keys}) {
      if (key != 0) bits_.insert(key);
    }
  }

  /**
   * @brief キーのビット列から構築する
   */
  constexpr explicit KeySet(const report_nkro_t& report) noexcept
    : bits_{std::in_place, std::as_bytes(std::span{report.bits})} {}

  /**
   * @brief 1になっているビットを列挙する
   *
   * @param fn キーコードを引数とする関数オブジェクト
   */
  constexpr void scan(std::invocable<KeyCode> auto fn) const {
    for (const auto i : BitOneView{bits_}) {
      fn(KeyCode{static_cast<KeyCodeValue>(i)});
    }
  }

  /**
   * @brief AND NOT演算を行う
   *
   * @param other NOTされる側
   * @return lhs & ~rhs
   */
  [[nodiscard]] constexpr KeySet and_not(const KeySet& other) const noexcept {
    return KeySet{bits_.difference(other.bits_)};
  }

private:
  static constexpr size_t BIT_EXTENT = KEYCODE_COUNT;

  constexpr explicit KeySet(const BitArray<BIT_EXTENT>& bits) noexcept : bits_{bits} {}

  BitArray<BIT_EXTENT> bits_;
};

/**
 * @brief 修飾キーのビットセット
 */
class ModKeySet {
public:
  /**
   * @brief 全体を0で初期化する
   */
  constexpr ModKeySet() noexcept = default;

  /**
   * @brief 値から構築する
   */
  constexpr explicit ModKeySet(const report_keyboard_t& report) noexcept
    : bits_{std::in_place, {report.mods}} {}

  /**
   * @brief 値から構築する
   */
  constexpr explicit ModKeySet(const report_nkro_t& report) noexcept
    : bits_{std::in_place, {report.mods}} {}

  /**
   * @brief 1になっているビットを列挙する
   *
   * @param fn キーコードを引数とする関数オブジェクト
   */
  constexpr void scan(std::invocable<KeyCode> auto fn) const {
    for (const auto i : BitOneView{bits_}) {
      fn(KeyCode{static_cast<KeyCodeValue>(KC_LEFT_CTRL + i)});
    }
  }

  /**
   * @brief AND NOT演算を行う
   *
   * @param other NOTされる側
   * @return lhs & ~rhs
   */
  [[nodiscard]] constexpr ModKeySet and_not(const ModKeySet& other) const noexcept {
    return ModKeySet{bits_.difference(other.bits_)};
  }

private:
  using Value = decltype(report_keyboard_t::mods);

  static constexpr size_t BIT_EXTENT = sizeof(Value) * CHAR_BIT;

  constexpr explicit ModKeySet(BitArray<BIT_EXTENT, Value> bits) noexcept : bits_{bits} {}

  BitArray<BIT_EXTENT, Value> bits_;
};

/**
 * @brief マウスボタンのビットセット
 */
class MouseButtonSet {
public:
  /**
   * @brief 全体を0で初期化する
   */
  constexpr MouseButtonSet() noexcept = default;

  /**
   * @brief 値から構築する
   */
  constexpr explicit MouseButtonSet(const report_mouse_t& report) noexcept
    : bits_{std::in_place, {report.buttons}} {}

  /**
   * @brief いずれかのビットが1かを調べる
   */
  constexpr explicit operator bool() const noexcept {
    return bits_.any();
  }

  /**
   * @brief 1になっているビットを列挙する
   *
   * @param fn ビット位置を引数とする関数オブジェクト
   */
  constexpr void scan(std::invocable<uint8_t> auto fn) const {
    for (const auto i : BitOneView{bits_}) {
      fn(static_cast<uint8_t>(i));
    }
  }

  /**
   * @brief AND NOT演算を行う
   *
   * @param other NOTされる側
   * @return lhs & ~rhs
   */
  [[nodiscard]] constexpr MouseButtonSet and_not(const MouseButtonSet& other) const noexcept {
    return MouseButtonSet{bits_.difference(other.bits_)};
  }

private:
  using Value = decltype(report_mouse_t::buttons);

  static constexpr size_t BIT_EXTENT = sizeof(Value) * CHAR_BIT;

  constexpr explicit MouseButtonSet(BitArray<BIT_EXTENT, Value> bits) noexcept : bits_{bits} {}

  BitArray<BIT_EXTENT, Value> bits_;
};
}  // namespace quark
