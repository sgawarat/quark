// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file stable.h
 * @brief すべてのソースにインクルードされるヘッダ
 */
#pragma once

#ifdef _WIN32
#include <Windows.h>

// QMKと衝突するマクロを取り除く
#undef KEY_EVENT
#endif

// リリース時にQMKのデバッグ機能をオフにする
#ifdef NDEBUG
#define NO_DEBUG
#define NO_PRINT
#endif

// QMKが必要とする宣言や定義
#ifdef __cplusplus
extern "C" {
#endif

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
