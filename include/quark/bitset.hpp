// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file bitset.hpp
 * @brief ビットセット
 */
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <ranges>
#include <span>

#include <cstdint>

namespace quark {
/**
 * @brief ビットセット
 *
 * @tparam N ビット数
 * @tparam T ビット列を格納する値の型
 */
template <size_t N, std::unsigned_integral T = unsigned int>
class BitSet {
public:
  /**
   * @brief ビット列を格納する値の型
   */
  using ValueType = T;

  /**
   * @brief 確保されたビット数
   */
  static constexpr size_t BIT_SIZE = N;

  /**
   * @brief 確保されたバイト数
   */
  static constexpr size_t BYTE_SIZE = (BIT_SIZE + 7) / 8;

  /**
   * @brief 確保された値の数
   */
  static constexpr size_t VALUE_SIZE = (BYTE_SIZE + sizeof(T) - 1) / sizeof(T);

  /**
   * @brief 値のビット幅
   */
  static constexpr size_t BIT_WIDTH = sizeof(T) * 8;

  /**
   * @brief 全体を0で初期化する
   */
  constexpr BitSet() noexcept : values_{} {}

  /**
   * @brief 全体を既定値で初期化する
   */
  constexpr explicit BitSet(T default_value) noexcept {
    for (auto& value : values_) {
      value = default_value;
    }
  }

  /**
   * @brief バイト列をコピーする
   *
   * @param bytes バイト列
   */
  template <size_t SIZE>
  constexpr explicit BitSet(std::span<const std::byte, SIZE> bytes) noexcept {
    const auto dest = std::as_writable_bytes(std::span{values_});
    if constexpr (SIZE >= BYTE_SIZE) {
      std::ranges::copy(bytes.template subspan<0, BYTE_SIZE>(), dest.begin());
    } else {
      const auto [_, mid] = std::ranges::copy(bytes, dest.begin());
      std::ranges::fill(mid, dest.end(), std::byte{0});
    }
    if constexpr (std::endian::native == std::endian::big && sizeof(T) > 1) {
      for (auto& value : values_) {
        value = std::byteswap(value);
      }
    }
  }

  /**
   * @brief 8ビット整数列をバイト列としてコピーする
   *
   * @param values バイト列
   */
  template <size_t SIZE>
  constexpr explicit BitSet(std::span<const uint8_t, SIZE> values) noexcept
    : BitSet{std::as_bytes(values)} {}

  /**
   * @brief ビットを1にする
   *
   * @param position ビット列における位置
   */
  constexpr void set(size_t position) noexcept {
    if (position >= BIT_SIZE) return;
    const auto index = position / BIT_WIDTH;
    const auto offset = position % BIT_WIDTH;
    if (index < values_.size()) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
      values_[index] |= static_cast<T>(1) << offset;
    }
  }

  /**
   * @brief 1になっているビットを列挙する
   *
   * @param fn ビット位置を引数とする関数オブジェクト
   */
  constexpr void scan(std::invocable<size_t> auto fn) const noexcept(noexcept(fn(size_t{}))) {
    for (size_t base = 0; auto value : values_) {
      size_t index = base;
      base += BIT_WIDTH;
      while (value != 0) {
        // 連続する0をスキップする
        const auto offset = std::countr_zero(value);
        index += static_cast<size_t>(offset);
        value >>= offset;

        // ビットをすべて見たら終了する
        if (index >= N) return;

        // 見つかった1を報告して0にする
        fn(index);
        value &= ~T{1};
      }
    }
  }

  /**
   * @brief AND NOT演算を行う
   *
   * @param other NOTされるビットセット
   * @return lhs & ~rhs
   */
  [[nodiscard]] constexpr BitSet and_not(const BitSet& other) const noexcept {
    BitSet out;
    for (auto&& [result, lhs, rhs] : std::views::zip(out.values_, values_, other.values_)) {
      result = static_cast<T>(lhs & ~rhs);
    }
    return out;
  }

private:
  std::array<T, VALUE_SIZE> values_;  ///< ビット列を格納する値の配列
};
}  // namespace quark
