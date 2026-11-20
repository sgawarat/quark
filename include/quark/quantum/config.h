// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file config.h
 * @brief configヘッダ
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// N番目の引数を返すマクロ
#define QUARK_PP_SELECT_0(_0, ...) _0
#define QUARK_PP_SELECT_1(_0, _1, ...) _1
#define QUARK_PP_SELECT_2(_0, _1, _2, ...) _2
#define QUARK_PP_SELECT_3(_0, _1, _2, _3, ...) _3
#define QUARK_PP_SELECT_4(_0, _1, _2, _3, _4, ...) _4
#define QUARK_PP_SELECT_5(_0, _1, _2, _3, _4, _5, ...) _5
#define QUARK_PP_SELECT_6(_0, _1, _2, _3, _4, _5, _6, ...) _6
#define QUARK_PP_SELECT_7(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define QUARK_PP_SELECT_8(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define QUARK_PP_SELECT_9(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9
#define QUARK_PP_SELECT(n, ...) QUARK_PP_SELECT_##n(__VA_ARGS__)

// 引数が空か調べるマクロ
#define QUARK_PP_EMPTY(...) QUARK_PP_SELECT_0(__VA_OPT__(0, ) 1)

// configヘッダリストからN番目の文字列を取り出すマクロ
#define QUARK_CONFIG_HEADER(n) \
  QUARK_PP_SELECT(n, QUARK_CONFIG_HEADER_LIST, , , , , , , , , , , , , , , )

// N番目のconfigヘッダが存在するか調べるマクロ
#define QUARK_HAS_CONFIG_HEADER(n) !QUARK_PP_EMPTY(QUARK_CONFIG_HEADER(n))

// configヘッダをインクルードする
// IWYU pragma: begin_exports
#if QUARK_HAS_CONFIG_HEADER(0)
#include QUARK_CONFIG_HEADER(0)
#endif
#if QUARK_HAS_CONFIG_HEADER(1)
#include QUARK_CONFIG_HEADER(1)
#endif
#if QUARK_HAS_CONFIG_HEADER(2)
#include QUARK_CONFIG_HEADER(2)
#endif
#if QUARK_HAS_CONFIG_HEADER(3)
#include QUARK_CONFIG_HEADER(3)
#endif
#if QUARK_HAS_CONFIG_HEADER(4)
#include QUARK_CONFIG_HEADER(4)
#endif
#if QUARK_HAS_CONFIG_HEADER(5)
#include QUARK_CONFIG_HEADER(5)
#endif
#if QUARK_HAS_CONFIG_HEADER(6)
#include QUARK_CONFIG_HEADER(6)
#endif
#if QUARK_HAS_CONFIG_HEADER(7)
#include QUARK_CONFIG_HEADER(7)
#endif
#if QUARK_HAS_CONFIG_HEADER(8)
#include QUARK_CONFIG_HEADER(8)
#endif
#if QUARK_HAS_CONFIG_HEADER(9)
#include QUARK_CONFIG_HEADER(9)
#endif
// IWYU pragma: end_exports

// リリース時にQMKのデバッグ機能をオフにする
#ifdef NDEBUG
#define NO_DEBUG
#define NO_PRINT
#endif

// 使用されているのに定義されないマクロを既定値で定義する
// platforms/eeprom
#define EEPROM_CUSTOM
#define EEPROM_SIZE 0

// joystick
#ifndef JOYSTICK_ENABLE
#define JOYSTICK_AXIS_RESOLUTION 8
#define JOYSTICK_AXIS_COUNT 2
#define JOYSTICK_BUTTON_COUNT 8
#endif

// keycode_string
#ifndef KEYCODE_STRING_ENABLE
#define KEYCODE_STRING_ENABLE 0
#endif

// eeconfig
#ifndef EECONFIG_KB_DATA_SIZE
#define EECONFIG_KB_DATA_SIZE 0
#endif
#ifndef EECONFIG_USER_DATA_SIZE
#define EECONFIG_USER_DATA_SIZE 0
#endif

// quantum
#ifndef SHUTDOWN_DELAY
#define SHUTDOWN_DELAY 0
#endif

#ifdef __cplusplus
}  // extern "C"
#endif
