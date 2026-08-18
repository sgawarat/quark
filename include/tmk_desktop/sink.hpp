/**
 * @file sink.hpp
 * @brief OSに入力イベントを送り出す側
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#pragma once

#include <exception>
#include <variant>
#include <cstdint>

#ifdef _WIN32
#include "win32/sink.hpp"
#endif

extern "C" {
#include <report.h>
}  // extern "C"

namespace tmk_desktop {
/**
 * @brief Sinkの状態
 */
enum class SinkStatus {
  RESET,     ///< リセット済み
  RUNNING,   ///< 動作中
  STOPPING,  ///< 停止しようとしている
  STOPPED,   ///< 停止した
};

/**
 * @brief シグナルの値
 */
enum class SinkSignal {
  KEY_REPEAT,      ///< キーリピート
  KEY_REPEAT_END,  ///< キーリピートが途切れた
  RESET,           ///< 状態をリセットする
};

/**
 * @brief Sinkに渡されるイベントを格納するクラス
 */
using SinkEvent = std::variant<report_keyboard_t, report_nkro_t, report_mouse_t, report_extra_t, NativeSinkEvent, SinkSignal>;

/**
 * @brief Sinkを始動させる
 *
 * @retval true 始動に成功
 * @retval false すでに始動している
 * @exception system_error スレッドの生成に失敗
 * @exception e 設定の読み込みに失敗
 */
bool start_sink();

/**
 * @brief Sinkを停止させる
 *
 * @retval true 停止に成功
 * @retval false すでに停止しているか、停止しようとしている
 * @exception system_error スレッドのjoinに失敗
 */
bool stop_sink();

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
void on_sink_error(std::exception& e) noexcept;
}  // namespace tmk_desktop
