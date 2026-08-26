// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.hpp
 * @brief Win32のキー
 */
#pragma once

#include <cstdint>

#include <Windows.h>

namespace quark::inline win32 {

/**
 * @brief キーの個数
 */
static constexpr size_t KEY_COUNT = 0x200;

/**
 * @brief キーを表す値の型
 *
 * 値を連続させるためにExtendedキーを0x100以降に並べている。
 */
enum Key : uint16_t {
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
  K_EQUAL = 0xd,
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
  K_LEFT_BRACKET = 0x1a,
  K_RIGHT_BRACKET = 0x1b,
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
  K_QUOTE = 0x28,
  K_GRAVE = 0x29,
  K_LEFT_SHIFT = 0x2a,
  K_BACKSLASH = 0x2b,
  K_NONUS_HASH = 0x2b,
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
  K_RIGHT_SHIFT = 0x36,
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
  K_SYSTEM_REQUEST = 0x54,  // K_PRINT_SCREENに統合される
  K_F11 = 0x57,
  K_F12 = 0x58,
  K_KP_EQUAL = 0x59,
  K_INTERNATIONAL_6 = 0x5c,
  K_F13 = 0x64,
  K_F14 = 0x65,
  K_F15 = 0x66,
  K_F16 = 0x67,
  K_F17 = 0x68,
  K_F18 = 0x69,
  K_F19 = 0x6a,
  K_F20 = 0x6b,
  K_F21 = 0x6c,
  K_F22 = 0x6d,
  K_F23 = 0x6e,
  K_INTERNATIONAL_2 = 0x70,
  K_INTERNATIONAL_1 = 0x73,
  K_F24 = 0x76,
  K_LANGUAGE_5 = 0x76,
  K_LANGUAGE_4 = 0x77,
  K_LANGUAGE_3 = 0x78,
  K_INTERNATIONAL_4 = 0x79,
  K_INTERNATIONAL_5 = 0x7b,
  K_INTERNATIONAL_3 = 0x7d,
  K_KP_COMMA = 0x007e,
  K_LANGUAGE_2 = 0x00f1,
  K_LANGUAGE_1 = 0x00f2,
  K_MEDIA_PREV_TRACK = 0x110,
  K_MEDIA_NEXT_TRACK = 0x119,
  K_KP_ENTER = 0x11c,
  K_RIGHT_CTRL = 0x11d,
  K_AUDIO_MUTE = 0x120,
  K_CALCULATOR = 0x121,
  K_MEDIA_PLAY_PAUSE = 0x122,
  K_MEDIA_STOP = 0x124,
  K_AUDIO_VOL_DOWN = 0x12e,
  K_AUDIO_VOL_UP = 0x130,
  K_WWW_HOME = 0x132,
  K_KP_SLASH = 0x135,
  K_RIGHT_SHIFT_EXTENDED = 0x136,  // K_RIGHT_SHIFTに統合される
  K_PRINT_SCREEN = 0x137,
  K_RIGHT_ALT = 0x138,
  K_NUM_LOCK = 0x145,
  K_CANCEL = 0x146,  // K_PAUSEに統合される
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
  K_SYSTEM_SLEEP = 0x15f,
  K_WWW_SEARCH = 0x165,
  K_WWW_FAVORITES = 0x166,
  K_WWW_REFRESH = 0x167,
  K_WWW_STOP = 0x168,
  K_WWW_FORWARD = 0x169,
  K_WWW_BACK = 0x16a,
  K_MY_COMPUTER = 0x16b,
  K_MAIL = 0x16c,
  K_MEDIA_SELECT = 0x16d,
};

/**
 * @brief Win32スキャンコードをキー番号に変換する
 *
 * @param vk Win32仮想キーコード
 * @param sc Win32スキャンコード
 * @param flags LLKHFフラグ
 * @return Key 対応するキー番号
 */
inline Key make_key(DWORD vk, DWORD sc, DWORD flags) noexcept {
  // スキャンコードが不明な場合には仮想キーコードから復元を試みる
  if (sc == 0) sc = MapVirtualKey(vk, MAPVK_VK_TO_VSC_EX);

  const uint8_t scancode = static_cast<uint8_t>(sc & 0xff);
  const bool extended = !!(flags & LLKHF_EXTENDED) || (sc > 0xff);
  if (extended) {
    switch (scancode) {
      // RShiftがEXTENDEDありになることがあるので、EXTENDEDなしに統合する
      case 0x36: return K_RIGHT_SHIFT;

      // Break(0xe046)はCtrl+PauseなのでPauseに統合する
      case 0x46: return K_PAUSE;

      // その他
      default: return static_cast<Key>(uint16_t{scancode} | 0x100);
    }
  } else {
    switch (scancode) {
      // SysRq(0x54)はAlt+PrtScなのでPrtScに統合する
      case 0x54: return K_PRINT_SCREEN;

      // その他
      default: return static_cast<Key>(uint16_t{scancode});
    }
  }
}
}  // namespace quark::inline win32
