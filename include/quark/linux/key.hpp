// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.hpp
 * @brief Linuxにおけるキー表現
 */
#pragma once

#include <cstdint>
#include <type_traits>

#include "quark/quantum/key.hpp"

namespace quark::inline evdev {
/**
 * @brief Linuxのキー表現
 */
enum class Key : uint16_t {};

constexpr auto operator+(Key self) noexcept {
  return static_cast<std::underlying_type_t<Key>>(self);
}

/**
 * @brief QuantumのキーコードからLinuxのキー表現へ変換する
 */
Key to_key(KeyCode keycode) noexcept;

/**
 * @brief Linuxのキー表現からQMKのキーコードへ変換する
 */
KeyCode to_keycode(Key keycode) noexcept;
}  // namespace quark::inline evdev
