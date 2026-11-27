// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file bitset.hpp
 * @brief ビットセット
 */
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <span>
#include <type_traits>

namespace quark {
/**
 * @brief ビットセットにおいて、ビットの位置を表現する型
 */
template <typename BitSet>
using BitSetPosition = std::remove_cvref_t<BitSet>::Position;

/**
 * @brief ビットセットにおいて、ビットを格納する数値列の要素型
 */
template <typename BitSet>
using BitSetValue = std::remove_cvref_t<BitSet>::Value;

/**
 * @brief ビットセットにおいて、ビットの位置として使用できる型の制約
 */
template <typename T, typename BitSet>
concept BitSetPositionFor = std::same_as<T, BitSetPosition<BitSet>>;

/**
 * @brief ビットセットにおいて、ビットの位置として使用できるレンジ型の制約
 */
template <typename R, typename BitSet>
concept BitSetPositionRangeFor = std::ranges::input_range<R> && std::ranges::sized_range<R> &&
                                 BitSetPositionFor<std::ranges::range_value_t<R>, BitSet>;

/**
 * @brief ビットセットにおいて、ビットを格納する数値列の要素として使用できる型の制約
 */
template <typename T, typename BitSet>
concept BitSetValueFor = std::same_as<T, BitSetValue<BitSet>>;

/**
 * @brief ビットセットにおいて、ビットを格納する数値列の要素として使用できるレンジ型の制約
 */
template <typename R, typename BitSet>
concept BitSetValueRangeFor = std::ranges::input_range<R> && std::ranges::sized_range<R> &&
                              BitSetValueFor<std::ranges::range_value_t<R>, BitSet>;

/**
 * @brief ビットセットにおいて、バイト列として使用できるレンジ型の制約
 */
template <typename R>
concept BitSetByteRange = std::ranges::input_range<R> && std::ranges::sized_range<R> &&
                          std::same_as<std::ranges::range_value_t<R>, std::byte>;

/**
 * @brief 固定長配列をビット列として管理するためのクラス
 *
 * @tparam N ビット列の大きさ
 * @tparam T ビットを格納する数値列の要素型
 */
template <size_t N, typename T = uintptr_t>
  requires(std::unsigned_integral<T> && !std::same_as<T, bool>)
class BitArray {
public:
  /**
   * @brief ビットを格納する数値列の要素型
   */
  using Element = T;

  /**
   * @brief ビットを格納する数値列の要素型（CVなし）
   */
  using Value = std::remove_cv_t<T>;

  /**
   * @brief ビット列における位置を表す型
   */
  using Position = size_t;

  /**
   * @brief ビット列の大きさ
   */
  static constexpr size_t EXTENT = N;

  /**
   * @brief 数値列の要素型のビット幅
   */
  static constexpr size_t WIDTH = sizeof(T) * CHAR_BIT;

  /**
   * @brief 数値列の要素数
   */
  static constexpr size_t HEIGHT = (N + WIDTH - 1) / WIDTH;

  /**
   * @brief 全体を0で初期化する
   */
  constexpr BitArray() noexcept : values_{} {}

  /**
   * @brief ビット位置の配列から構築する
   *
   * @param positions ビット位置の配列
   */
  template <BitSetPositionRangeFor<BitArray> R>
  constexpr explicit BitArray(R&& positions) noexcept : values_{} {
    insert(std::forward<R>(positions));
  }

  /**
   * @brief ビット位置の配列から構築する
   *
   * @param positions ビット位置の配列
   */
  constexpr BitArray(std::initializer_list<Position> positions) noexcept : values_{} {
    insert(positions);
  }

  /**
   * @brief 値の配列から構築する
   *
   * @param values 値の配列
   */
  template <BitSetValueRangeFor<BitArray> R>
  constexpr explicit BitArray(std::in_place_t, R&& values) noexcept {
    assign(std::forward<R>(values));
  }

  /**
   * @brief 値の配列から構築する
   *
   * @param values 値の配列
   */
  constexpr explicit BitArray(std::in_place_t, std::initializer_list<Value> values) noexcept {
    assign(values);
  }

  /**
   * @brief バイト列から構築する
   *
   * @param bytes バイト列
   */
  template <BitSetByteRange R>
  constexpr explicit BitArray(std::in_place_t, R&& bytes) noexcept {
    assign(std::forward<R>(bytes));
  }

  /**
   * @brief ビットの状態を取得する
   *
   * @param position ビット列における位置
   * @retval true ビットが1
   * @retval false ビットが0、または、positionが範囲外
   */
  [[nodiscard]] constexpr bool contains(Position position) const noexcept {
    if (position >= size()) return false;
    const auto index = position / WIDTH;
    const auto offset = position % WIDTH;
    if (index >= values_.size()) return false;
    // NOLINTNEXTLINE(*-avoid-unchecked-container-access)
    return (values_[index] & static_cast<Value>(Value{1} << offset)) != 0;
  }

