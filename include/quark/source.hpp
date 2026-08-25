// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.hpp
 * @brief OSから入力イベントを受け取る側
 */
#pragma once

#include <exception>

namespace quark {
/**
 * @brief Sourceの状態
 */
enum class SourceStatus {
  STOPPED,         ///< 停止中
  RUNNING,         ///< 動作中
  STOP_REQUESTED,  ///< 停止要求済
};

/**
 * @brief Sourceを始動させる
 *
 * @retval true 始動に成功
 * @retval false すでに始動している
 * @exception system_error スレッドの生成に失敗
 */
bool start_source();

/**
 * @brief Sourceを停止させる
 *
 * @exception system_error スレッドのjoinに失敗
 */
void stop_source();

/**
 * @brief Sourceの状態を取得する
 *
 * @return 現在のSourceの状態
 */
SourceStatus get_source_status() noexcept;

/**
 * @brief Sourceが異常停止したときに呼ばれる関数
 *
 * アプリケーション側で実装される。
 */
void on_source_error(const std::exception& e) noexcept;
}  // namespace quark
