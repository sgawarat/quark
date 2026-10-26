// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief Event FD
 */
#pragma once

#include <sys/eventfd.h>
#include <unistd.h>

#include <utility>

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
    return std::move(evfd);
  }

  EventFd() noexcept = default;

  [[nodiscard]] Result<void> write(eventfd_t value) const noexcept {
    QUARK_EVDEV_RESULT_CHECK(eventfd_write(handle(), value));
    return {};
  }

private:
  explicit EventFd(int fd) noexcept : FdBase{fd} {}
};
}  // namespace quark::inline evdev