  /**
   * @brief いずれかのビットが1か調べる
   */
  [[nodiscard]] constexpr bool any() const noexcept {
    Value acc{};
    for (const auto& value : values_) {
      acc |= value;
    }
    return acc != 0;
  }

  /**
   * @brief あるビット列の上位集合か調べる
   *
   * @param other 下位集合にあたるビットセット
   */
  [[nodiscard]] constexpr bool is_superset_of(const BitArray& other) const noexcept {
    // NOLINTNEXTLINE(readability-use-anyofallof)
    for (const auto& [lhs, rhs] : std::views::zip(values_, other.values_)) {
      if ((lhs & rhs) != rhs) return false;
    }
    return true;
  }

  /**
   * @brief ビット列の大きさを取得する
   */
  [[nodiscard]] constexpr size_t size() const noexcept {
    return N;
  }

  /**
   * @brief ビットを格納する数値列を取得する
   */
  [[nodiscard]] constexpr auto values() noexcept {
    return std::span{values_};
  }

  /**
   * @brief ビットを格納する数値列を取得する
   */
  [[nodiscard]] constexpr auto values() const noexcept {
    return std::span{values_};
  }

  /**
   * @brief ビットを1にする
   *
   * @param position ビット列における位置
   */
  constexpr void insert(Position position) noexcept {
    if (position >= size()) return;
    const size_t index = position / WIDTH;
    const size_t offset = position % WIDTH;
    if (index >= values_.size()) return;
    // NOLINTNEXTLINE(*-avoid-unchecked-container-access)
    values_[index] |= static_cast<Value>(Value{1} << offset);
  }

  /**
   * @brief ビットを1にする
   *
   * @param positions ビット列における位置の配列
   */
  template <BitSetPositionRangeFor<BitArray> R>
  constexpr void insert(R&& positions) noexcept {
    for (auto&& position : std::forward<R>(positions)) {
      insert(std::forward<decltype(position)>(position));
    }
  }

  /**
   * @brief ビットを0にする
   *
   * @param position ビット列における位置
   */
  constexpr void remove(Position position) noexcept {
    if (position >= size()) return;
    const size_t index = position / WIDTH;
    const size_t offset = position % WIDTH;
    if (index >= values_.size()) return;
    // NOLINTNEXTLINE(*-avoid-unchecked-container-access)
    values_[index] &= ~(Value{1} << offset);
  }

  /**
   * @brief ビットを0にする
   *
   * @param positions ビット列における位置の配列
   */
  template <BitSetPositionRangeFor<BitArray> R>
  constexpr void remove(R&& positions) noexcept {
    for (auto&& position : std::forward<R>(positions)) {
      remove(std::forward<decltype(position)>(position));
    }
  }

  /**
   * @brief 全体を0で初期化する
   */
  constexpr void clear() noexcept {
    for (auto& value : values_) {
      value = 0;
    }
  }

  /**
   * @brief 数値列を代入する
   *
   * 数値列は末尾に0が続く無限長の配列として扱われる。
   *
   * @param values 数値列
   */
  template <BitSetValueRangeFor<BitArray> R>
  constexpr void assign(const R& values) noexcept {
    const auto size = std::min(std::ranges::ssize(values), std::ranges::ssize(values_));
    auto [_, mid] = std::ranges::copy_n(std::ranges::begin(values), size, values_.begin());
    std::ranges::fill(mid, values_.end(), Value{});
    clear_unused_bits();
  }

  /**
   * @brief バイト列を代入する
   *
   * バイト列は末尾に0が続く無限長の配列として扱われる。
   *
   * @param bytes バイト列
   */
  template <BitSetByteRange R>
  constexpr void assign(const R& bytes) noexcept {
    const auto dest = std::as_writable_bytes(std::span{values_});
    const auto size = std::min(std::ranges::size(bytes), dest.size());
    auto [_, mid] = std::ranges::copy_n(std::ranges::begin(bytes), size, dest.begin());
    std::ranges::fill(mid, dest.end(), std::byte{});
    clear_unused_bits();
    if constexpr (std::endian::native == std::endian::big) {
      for (auto& value : values_) {
        value = std::byteswap(value);
      }
    }
  }

  /**
   * @brief ビット列の和を計算する
   *
   * @param other 右手側のビット列
   * @return *this |= other
   */
  constexpr BitArray& unite(const BitArray& other) noexcept {
    for (auto&& [lhs, rhs] : std::views::zip(values_, other.values_)) {
      lhs |= rhs;
    }
    return *this;
  }

