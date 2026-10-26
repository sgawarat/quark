// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief イベント
 */
#pragma once

#include "quark/keyset.hpp"
#include "quark/quantum/key.hpp"
#include "quark/quantum/report.hpp"
#include "quark/queue.hpp"

namespace quark {
/**
 * @brief キーを押すイベント
 */
struct KeyPressEvent {
  KeyCode keycode;
};

/**
 * @brief キーを離すイベント
 */
struct KeyReleaseEvent {
  KeyCode keycode;
};

/**
 * @brief マウスボタンを押すイベント
 */
struct MousePressEvent {
  MouseButtonSet buttons;
};

/**
 * @brief マウスボタンを離すイベント
 */
struct MouseReleaseEvent {
  MouseButtonSet buttons;
};

/**
 * @brief マウスホイールを動かすイベント
 */
struct MouseWheelEvent {
  mouse_hv_report_t rel_v;
  mouse_hv_report_t rel_h;
};

/**
 * @brief マウスカーソルを動かすイベント
 */
struct MouseCursorEvent {
  mouse_xy_report_t rel_x;
  mouse_xy_report_t rel_y;
};

/**
 * @brief Source制御コマンド
 */
enum class SourceCommand : uint16_t {
  CLEAR,  ///< すべてのキーを離す
};

/**
 * @brief Sink制御コマンド
 */
enum class SinkCommand : uint16_t {};

/**
 * @brief SourceからKeyboardへ送られるイベントを管理するキュー
 */
using SourceEventQueue = EventQueue<KeyPressEvent, KeyReleaseEvent, SourceCommand>;

/**
 * @brief SourceEventQueueを流れるパケット
 */
using SourceEventPacket = SourceEventQueue::Packet;

/**
 * @brief KeyboardからSinkへ送られるイベントを管理するキュー
 */
using SinkEventQueue = EventQueue<KeyPressEvent, KeyReleaseEvent, MousePressEvent,
    MouseReleaseEvent, MouseWheelEvent, MouseCursorEvent, SinkCommand>;

/**
 * @brief SinkEventQueueを流れるパケット
 */
using SinkEventPacket = SinkEventQueue::Packet;
}  // namespace quark
