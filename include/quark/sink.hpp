// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.hpp
 * @brief OSに入力イベントを送り出す側
 */
#pragma once

#include <exception>
#include <variant>

#include <cstdint>

extern "C" {
#include <report.h>
}  // extern "C"

#ifdef _WIN32
#include "win32/sink.hpp"
#endif

namespace quark {
/**
 * @brief Sinkの状態
 */
enum class SinkStatus {
  STOPPED,         ///< 停止中
  RUNNING,         ///< 動作中
  STOP_REQUESTED,  ///< 停止要求済
};

/**
 * @brief シグナルの値
 */
enum class SinkSignal {
  KEY_REPEAT,  ///< キーリピート
  RESET,       ///< 初期状態に戻す
};

/**
 * @brief Sinkに渡されるイベントを格納するクラス
 */
using SinkEvent =
    std::variant<report_keyboard_t, report_nkro_t, report_mouse_t, report_extra_t, NativeSinkEvent, SinkSignal>;

/**
 * @brief Sinkを始動させる
 *
 * @retval true 始動に成功
 * @retval false すでに始動している
 * @exception system_error スレッドの生成に失敗
 */
bool start_sink();

/**
 * @brief Sinkを停止させる
 *
 * @exception system_error スレッドのjoinに失敗
 */
void stop_sink();

/**
 * @brief Sinkにイベントを送る
 *
 * @param event イベント
 * @exception bad_alloc メモリ確保に失敗
 * @exception system_error mutexのロックに失敗
 */
void send_to_sink(const SinkEvent& event);

/**
 * @brief Sinkの状態を取得する
 *
 * @return 現在のSinkの状態
 */
SinkStatus get_sink_status() noexcept;

/**
 * @brief Sinkが異常停止したときに呼ばれる関数
 *
 * アプリケーション側で実装される。
 */
void on_sink_error(const std::exception& e) noexcept;
}  // namespace quark
