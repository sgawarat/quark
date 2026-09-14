// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file engine.hpp
 * @brief キーボードエンジン
 */
#pragma once

#include <memory>
#include <thread>

#include <quark/utility.hpp>

#ifndef QUARK_TICK_ACTIVE_RATE
/**
 * @brief アクティブ状態の更新レート（Hz）
 */
#define QUARK_TICK_ACTIVE_RATE 1000
#endif

#ifndef QUARK_TICK_ACTIVE_TIME
/**
 * @brief アクティブ状態の終了時間
 */
#define QUARK_TICK_ACTIVE_TIME (std::chrono::milliseconds(500))
#endif

namespace quark {
/**
 * @brief Engineに渡されるシグナルを表す列挙型
 */
enum class EngineSignal {
  CLEAR,  ///< すべてのキーを離す
  RESET,  ///< 初期状態に戻す
};

/**
 * @brief キーボードエンジンを管理するためのクラス
 */
class Engine final : private MoveOnly {
public:
  Engine();

  ~Engine() noexcept;

  explicit operator bool() const noexcept {
    return !!state_;
  }

  /**
   * @brief 初期化する
   */
  void init();

  /**
   * @brief 処理を開始する
   *
   * @param on_error エラー発生時に呼び出される関数
   * @param data on_errorの引数
   */
  void start(void (*on_error)(void*) noexcept, void* data);

  /**
   * @brief 処理を停止する
   */
  void stop() noexcept;

  /**
   * @brief シグナルを送る
   */
  void send(EngineSignal signal);

private:
  /**
   * @brief 内部状態を管理するクラス
   */
  class State;

  /**
   * @brief 内部状態を管理するクラスのshared_ptr
   */
  using SharedState = std::shared_ptr<State>;

  SharedState state_;
  std::jthread sink_thread_;
  std::jthread keyboard_thread_;
  std::jthread source_thread_;
};
}  // namespace quark
