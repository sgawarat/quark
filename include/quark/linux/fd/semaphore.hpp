// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief Semaphore式のEvent FD
 */
#pragma once

#include <sys/eventfd.h>
#include <unistd.h>

#include <cstddef>

#include "quark/linux/fd/base.hpp"
#include "quark/linux/result.hpp"

namespace quark::inline evdev {
/**
 * @brief Semaphore式のEvent FDを管理するためのクラス
 */
class SemaphoreEventFd final : public FdBase {
public:
  static Result<SemaphoreEventFd> create() noexcept {
    QUARK_EVDEV_RESULT_TRY(
        SemaphoreEventFd evfd, eventfd(0, EFD_SEMAPHORE | EFD_NONBLOCK | EFD_CLOEXEC));
    return evfd;
  }

  SemaphoreEventFd() noexcept = default;

  [[nodiscard]] Result<bool> acquire() const noexcept {
    eventfd_t value{};
    const int ret = eventfd_read(handle(), &value);
    if (ret < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) return false;
      return make_error(errno);
    }
    return true;
  }

  [[nodiscard]] Result<void> release() const noexcept {
    size_t retry_count = 0;
    while (true) {
      const int ret = eventfd_write(handle(), 1);
      if (ret < 0) [[unlikely]] {
        if (errno == EINTR && retry_count++ < MAX_RETRY_COUNT) continue;
        return make_error(errno);
      }
      break;
    }
    return {};
  }

private:
  explicit SemaphoreEventFd(int fd) noexcept : FdBase{fd} {}
};
}  // namespace quark::inline evdev
