// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file engine.hpp
 * @brief キーボードエンジン
 */
#pragma once

#include <functional>
#include <memory>

#include "quark/utility.hpp"

#ifdef _WIN32
#include "quark/win32/thread.hpp"
#elifdef __linux__
#include "quark/linux/thread.hpp"
#endif

namespace quark {
/**
 * @brief Engineの制御シグナル
 */
enum class EngineSignal {
  SCREEN_LOCKED,    ///< ロック画面に入った
  SCREEN_UNLOCKED,  ///< ロック画面から出た
};

/**
 * @brief キーボードエンジンを管理するためのクラス
 */
class Engine final : private MoveOnly {
public:
  /**
   * @brief 既定値で構築する
   *
   * @param on_error エラー発生時に呼び出される関数。その呼出はスレッドセーフである必要がある。
   */
  Engine(std::function<void()> on_error);

  ~Engine() noexcept;

  explicit operator bool() const noexcept {
    return !!state_;
  }

  /**
   * @brief 処理を開始する
   */
  void start();

  /**
   * @brief 処理を停止する
   */
  void stop() noexcept;

  /**
   * @brief シグナルを送る
   */
  void send(EngineSignal signal);

private:
  class State;

  std::shared_ptr<State> state_;
  Thread sink_thread_;
  Thread keyboard_thread_;
  Thread source_thread_;
};
}  // namespace quark
