// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file base.hpp
 * @brief FD管理
 */
#pragma once

#include <unistd.h>

#include <cstddef>
#include <utility>

#include "quark/result.hpp"
#include "quark/utility.hpp"

namespace quark::inline evdev {
/**
 * @brief ファイルデスクリプタを管理するための基底クラス
 */
class FdBase : MoveOnly {
public:
  /**
   * @brief ファイルデスクリプタが有効か調べる
   */
  [[nodiscard]] bool valid() const noexcept {
    return fd_ >= 0;
  }

  /**
   * @brief ファイルデスクリプタを取得する
   */
  [[nodiscard]] int handle() const noexcept {
    return fd_;
  }

  /**
   * @brief 中身を入れ替える
   */
  friend void swap(FdBase& lhs, FdBase& rhs) noexcept {
    std::swap(lhs.fd_, rhs.fd_);
  }

protected:
  /**
   * @brief read/writeで失敗時にリトライする最大回数
   */
  static constexpr size_t MAX_RETRY_COUNT = 16;

  FdBase() = default;

  FdBase(FdBase&& other) noexcept : fd_{other.fd_} {
    other.fd_ = -1;
  }

  explicit FdBase(int fd) noexcept : fd_{fd} {}

  FdBase& operator=(FdBase&& other) noexcept {
    auto tmp{std::move(other)};
    swap(*this, tmp);
    return *this;
  }

  ~FdBase() noexcept {
    if (fd_ >= 0) close(fd_);
  }

private:
  int fd_{-1};
};
}  // namespace quark::inline evdev
