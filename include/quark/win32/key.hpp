// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.hpp
 * @brief Win32におけるキー表現
 */
#pragma once

#include <cstdint>
#include <type_traits>

#include "quark/quantum/key.hpp"

namespace quark::inline win32 {
/**
 * @brief Win32のキー表現
 *
 * 標準的なキーボードドライバが返す仮想スキャンコード（のはず）
 *
 * 参考：https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#scan-codes
 */
enum class Key : uint16_t {
  NO = 0,  // 無効値
  ESCAPE = 0x0001,
  DIGIT_1 = 0x0002,
  DIGIT_2 = 0x0003,
  DIGIT_3 = 0x0004,
  DIGIT_4 = 0x0005,
  DIGIT_5 = 0x0006,
  DIGIT_6 = 0x0007,
  DIGIT_7 = 0x0008,
  DIGIT_8 = 0x0009,
  DIGIT_9 = 0x000a,
  DIGIT_0 = 0x000b,
  MINUS = 0x000c,
  EQUAL = 0x000d,  // en: OEM_PLUS, ja: OEM_7
  BACKSPACE = 0x000e,
  TAB = 0x000f,
  Q = 0x0010,
  W = 0x0011,
  E = 0x0012,
  R = 0x0013,
  T = 0x0014,
  Y = 0x0015,
  U = 0x0016,
  I = 0x0017,
  O = 0x0018,
  P = 0x0019,
  LEFT_BRACKET = 0x001a,   // en: OEM_4, ja: OEM_3
  RIGHT_BRACKET = 0x001b,  // en: OEM_6, ja: OEM_4
  ENTER = 0x001c,
  LEFT_CTRL = 0x001d,
  A = 0x001e,
  S = 0x001f,
  D = 0x0020,
  F = 0x0021,
  G = 0x0022,
  H = 0x0023,
  J = 0x0024,
  K = 0x0025,
  L = 0x0026,
  SEMICOLON = 0x0027,  // en: OEM_1, ja: OEM_PLUS
  QUOTE = 0x0028,      // en: OEM_7, ja: OEM_1
  GRAVE = 0x0029,      // en: OEM_3, ja: OEM_AUTO（半角全角）
  LEFT_SHIFT = 0x002a,
  BACKSLASH = 0x002b,      // en: OEM_5, ja: OEM_6
  NONUS_HASH = BACKSLASH,  // 区別が難しいので、NONUS_HASHはBACKSLASHとして扱う
  Z = 0x002c,
  X = 0x002d,
  C = 0x002e,
  V = 0x002f,
  B = 0x0030,
  N = 0x0031,
  M = 0x0032,
  COMMA = 0x0033,
  DOT = 0x0034,
  SLASH = 0x0035,  // OEM_2
  RIGHT_SHIFT = 0x0036,
  KP_ASTERISK = 0x0037,
  LEFT_ALT = 0x0038,
  SPACE = 0x0039,
  CAPS_LOCK = 0x003a,  // en: CAPITAL, ja: OEM_ATTN（英数）
  F1 = 0x003b,
  F2 = 0x003c,
  F3 = 0x003d,
  F4 = 0x003e,
  F5 = 0x003f,
  F6 = 0x0040,
  F7 = 0x0041,
  F8 = 0x0042,
  F9 = 0x0043,
  F10 = 0x0044,
  PAUSE = 0x0045,  // ドライバ特有の挙動（NumLockと置き換え）
  SCROLL_LOCK = 0x0046,
  KP_7 = 0x0047,
  KP_8 = 0x0048,
  KP_9 = 0x0049,
  KP_MINUS = 0x004a,
  KP_4 = 0x004b,
  KP_5 = 0x004c,
  KP_6 = 0x004d,
  KP_PLUS = 0x004e,
  KP_1 = 0x004f,
  KP_2 = 0x0050,
  KP_3 = 0x0051,
  KP_0 = 0x0052,
  KP_DOT = 0x0053,
  SYSTEM_REQUEST = 0x0054,
  NONUS_BACKSLASH = 0x0056,  // OEM_102
  F11 = 0x0057,
  F12 = 0x0058,
  KP_EQUAL = 0x0059,
  INTERNATIONAL_6 = 0x005c,  // unused?
  F13 = 0x0064,
  F14 = 0x0065,
  F15 = 0x0066,
  F16 = 0x0067,
  F17 = 0x0068,
  F18 = 0x0069,
  F19 = 0x006a,
  F20 = 0x006b,
  F21 = 0x006c,
  F22 = 0x006d,
  F23 = 0x006e,
  INTERNATIONAL_2 = 0x0070,  // ja: OEM_COPY（かな）
  // LANGUAGE_2 = 0x0071,  // こちらの可能性もあるらしい（未検証）
  // LANGUAGE_1 = 0x0072,
  INTERNATIONAL_1 = 0x0073,  // en: ABNT_C1, ja: OEM_102
  F24 = 0x0076,
  LANGUAGE_5 = F24,
  LANGUAGE_4 = 0x0077,
  LANGUAGE_3 = 0x0078,       // unused?
  INTERNATIONAL_4 = 0x0079,  // ja: CONVERT
  INTERNATIONAL_5 = 0x007b,  // ja: NONCONVERT
  INTERNATIONAL_3 = 0x007d,  // ja: OEM_5
  KP_COMMA = 0x007e,         // ABNT_C2

