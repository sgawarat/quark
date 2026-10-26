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
 */
struct Pinned {
  Pinned() = default;
  Pinned(const Pinned&) = delete;
  Pinned(Pinned&&) = delete;
  Pinned& operator=(const Pinned&) = delete;
  Pinned& operator=(Pinned&&) = delete;
  ~Pinned() = default;
};
}  // namespace quark
