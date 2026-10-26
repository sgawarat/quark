// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.hpp
 * @brief LinuxにおけるSink処理
 */
#pragma once

#include <memory>
#include <stop_token>

#include "quark/event.hpp"
#include "quark/linux/fd/uinput.hpp"

namespace quark::inline evdev {
/**
 * @brief Sink処理に必要な初期データを格納するためのクラス
 */
struct SinkContextDesc {};

/**
 * @brief Sink処理を行うためのクラス
 */
class SinkContext final : Pinned {
public:
  SinkContext(std::shared_ptr<SinkEventQueue> consumer, const SinkContextDesc& desc);

  ~SinkContext() noexcept;

  /**
   * @brief ループ処理を行う
   *
   * 停止要求を受けるか異常停止するまで制御を返さない。
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   */
  void run(const std::stop_token& stop_token);

private:
  void update(EventTime time, TickEvent event);
  void update(EventTime time, KeyPressEvent event);
  void update(EventTime time, KeyReleaseEvent event);
  void update(EventTime time, MousePressEvent event);
  void update(EventTime time, MouseReleaseEvent event);
  void update(EventTime time, MouseWheelEvent event);
  void update(EventTime time, MouseCursorEvent event);
  void update(EventTime time, SinkCommand event);

  std::shared_ptr<SinkEventQueue> consumer_;
  UinputFd uinput_fd_;  ///< 入力イベントを送信するためのFD
};
}  // namespace quark::inline evdev