  MEDIA_PREV_TRACK = 0xe010,
  MEDIA_NEXT_TRACK = 0xe019,
  KP_ENTER = 0xe01c,
  RIGHT_CTRL = 0xe01d,
  AUDIO_MUTE = 0xe020,
  CALCULATOR = 0xe021,
  MEDIA_PLAY_PAUSE = 0xe022,
  MEDIA_STOP = 0xe024,
  AUDIO_VOL_DOWN = 0xe02e,
  AUDIO_VOL_UP = 0xe030,
  WWW_HOME = 0xe032,
  KP_SLASH = 0xe035,
  PRINT_SCREEN = 0xe037,
  RIGHT_ALT = 0xe038,
  NUM_LOCK = 0xe045,  // ドライバ特有の挙動（EXTENDEDが付く）
  CANCEL = 0xe046,
  HOME = 0xe047,
  UP = 0xe048,
  PAGE_UP = 0xe049,
  LEFT = 0xe04b,
  RIGHT = 0xe04d,
  END = 0xe04f,
  DOWN = 0xe050,
  PAGE_DOWN = 0xe051,
  INSERT = 0xe052,
  DELETE = 0xe053,
  LEFT_GUI = 0xe05b,
  RIGHT_GUI = 0xe05c,
  APPLICATION = 0xe05d,
  SYSTEM_POWER = 0xe05e,
  SYSTEM_SLEEP = 0xe05f,
  SYSTEM_WAKE = 0xe063,
  WWW_SEARCH = 0xe065,
  WWW_FAVORITES = 0xe066,
  WWW_REFRESH = 0xe067,
  WWW_STOP = 0xe068,
  WWW_FORWARD = 0xe069,
  WWW_BACK = 0xe06a,
  MY_COMPUTER = 0xe06b,
  MAIL = 0xe06c,
  MEDIA_SELECT = 0xe06d,
  LANGUAGE_2 = 0xe0f1,  // ja: IME_OFF
  LANGUAGE_1 = 0xe0f2,  // ja: IME_ON
};

/**
 * @brief Win32のキー表現を値に変換する
 */
constexpr auto operator+(Key self) noexcept {
  return static_cast<std::underlying_type_t<Key>>(self);
}

/**
 * @brief QuantumのキーコードからWin32のキー表現へ変換する
 */
Key to_key(KeyCode keycode) noexcept;

/**
 * @brief Win32のキー表現からQMKのキーコードへ変換する
 */
KeyCode to_keycode(Key key) noexcept;
}  // namespace quark::inline win32
