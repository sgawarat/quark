// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyboard.hpp
 * @brief キーボード処理
 */
#pragma once

#include <memory>
#include <stop_token>

#include "quark/event.hpp"
#include "quark/keyset.hpp"
#include "quark/quantum/quantum.hpp"
#include "quark/queue.hpp"

#ifdef _WIN32
#include "quark/win32/sink.hpp"
#elifdef __linux__
#include "quark/linux/sink.hpp"
#endif

#ifndef QUARK_TICK_INTERVAL
/**
 * @brief キーボードの更新間隔（ミリ秒）
 *
 * キーボードエンジンはこの時間間隔に基づいてtick eventを処理する。
 */
#define QUARK_TICK_INTERVAL 5
#endif

#ifndef QUARK_TICK_ACTIVE_TIME
/**
 * @brief キーボードをアクティブにする時間（ミリ秒）
 *
 * この時間を無入力のまま過ぎると、キーボードエンジンは休眠モードに入る。
 * 休眠モードでは処理負荷が低く抑えられる代わりにtick eventを処理しなくなる。
 */
#define QUARK_TICK_ACTIVE_TIME 500
#endif

namespace quark {
/**
 * @brief Keyboardへ送られるイベントを管理するキュー
 */
using KeyboardEventQueue = EventQueue<KeyPressEvent, KeyReleaseEvent, EngineSignal>;

/**
 * @brief KeyboardEventQueueを流れるパケット
 */
using KeyboardEventPacket = KeyboardEventQueue::Packet;

/**
 * @brief キーボード処理を管理するクラス
 */
class KeyboardContext final : protected QuantumHost {
public:
  explicit KeyboardContext(
      std::shared_ptr<KeyboardEventQueue> consumer, std::shared_ptr<SinkEventQueue> producer);

  ~KeyboardContext() noexcept override;

  /**
   * @brief ループ処理を行う
   *
   * 停止要求を受けるか異常停止するまで制御を返さない。
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   */
  void run(const std::stop_token& stop_token);

protected:
  void send_report(const report_keyboard_t& report) override;
  void send_report(const report_nkro_t& report) override;
  void send_report(const report_mouse_t& report) override;
  void send_report(const report_extra_t& report) override;

private:
  /**
   * @brief キーボードの更新間隔
   */
  static constexpr auto TICK_INTERVAL = std::chrono::duration_cast<EventClock::duration>(
      std::chrono::milliseconds(QUARK_TICK_INTERVAL));

  /**
   * @brief キーボードをアクティブにする時間
   */
  static constexpr auto TICK_ACTIVE_TIME = std::chrono::duration_cast<EventClock::duration>(
      std::chrono::milliseconds(QUARK_TICK_ACTIVE_TIME));

  void update(EventTime time, TickEvent);
  void update(EventTime time, KeyPressEvent event);
  void update(EventTime time, KeyReleaseEvent event);
  void update(EventTime time, EngineSignal signal);

  std::shared_ptr<KeyboardEventQueue> consumer_;
  std::shared_ptr<SinkEventQueue> producer_;
  EventClock::time_point tick_period_tp_;   ///< 次のtickの開始時間
  EventClock::time_point tick_timeout_tp_;  ///< アクティブモードの終了時間
  KeySet prev_keys_;                        ///< 直前のキー状態
  MouseButtonSet prev_mouse_buttons_;       ///< 直前のマウスボタン状態
  KeyCode last_extra_key_;                  ///< 最後に押されたエクストラキー
};
}  // namespace quark
