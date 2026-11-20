// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file event.hpp
 * @brief イベント
 */
#pragma once

#include "quark/keyset.hpp"
#include "quark/quantum/key.hpp"
#include "quark/quantum/report.hpp"

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
}  // namespace quark
