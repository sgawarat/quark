// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyboard.hpp
 * @brief キーボードを動かす側
 */
#pragma once

#include <exception>
#include <future>
#include <variant>

#ifdef _WIN32
#include "win32/keyboard.hpp"
#endif

namespace quark {
/**
 * @brief Keyboardの状態
 */
enum class KeyboardStatus {
  STOPPED,         ///< 停止中
  RUNNING,         ///< 動作中
  STOP_REQUESTED,  ///< 停止要求済
};

/**
 * @brief シグナルの値
 */
enum class KeyboardSignal {
  CLEAR,  ///< すべてのキーを離す
  RESET,  ///< 初期状態に戻す
};

/**
 * @brief Keyboardに渡されるイベントを格納するクラス
 */
using KeyboardEvent = std::variant<std::monostate, KeyEvent, KeyboardSignal>;

/**
 * @brief Keyboardを始動させる
 *
 * @return 処理結果を返すためのfutureを返す。スレッドがすでに起動済みなら空を返す。
 * @exception system_error スレッドの生成に失敗
 */
std::future<void> start_keyboard();

/**
 * @brief Keyboardを停止させる
 *
 * @exception system_error スレッドのjoinに失敗
 */
void stop_keyboard();

/**
 * @brief Keyboardにイベントを送る
 *
 * @param event イベント
 * @exception bad_alloc メモリ確保に失敗
 * @exception system_error mutexのロックに失敗
 */
void send_to_keyboard(const KeyboardEvent& event);

/**
 * @brief Keyboardの状態を取得する
 *
 * @return 現在のKeyboardの状態
 */
KeyboardStatus get_keyboard_status() noexcept;

/**
 * @brief Keyboardが異常停止したときに呼ばれる関数
 *
 * アプリケーション側で実装される。
 */
void on_keyboard_error() noexcept;
}  // namespace quark
