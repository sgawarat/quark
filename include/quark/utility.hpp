// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file utility.hpp
 * @brief ユーティリティ
 */
#pragma once

namespace quark {
/**
 * @brief コピーを禁止するための基底クラス
 *
 * 適用したいクラスでprivate継承する。
 */
struct MoveOnly {
  MoveOnly() = default;
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(const MoveOnly&) = delete;
  MoveOnly& operator=(MoveOnly&&) = default;
  ~MoveOnly() = default;
};

/**
 * @brief コピーとムーブを禁止するための基底クラス
 *
 * 適用したいクラスでprivate継承する。
 */
struct Stationary {
  Stationary() = default;
  Stationary(const Stationary&) = delete;
  Stationary(Stationary&&) = default;
  Stationary& operator=(const Stationary&) = delete;
  Stationary& operator=(Stationary&&) = default;
  ~Stationary() = default;
};
}  // namespace quark
