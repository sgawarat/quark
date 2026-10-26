// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
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

#ifndef QUARK_TICK_INTERVAL
/**
 * @brief キーボードの更新間隔（ms）
 */
#define QUARK_TICK_INTERVAL 5
#endif

#ifndef QUARK_TICK_ACTIVE_TIME
/**
 * @brief キーボードをアクティブにする時間(ms)
 */
#define QUARK_TICK_ACTIVE_TIME 500
#endif

namespace quark {
/**
 * @brief キーボード処理を管理するクラス
 */
class KeyboardContext final : protected QuantumHost {
public:
  explicit KeyboardContext(
      std::shared_ptr<SourceEventQueue> consumer, std::shared_ptr<SinkEventQueue> producer);

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
  static constexpr auto TICK_INTERVAL = std::chrono::duration_cast<EventClock::duration>(
      std::chrono::milliseconds(QUARK_TICK_INTERVAL));

  static constexpr auto TICK_ACTIVE_TIME = std::chrono::duration_cast<EventClock::duration>(
      std::chrono::milliseconds(QUARK_TICK_ACTIVE_TIME));

  void update(EventTime time, TickEvent);
  void update(EventTime time, KeyPressEvent event);
  void update(EventTime time, KeyReleaseEvent event);
  void update(EventTime time, SourceCommand event);

  std::shared_ptr<SourceEventQueue> consumer_;
  std::shared_ptr<SinkEventQueue> producer_;
  EventClock::time_point tick_period_tp_;   ///< 次のtickの開始時間
  EventClock::time_point tick_timeout_tp_;  ///< アクティブモードの終了時間
  KeySet prev_keys_;                        ///< 直前のキー状態
  ModKeySet prev_mods_;                     ///< 直前の修飾キー状態
  MouseButtonSet prev_mouse_buttons_;       ///< 直前のマウスボタン状態
  KeyCode last_extra_key_;                  ///< 最後に押されたエクストラキー
};
}  // namespace quark
