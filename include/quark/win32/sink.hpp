// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.hpp
 * @brief Win32のSink処理
 */
#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <stop_token>
#include <variant>

#include <Windows.h>

extern "C" {
#include <report.h>
}  // extern "C"

#include <quark/bitset.hpp>
#include <quark/utility.hpp>

namespace quark::inline win32 {
/**
 * @brief Sinkに渡されるシグナルを表す列挙型
 */
enum class SinkSignal {
  KEY_REPEAT,  ///< キーリピート
  RESET,       ///< 初期状態に戻す
};

/**
 * @brief Sinkに渡されるイベントを格納するクラス
 */
using SinkEvent = std::variant<std::monostate, report_keyboard_t, report_nkro_t, report_mouse_t,
    report_extra_t, SinkSignal>;

/**
 * @brief Sinkに渡されるイベントのためのキューを管理するクラス
 */
class SinkQueue final : private Stationary {
public:
  SinkQueue();

  ~SinkQueue() noexcept;

  /**
   * @brief キューにイベントを追加する
   */
  void push(const SinkEvent& event);

  /**
   * @brief キューからイベントを取り出す
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   * @param event 受け取ったイベントの格納先
   * @retval true イベントを受け取った
   * @retval false 停止要求を受け取った
   */
  bool pop(const std::stop_token& stop_token, SinkEvent& event);

private:
  std::deque<SinkEvent> queue_;
  std::mutex queue_mtx_;
  std::condition_variable_any queue_cv_;
};

/**
 * @brief Sink処理を行うためのクラス
 */
class SinkContext final : private Stationary {
public:
  explicit SinkContext(SinkQueue& queue) : queue_{queue} {}

  ~SinkContext() noexcept = default;

  /**
   * @brief ループ処理を行う
   *
   * 停止要求を受けるか異常停止するまで制御を返さない。
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   */
  void run(const std::stop_token& stop_token);

private:
  /**
   * @brief キーの状態を格納するためのビットセット型
   */
  using KeySet = BitSet<256>;

  /**
   * @brief 修飾キーの状態を格納するためのビットセット型
   */
  using ModKeySet = BitSet<8, uint8_t>;

  // イベントのvisitor
  void update(std::monostate) noexcept;
  void update(const report_keyboard_t& report) noexcept;
  void update(const report_nkro_t& report) noexcept;
  void update(const report_mouse_t& report) noexcept;
  void update(const report_extra_t& report) noexcept;
  void update(SinkSignal signal) noexcept;

  SinkQueue& queue_;
  KeySet prev_keys_;              ///< 直前のキー状態
  ModKeySet prev_mods_;           ///< 直前の修飾キー状態
  uint8_t prev_mouse_buttons_{};  ///< 直前のマウスボタン状態
  INPUT last_input_{};            ///< 最後に押したキーのイベント
};
}  // namespace quark::inline win32
