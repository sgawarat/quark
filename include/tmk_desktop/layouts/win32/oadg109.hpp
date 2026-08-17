/**
 * @file jp109.hpp
 * @brief 日本語配列
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#pragma once

#include <tmk_desktop/event.hpp>

extern "C" {
#include <quantum_keycodes.h>
#include <keymap_japanese.h>
}  // extern "C"

namespace tmk_desktop {
/**
 * @brief 日本語配列にあるキー
 *
 * 基本的にはPS/2 Set1コードと等価だが、
 * - PauseキーがNumLockの値(45)に割り当てられる
 * - NumLockキーがExtendedキーになっている
 * - RShiftキーがExtendedキーになっている
 */
enum KeyConst : Key {
  K_ESCAPE = 0x1,
  K_1 = 0x2,
  K_2 = 0x3,
  K_3 = 0x4,
  K_4 = 0x5,
  K_5 = 0x6,
  K_6 = 0x7,
  K_7 = 0x8,
  K_8 = 0x9,
  K_9 = 0xa,
  K_0 = 0xb,
  K_MINUS = 0xc,
  K_CIRCUMFLEX = 0xd,
  K_BACKSPACE = 0xe,
  K_TAB = 0xf,
  K_Q = 0x10,
  K_W = 0x11,
  K_E = 0x12,
  K_R = 0x13,
  K_T = 0x14,
  K_Y = 0x15,
  K_U = 0x16,
  K_I = 0x17,
  K_O = 0x18,
  K_P = 0x19,
  K_AT = 0x1a,
  K_LEFT_BRACKET = 0x1b,
  K_ENTER = 0x1c,
  K_LEFT_CTRL = 0x1d,
  K_A = 0x1e,
  K_S = 0x1f,
  K_D = 0x20,
  K_F = 0x21,
  K_G = 0x22,
  K_H = 0x23,
  K_J = 0x24,
  K_K = 0x25,
  K_L = 0x26,
  K_SEMICOLON = 0x27,
  K_COLON = 0x28,
  K_HANKAKU_ZENKAKU = 0x29,
  K_LEFT_SHIFT = 0x2a,
  K_RIGHT_BRACKET = 0x2b,
  K_Z = 0x2c,
  K_X = 0x2d,
  K_C = 0x2e,
  K_V = 0x2f,
  K_B = 0x30,
  K_N = 0x31,
  K_M = 0x32,
  K_COMMA = 0x33,
  K_DOT = 0x34,
  K_SLASH = 0x35,
  K_KP_ASTERISK = 0x37,
  K_LEFT_ALT = 0x38,
  K_SPACE = 0x39,
  K_CAPS_LOCK = 0x3a,
  K_F1 = 0x3b,
  K_F2 = 0x3c,
  K_F3 = 0x3d,
  K_F4 = 0x3e,
  K_F5 = 0x3f,
  K_F6 = 0x40,
  K_F7 = 0x41,
  K_F8 = 0x42,
  K_F9 = 0x43,
  K_F10 = 0x44,
  K_PAUSE = 0x45,
  K_SCROLL_LOCK = 0x46,
  K_KP_7 = 0x47,
  K_KP_8 = 0x48,
  K_KP_9 = 0x49,
  K_KP_MINUS = 0x4a,
  K_KP_4 = 0x4b,
  K_KP_5 = 0x4c,
  K_KP_6 = 0x4d,
  K_KP_PLUS = 0x4e,
  K_KP_1 = 0x4f,
  K_KP_2 = 0x50,
  K_KP_3 = 0x51,
  K_KP_0 = 0x52,
  K_KP_DOT = 0x53,
  K_F11 = 0x57,
  K_F12 = 0x58,
  K_KATAKANA_HIRAGANA = 0x70,
  K_BACKSLASH = 0x73,
  K_HENKAN = 0x79,
  K_MUHENKAN = 0x7b,
  K_YEN = 0x7d,
  K_MEDIA_PREV_TRACK = 0x110,
  K_MEDIA_NEXT_TRACK = 0x119,
  K_KP_ENTER = 0x11c,
  K_RCTRL = 0x11d,
  K_AUDIO_MUTE = 0x120,
  K_MEDIA_PLAY_PAUSE = 0x122,
  K_AUDIO_VOL_DOWN = 0x12e,
  K_AUDIO_VOL_UP = 0x130,
  K_KP_SLASH = 0x135,
  K_RIGHT_SHIFT = 0x136,
  K_PRINT_SCREEN = 0x137,
  K_RIGHT_ALT = 0x138,
  K_NUM_LOCK = 0x145,
  K_HOME = 0x147,
  K_UP = 0x148,
  K_PAGE_UP = 0x149,
  K_LEFT = 0x14b,
  K_RIGHT = 0x14d,
  K_END = 0x14f,
  K_DOWN = 0x150,
  K_PAGE_DOWN = 0x151,
  K_INSERT = 0x152,
  K_DELETE = 0x153,
  K_LEFT_GUI = 0x15b,
  K_RIGHT_GUI = 0x15c,
  K_APPLICATION = 0x15d,
};

extern bool is_tapping_key(Key key) noexcept {
  switch (key) {
    case K_HANKAKU_ZENKAKU:
    case K_CAPS_LOCK:
    case K_KATAKANA_HIRAGANA:
      return true;
  }
  return false;
}
}  // namespace tmk_desktop
