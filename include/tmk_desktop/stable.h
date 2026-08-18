/**
 * @file stable.h
 * @brief すべてのソースにインクルードされるヘッダ
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#pragma once

#ifdef _WIN32
#include <Windows.h>

// QMKと干渉するマクロを取り除く
// #undef IS_ERROR
// #undef DELETE
// #undef OUT
#undef KEY_EVENT
#undef DELETE
#endif

// QMKが必要とする宣言や定義
#ifdef __cplusplus
extern "C" {
#endif

// timer
#define FAST_TIMER_T_SIZE 32

// wait
void wait_ms(uintptr_t ms);
void wait_us(uintptr_t us);
#define waitInputPinDelay(...) ((void)0)

// platforms/eeprom
#define EEPROM_CUSTOM
#define EEPROM_SIZE 0

#ifdef __cplusplus
}  // extern "C"
#endif
