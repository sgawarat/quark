// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyset.hpp
 * @brief キーセット
 */
#pragma once

#include <cstdint>
#include <span>

#include "quark/bitset.hpp"
#include "quark/quantum/key.hpp"
#include "quark/quantum/report.hpp"

namespace quark {
/**
 * @brief キーのビットセット
 */
class KeySet : public BitArray<KEYCODE_COUNT> {
public:
  /**
   * @brief 全体を0で初期化する
   */
  constexpr KeySet() noexcept = default;

  /**
   * @brief BitArrayから構築する
   */
  // NOLINTNEXTLINE(*-explicit-constructor)
  constexpr KeySet(const BitArray& other) : BitArray{other} {}

  /**
   * @brief キーの配列から構築する
   */
  constexpr explicit KeySet(const report_keyboard_t& report) noexcept {
    for (const auto key : std::span{report.keys}) {
      if (key != 0) BitArray::insert(key);
    }
    add_mods(report.mods);
  }

  /**
   * @brief キーのビット列から構築する
   */
  constexpr explicit KeySet(const report_nkro_t& report) noexcept
    : BitArray{std::in_place, std::as_bytes(std::span{report.bits})} {
    add_mods(report.mods);
  }

  /**
   * @brief ビットが1か調べる
   */
  [[nodiscard]] constexpr bool contains(KeyCode keycode) const noexcept {
    return BitArray::contains(+keycode);
  }

  /**
   * @brief ビットを1にする
   */
  constexpr void insert(KeyCode keycode) noexcept {
    BitArray::insert(+keycode);
  }

  /**
   * @brief ビットを0にする
   */
  constexpr void remove(KeyCode keycode) noexcept {
    BitArray::remove(+keycode);
  }

  /**
   * @brief 1になっているビットを列挙する
   *
   * @param fn キーコードを引数とする関数オブジェクト
   */
  constexpr void scan(std::invocable<KeyCode> auto fn) const {
    for (const auto i : BitOneView{*this}) {
      fn(KeyCode{static_cast<KeyCodeValue>(i)});
    }
  }

  /**
   * @brief 通常のキーの中で1になっているビットを列挙する
   *
   * @param fn キーコードを引数とする関数オブジェクト
   */
  constexpr void scan_keys(std::invocable<KeyCode> auto fn) const {
    for (const auto i : BitOneView{*this, 0, KEYCODE_MOD_BEGIN}) {
      fn(KeyCode{static_cast<KeyCodeValue>(i)});
    }
  }

  /**
   * @brief 修飾キーの中で1になっているビットを列挙する
   *
   * @param fn キーコードを引数とする関数オブジェクト
   */
  constexpr void scan_mods(std::invocable<KeyCode> auto fn) const {
    for (const auto i : BitOneView{*this, KEYCODE_MOD_BEGIN, KEYCODE_MOD_END}) {
      fn(KeyCode{static_cast<KeyCodeValue>(i)});
    }
  }

  /**
   * @brief ビット列の変化から、押したキーのビット列を計算する
   *
   * @param prev 直前のビット列
   * @param next 直後のビット列
   * @return 押したキーを1とするビット列
   */
  [[nodiscard]] friend constexpr KeySet pressing(const KeySet& prev, const KeySet& next) noexcept {
    return bit_difference(next, prev);
  }

  /**
   * @brief ビット列の変化から、離したキーのビット列を計算する
   *
   * @param prev 直前のビット列
   * @param next 直後のビット列
   * @return 離したキーを1とするビット列
   */
  [[nodiscard]] friend constexpr KeySet releasing(const KeySet& prev, const KeySet& next) noexcept {
    return bit_difference(prev, next);
  }

private:
  /**
   * @brief reportのmodsをビット列に直接加える
   */
  constexpr void add_mods(uint8_t mods) noexcept {
    constexpr size_t index = KEYCODE_MOD_BEGIN / BitArray::WIDTH;
    constexpr size_t offset = KEYCODE_MOD_BEGIN % BitArray::WIDTH;
    static_assert(index < BitArray::HEIGHT);              // 配列の範囲外を指していないか
    static_assert(offset + CHAR_BIT <= BitArray::WIDTH);  // 配列の境界を跨いでいないか
    // NOLINTNEXTLINE(*-avoid-unchecked-container-access)
    values()[index] |= BitArray::Value{mods} << offset;
  }
};

using ReportMouseButtonType = decltype(report_mouse_t::buttons);
static constexpr size_t REPORT_MOUSE_BUTTON_COUNT = sizeof(ReportMouseButtonType) * CHAR_BIT;

/**
 * @brief マウスボタンのビットセット
 */
class MouseButtonSet : public BitArray<REPORT_MOUSE_BUTTON_COUNT, ReportMouseButtonType> {
public:
  /**
   * @brief 全体を0で初期化する
   */
  constexpr MouseButtonSet() noexcept = default;

  /**
   * @brief BitArrayから構築する
   */
  // NOLINTNEXTLINE(*-explicit-constructor)
  constexpr MouseButtonSet(const BitArray& other) : BitArray{other} {}

  /**
   * @brief 値から構築する
   */
  constexpr explicit MouseButtonSet(const report_mouse_t& report) noexcept
    : BitArray{std::in_place, {report.buttons}} {}

  /**
   * @brief 1になっているビットを列挙する
   *
   * @param fn ビット位置を引数とする関数オブジェクト
   */
  constexpr void scan(std::invocable<uint8_t> auto fn) const {
    for (const auto i : BitOneView{*this}) {
      fn(static_cast<uint8_t>(i));
    }
  }

  /**
   * @brief ビット列の変化から、押したボタンのビット列を計算する
   *
   * @param prev 直前のビット列
   * @param next 直後のビット列
   * @return 押したボタンを1とするビット列
   */
  [[nodiscard]] friend constexpr MouseButtonSet pressing(
      const MouseButtonSet& prev, const MouseButtonSet& next) noexcept {
    return bit_difference(next, prev);
  }

  /**
   * @brief ビット列の変化から、離したボタンのビット列を計算する
   *
   * @param prev 直前のビット列
   * @param next 直後のビット列
   * @return 離したボタンを1とするビット列
   */
  [[nodiscard]] friend constexpr MouseButtonSet releasing(
      const MouseButtonSet& prev, const MouseButtonSet& next) noexcept {
    return bit_difference(prev, next);
  }
};
}  // namespace quark
