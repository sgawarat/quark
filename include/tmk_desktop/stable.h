/**
 * @file stable.h
 * @brief すべてのソースにインクルードされるヘッダ
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#pragma once

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef _WIN32
#include <Windows.h>

// TMKと干渉するマクロを取り除く
#undef IS_ERROR
#undef DELETE
#undef OUT
#endif

// TMKが必要とする宣言や定義
#ifdef __cplusplus
extern "C" {
#endif
void wait_ms(uintptr_t ms);
void wait_us(uintptr_t us);
inline void bootloader_jump() {}

#define PROGMEM
#define pgm_read_byte(p) *((unsigned char*)p)
#define pgm_read_word(p) *((uint16_t*)p)

// KEYBOARD_REPORT_SIZEを指定の値で定義させるため、PROTOCOL_PJRCを使う
#define PROTOCOL_PJRC
#ifdef NKRO_ENABLE
// 修飾キー（8ビット）とキーコード全種（256ビット）
#define KBD2_SIZE 33
#else
// 10キーくらい同時押しできれば大丈夫そう？
#define KBD2_SIZE 12
#endif

#ifdef __cplusplus
}  // extern "C"
#endif
