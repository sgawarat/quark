// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file quantum.hpp
 * @brief Quantum管理
 */
#pragma once

#include "quark/quantum/key.hpp"
#include "quark/quantum/report.hpp"
#include "quark/utility.hpp"

namespace quark::inline quantum {
/**
 * @brief Quantumのホストを定義するための基底クラス
 */
class QuantumHost : private Pinned {
public:
  QuantumHost();

  virtual ~QuantumHost() noexcept;

protected:
  /**
   * @brief matrixに1をセットする
   */
  void matrix_set(KeyCode keycode);

  /**
   * @brief matrixに0をセットする
   */
  void matrix_reset(KeyCode keycode);

  /**
   * @brief matrix全体をゼロクリアする
   */
  void matrix_clear();

  /**
   * @brief 内部状態を更新する
   */
  void keyboard_task();

  /**
   * @brief キー入力イベントを処理する
   */
  virtual void send_report(const report_keyboard_t& report) = 0;

  /**
   * @brief NKRO時のキー入力イベントを処理する
   */
  virtual void send_report(const report_nkro_t& report) = 0;

  /**
   * @brief マウスの入力イベントを処理する
   */
  virtual void send_report(const report_mouse_t& report) = 0;

  /**
   * @brief エクストラキーの入力イベントを処理する
   */
  virtual void send_report(const report_extra_t& report) = 0;
};
}  // namespace quark::inline quantum
