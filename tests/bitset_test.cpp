// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file bitset_test.cpp
 * @brief bitsetの単体テスト
 */
#include "quark/bitset.hpp"

#include <cstddef>
#include <cstdint>

using namespace quark;

static constexpr size_t BIT_EXTENT = 100;

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE("BitArray", T, uint8_t, uintptr_t) {
  SUBCASE("contains") {
    BitArray<BIT_EXTENT, T> bits;

    for (size_t i = 0; i < BIT_EXTENT; i++) {
      REQUIRE_FALSE(bits.contains(i));
      bits.insert(i);
      REQUIRE(bits.contains(i));
    }
  }

  SUBCASE("any") {
    REQUIRE_FALSE(BitArray<BIT_EXTENT, T>{}.any());
    REQUIRE(BitArray<BIT_EXTENT, T>{63U}.any());
    REQUIRE(BitArray<BIT_EXTENT, T>{64U}.any());
  }

  SUBCASE("ones") {
    const BitArray<BIT_EXTENT, T> bits;
    for (const auto i : BitOneView{bits}) {
      FAIL(i);
    }
  }
}
