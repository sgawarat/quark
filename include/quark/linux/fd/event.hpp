// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief Event FD
 */
#pragma once

#include <sys/eventfd.h>
#include <unistd.h>

#include <cstddef>

#include "quark/linux/fd/base.hpp"
#include "quark/linux/result.hpp"

namespace quark::inline evdev {
/**
 * @brief Event FDを管理するためのクラス
 */
class EventFd final : public FdBase {
public:
  static Result<EventFd> create() noexcept {
    QUARK_EVDEV_RESULT_TRY(EventFd evfd, eventfd(0, EFD_CLOEXEC));
    return evfd;
  }

  static Result<EventFd> create_semaphore() noexcept {
    QUARK_EVDEV_RESULT_TRY(EventFd evfd, eventfd(0, EFD_SEMAPHORE | EFD_CLOEXEC));
    return evfd;
  }

  EventFd() noexcept = default;

  [[nodiscard]] Result<eventfd_t> read() const noexcept {
    size_t retry_count = 0;
    eventfd_t value{};
    while (true) {
      const int ret = eventfd_read(handle(), &value);
      if (ret < 0) [[unlikely]] {
        if (errno == EINTR && retry_count++ < MAX_RETRY_COUNT) continue;
        return make_error(errno);
      }
      break;
    }
    return value;
  }

  [[nodiscard]] Result<void> write(eventfd_t value) const noexcept {
    size_t retry_count = 0;
    while (true) {
      const int ret = eventfd_write(handle(), value);
      if (ret < 0) [[unlikely]] {
        if (errno == EINTR && retry_count++ < MAX_RETRY_COUNT) continue;
        return make_error(errno);
      }
      break;
    }
    return {};
  }

private:
  explicit EventFd(int fd) noexcept : FdBase{fd} {}
};
}  // namespace quark::inline evdev
