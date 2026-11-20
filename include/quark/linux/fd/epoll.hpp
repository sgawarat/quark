// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file epoll.hpp
 * @brief Epoll FD
 */
#pragma once

#include <sys/epoll.h>
#include <unistd.h>

#include <chrono>
#include <optional>
#include <utility>

#include "quark/linux/fd/base.hpp"
#include "quark/linux/result.hpp"

namespace quark::inline evdev {
/**
 * @brief Epoll FDを管理するためのクラス
 */
class EpollFd final : public FdBase {
public:
  static Result<EpollFd> create() noexcept {
    QUARK_EVDEV_RESULT_TRY(EpollFd epfd, epoll_create1(EPOLL_CLOEXEC));
    return std::move(epfd);
  }

  EpollFd() = default;

  [[nodiscard]] Result<void> add(int fd, const epoll_event& event) const noexcept {
    auto epoll_event = event;
    QUARK_EVDEV_RESULT_CHECK(epoll_ctl(handle(), EPOLL_CTL_ADD, fd, &epoll_event));
    return {};
  }

  [[nodiscard]] Result<void> del(int fd) const noexcept {
    QUARK_EVDEV_RESULT_CHECK(epoll_ctl(handle(), EPOLL_CTL_DEL, fd, nullptr));
    return {};
  }

  /**
   * @brief イベントが来るのを待つ
   *
   * @param timeout 制御を返すまでの時間（ミリ秒）
   * @return 受信したイベントがあればそれを、なければnulloptを返す。失敗時、errnoの値を返す。
   */
  [[nodiscard]] Result<std::optional<epoll_event>> wait_for(
      std::chrono::duration<int, std::milli> timeout) const noexcept {
    epoll_event event{};
    const int ret = epoll_wait(handle(), &event, 1, timeout.count());
    if (ret < 0) {
      if (errno == EINTR) return std::nullopt;
      return make_error(errno);
    }
    if (ret != 1) return std::nullopt;
    return std::optional{event};
  }

private:
  explicit EpollFd(int fd) noexcept : FdBase{fd} {}
};
}  // namespace quark::inline evdev
