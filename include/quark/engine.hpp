// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file engine.hpp
 * @brief キーボードエンジン
 */
#pragma once

#include <memory>
#include <thread>

#include "quark/utility.hpp"

namespace quark {
/**
 * @brief Engineの制御シグナル
 */
enum class EngineSignal {
  CLEAR,  ///< すべてのキーを離す
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
   * @brief 内部状態を管理するためのクラス
   */
  class State;

  /**
   * @brief 内部状態を共有するためのクラス
   */
  using SharedState = std::shared_ptr<State>;

  SharedState state_;
  std::jthread sink_thread_;
  std::jthread keyboard_thread_;
  std::jthread source_thread_;
};
}  // namespace quark