  /**
   * @brief ビット列の差を計算する
   *
   * @param other 右手側のビット列
   * @return *this &= ~other
   */
  constexpr BitArray& subtract(const BitArray& other) noexcept {
    for (auto&& [lhs, rhs] : std::views::zip(values_, other.values_)) {
      lhs &= static_cast<Value>(~rhs);
    }
    return *this;
  }

  /**
   * @brief ビット列の和を計算する
   */
  [[nodiscard]] friend constexpr BitArray bit_union(
      const BitArray& lhs, const BitArray& rhs) noexcept {
    return BitArray{lhs}.unite(rhs);
  }

  /**
   * @brief ビット列の差分を計算する
   */
  [[nodiscard]] friend constexpr BitArray bit_difference(
      const BitArray& lhs, const BitArray& rhs) noexcept {
    return BitArray{lhs}.subtract(rhs);
  }

private:
  /**
   * @brief 末尾の未使用ビットを0にする
   */
  constexpr void clear_unused_bits() noexcept {
    if constexpr (const auto offset = N % WIDTH; offset > 0) {
      values_.back() &= static_cast<Value>((Value{1} << offset) - 1);
    }
  }

  std::array<T, HEIGHT> values_;  ///< ビットを格納する数値列
};

/**
 * @brief 1のビットを列挙するビュー
 */
template <size_t N, typename T = std::uintptr_t>
class BitOneView : public std::ranges::view_interface<BitOneView<N, T>> {
public:
  /**
   * @brief ビットを格納する数値列の要素型
   */
  using Element = T;

  /**
   * @brief ビットを格納する数値列の要素型（CVなし）
   */
  using Value = std::remove_cv_t<T>;

  /**
   * @brief ビット列における位置を表す型
   */
  using Position = size_t;

  /**
   * @brief ビット列の大きさ
   */
  static constexpr size_t EXTENT = N;

  /**
   * @brief 数値列の要素型のビット幅
   */
  static constexpr size_t WIDTH = sizeof(T) * CHAR_BIT;

  /**
   * @brief 数値列の要素数
   */
  static constexpr size_t HEIGHT = (N + WIDTH - 1) / WIDTH;

  class Iterator {
  public:
    using iterator_concept = std::forward_iterator_tag;

    using difference_type = ptrdiff_t;
    using value_type = size_t;

    using ValueIterator = std::span<const T, HEIGHT>::const_iterator;

    constexpr Iterator() noexcept = default;

    constexpr explicit Iterator(ValueIterator iter, Position position) noexcept
      : iter_{iter}, position_{position} {
      next();  // 最初の1に移動する
    }

    [[nodiscard]] constexpr Position operator*() const noexcept {
      return position_;
    }

    constexpr Iterator& operator++() noexcept {
      position_++;  // 現在地の1を計算に含めないようにする
      next();
      return *this;
    }

    constexpr Iterator operator++(int) noexcept {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    [[nodiscard]] constexpr bool operator==(const Iterator& other) const noexcept {
      return iter_ == other.iter_ && position_ == other.position_;
    }

  private:
    /**
     * @brief 現在地から昇順に1のビットを探す
     */
    constexpr void next() noexcept {
      while (position_ < N) {
        // 直前の1より上位に現れる次の1を探す
        const typename ValueIterator::difference_type index = position_ / WIDTH;
        const size_t offset = position_ % WIDTH;
        const auto value = iter_[index];  // NOLINT(*-avoid-unchecked-container-access)
        const auto count = static_cast<size_t>(std::countr_zero(static_cast<T>(value >> offset)));

        if (count < WIDTH) {
          // 1が見つかったので、そこに移動する
          position_ += count;
          break;
        }

        // 1が見つからなかったので、次の値に進む
        position_ += WIDTH - offset;
      }

      // 終端位置をビット列の大きさに揃える
      if (position_ > N) position_ = N;
    }

    ValueIterator iter_{};
    Position position_{};
  };

  /**
   * @brief 全体のビューを構築する
   */
  constexpr explicit BitOneView(const BitArray<N, T>& bits) noexcept : values_{bits.values()} {}

  /**
   * @brief 指定の範囲のビューを構築する
   */
  constexpr explicit BitOneView(
      const BitArray<N, T>& bits, Position first, Position last = N) noexcept
    : values_{bits.values()}, first_{first}, last_{last} {
    if (last_ < first_) last_ = first_;
  }

  [[nodiscard]] constexpr auto begin() const noexcept {
    return Iterator{values_.begin(), first_};
  }

  [[nodiscard]] constexpr auto end() const noexcept {
    return Iterator{values_.begin(), last_};
  }

private:
  std::span<const T, HEIGHT> values_;
  Position first_ = 0;
  Position last_ = N;
};

template <size_t N, typename T>
BitOneView(const BitArray<N, T>&) -> BitOneView<N, T>;
}  // namespace quark
