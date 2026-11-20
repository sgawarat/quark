// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.hpp
 * @brief Win32におけるSink処理
 */
#pragma once

#include <Windows.h>

#include <memory>
#include <stop_token>

#include "quark/engine.hpp"
#include "quark/event.hpp"
#include "quark/queue.hpp"
#include "quark/utility.hpp"

namespace quark::inline win32 {
/**
 * @brief Sinkに送られるイベントを管理するキュー
 */
using SinkEventQueue = EventQueue<KeyPressEvent, KeyReleaseEvent, MousePressEvent,
    MouseReleaseEvent, MouseWheelEvent, MouseCursorEvent, EngineSignal>;

/**
 * @brief SinkEventQueueを流れるパケット
 */
using SinkEventPacket = SinkEventQueue::Packet;

/**
 * @brief Sink処理に必要な初期データを格納するためのクラス
 */
struct SinkContextDesc {};

/**
 * @brief Sink処理を行うためのクラス
 */
class SinkContext final : private Pinned {
public:
  explicit SinkContext(std::shared_ptr<SinkEventQueue> consumer, const SinkContextDesc& desc);

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
  void update(EventTime time, EngineSignal signal);

  std::shared_ptr<SinkEventQueue> consumer_;
  KeySet prev_keys_;                       ///< 直前のキー状態
  MouseButtonSet prev_mouse_buttons_;      ///< 直前のマウスボタン状態
  KeyCode last_key_;                       ///< 最後に押したキー
  EventClock::time_point repeat_period_;   ///< 次のキーリピートが発生する時刻
  EventClock::duration keyboard_delay_{};  ///< キーリピートの「表示までの待ち時間」
  EventClock::duration keyboard_speed_{};  ///< キーリピートの「繰り返し速度」
};
}  // namespace quark::inline win32
