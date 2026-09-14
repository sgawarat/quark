// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file scancode.hpp
 * @brief Win32のスキャンコード
 */
#pragma once

#include <cstdint>

extern "C" {
#include <keycode.h>
#include <report.h>
}  // extern "C"

namespace quark::inline win32 {
/**
 * @brief Win32のスキャンコードを表す列挙型
 */
enum class Scancode : uint16_t {
  SC_NO = 0x0000,
  SC_ESCAPE = 0x0001,
  SC_1 = 0x0002,  // NOLINT(misc-confusable-identifiers)
  SC_2 = 0x0003,
  SC_3 = 0x0004,
  SC_4 = 0x0005,
  SC_5 = 0x0006,
  SC_6 = 0x0007,
  SC_7 = 0x0008,
  SC_8 = 0x0009,
  SC_9 = 0x000a,
  SC_0 = 0x000b,  // NOLINT(misc-confusable-identifiers)
  SC_MINUS = 0x000c,
  SC_EQUAL = 0x000d,
  SC_BACKSPACE = 0x000e,
  SC_TAB = 0x000f,
  SC_Q = 0x0010,
  SC_W = 0x0011,
  SC_E = 0x0012,
  SC_R = 0x0013,
  SC_T = 0x0014,
  SC_Y = 0x0015,
  SC_U = 0x0016,
  SC_I = 0x0017,  // NOLINT(misc-confusable-identifiers)
  SC_O = 0x0018,  // NOLINT(misc-confusable-identifiers)
  SC_P = 0x0019,
  SC_LEFT_BRACKET = 0x001a,
  SC_RIGHT_BRACKET = 0x001b,
  SC_ENTER = 0x001c,
  SC_LEFT_CTRL = 0x001d,
  SC_A = 0x001e,
  SC_S = 0x001f,
  SC_D = 0x0020,
  SC_F = 0x0021,
  SC_G = 0x0022,
  SC_H = 0x0023,
  SC_J = 0x0024,
  SC_K = 0x0025,
  SC_L = 0x0026,
  SC_SEMICOLON = 0x0027,
  SC_QUOTE = 0x0028,
  SC_GRAVE = 0x0029,
  SC_LEFT_SHIFT = 0x002a,
  SC_BACKSLASH = 0x002b,
  SC_NONUS_HASH = SC_BACKSLASH,
  SC_Z = 0x002c,
  SC_X = 0x002d,
  SC_C = 0x002e,
  SC_V = 0x002f,
  SC_B = 0x0030,
  SC_N = 0x0031,
  SC_M = 0x0032,
  SC_COMMA = 0x0033,
  SC_DOT = 0x0034,
  SC_SLASH = 0x0035,
  SC_RIGHT_SHIFT = 0x0036,
  SC_KP_ASTERISK = 0x0037,
  SC_LEFT_ALT = 0x0038,
  SC_SPACE = 0x0039,
  SC_CAPS_LOCK = 0x003a,
  SC_F1 = 0x003b,
  SC_F2 = 0x003c,
  SC_F3 = 0x003d,
  SC_F4 = 0x003e,
  SC_F5 = 0x003f,
  SC_F6 = 0x0040,
  SC_F7 = 0x0041,
  SC_F8 = 0x0042,
  SC_F9 = 0x0043,
  SC_F10 = 0x0044,
  SC_PAUSE = 0x0045,
  SC_SCROLL_LOCK = 0x0046,
  SC_KP_7 = 0x0047,
  SC_KP_8 = 0x0048,
  SC_KP_9 = 0x0049,
  SC_KP_MINUS = 0x004a,
  SC_KP_4 = 0x004b,
  SC_KP_5 = 0x004c,
  SC_KP_6 = 0x004d,
  SC_KP_PLUS = 0x004e,
  SC_KP_1 = 0x004f,
  SC_KP_2 = 0x0050,
  SC_KP_3 = 0x0051,
  SC_KP_0 = 0x0052,
  SC_KP_DOT = 0x0053,
  SC_SYSTEM_REQUEST = 0x0054,
  SC_NONUS_BACKSLASH = 0x0056,
  SC_F11 = 0x0057,
  SC_F12 = 0x0058,
  SC_KP_EQUAL = 0x0059,
  SC_INTERNATIONAL_6 = 0x005c,
  SC_F13 = 0x0064,
  SC_F14 = 0x0065,
  SC_F15 = 0x0066,
  SC_F16 = 0x0067,
  SC_F17 = 0x0068,
  SC_F18 = 0x0069,
  SC_F19 = 0x006a,
  SC_F20 = 0x006b,
  SC_F21 = 0x006c,
  SC_F22 = 0x006d,
  SC_F23 = 0x006e,
  SC_INTERNATIONAL_2 = 0x0070,
  SC_INTERNATIONAL_1 = 0x0073,
  SC_F24 = 0x0076,
  SC_LANGUAGE_5 = 0x0076,
  SC_LANGUAGE_4 = 0x0077,
  SC_LANGUAGE_3 = 0x0078,
  SC_INTERNATIONAL_4 = 0x0079,
  SC_INTERNATIONAL_5 = 0x007b,
  SC_INTERNATIONAL_3 = 0x007d,
  SC_KP_COMMA = 0x007e,
  SC_LANGUAGE_2 = 0x00f1,  // or 0x0071
  SC_LANGUAGE_1 = 0x00f2,  // or 0x0072
  SC_MEDIA_PREV_TRACK = 0xe010,
  SC_MEDIA_NEXT_TRACK = 0xe019,
  SC_KP_ENTER = 0xe01c,
  SC_RIGHT_CTRL = 0xe01d,
  SC_AUDIO_MUTE = 0xe020,
  SC_CALCULATOR = 0xe021,
  SC_MEDIA_PLAY_PAUSE = 0xe022,
  SC_MEDIA_STOP = 0xe024,
  SC_AUDIO_VOL_DOWN = 0xe02e,
  SC_AUDIO_VOL_UP = 0xe030,
  SC_WWW_HOME = 0xe032,
  SC_KP_SLASH = 0xe035,
  SC_PRINT_SCREEN = 0xe037,
  SC_RIGHT_ALT = 0xe038,
  SC_NUM_LOCK = 0xe045,
  SC_CANCEL = 0xe046,
  SC_HOME = 0xe047,
  SC_UP = 0xe048,
  SC_PAGE_UP = 0xe049,
  SC_LEFT = 0xe04b,
  SC_RIGHT = 0xe04d,
  SC_END = 0xe04f,
  SC_DOWN = 0xe050,
  SC_PAGE_DOWN = 0xe051,
  SC_INSERT = 0xe052,
  SC_DELETE = 0xe053,
  SC_LEFT_GUI = 0xe05b,
  SC_RIGHT_GUI = 0xe05c,
  SC_APPLICATION = 0xe05d,
  SC_SYSTEM_POWER = 0xe05e,
  SC_SYSTEM_SLEEP = 0xe05f,
  SC_SYSTEM_WAKE = 0xe063,
  SC_WWW_SEARCH = 0xe065,
  SC_WWW_FAVORITES = 0xe066,
  SC_WWW_REFRESH = 0xe067,
  SC_WWW_STOP = 0xe068,
  SC_WWW_FORWARD = 0xe069,
  SC_WWW_BACK = 0xe06a,
  SC_MY_COMPUTER = 0xe06b,
  SC_MAIL = 0xe06c,
  SC_MEDIA_SELECT = 0xe06d,
};

/**
 * @brief キーコードをWin32のスキャンコードに変換する
 *
 * @param keycode キーコード
 * @return Scancode Win32のスキャンコード
 */
constexpr Scancode to_scancode(uint8_t keycode) noexcept {
  using enum Scancode;
  switch (keycode) {
    case KC_ESCAPE: return SC_ESCAPE;
    case KC_1: return SC_1;
    case KC_2: return SC_2;
    case KC_3: return SC_3;
    case KC_4: return SC_4;
    case KC_5: return SC_5;
    case KC_6: return SC_6;
    case KC_7: return SC_7;
    case KC_8: return SC_8;
    case KC_9: return SC_9;
    case KC_0: return SC_0;
    case KC_MINUS: return SC_MINUS;
    case KC_EQUAL: return SC_EQUAL;
    case KC_BACKSPACE: return SC_BACKSPACE;
    case KC_TAB: return SC_TAB;
    case KC_Q: return SC_Q;
    case KC_W: return SC_W;
    case KC_E: return SC_E;
    case KC_R: return SC_R;
    case KC_T: return SC_T;
    case KC_Y: return SC_Y;
    case KC_U: return SC_U;
    case KC_I: return SC_I;
    case KC_O: return SC_O;
    case KC_P: return SC_P;
    case KC_LEFT_BRACKET: return SC_LEFT_BRACKET;
    case KC_RIGHT_BRACKET: return SC_RIGHT_BRACKET;
    case KC_ENTER: return SC_ENTER;
    case KC_LEFT_CTRL: return SC_LEFT_CTRL;
    case KC_A: return SC_A;
    case KC_S: return SC_S;
    case KC_D: return SC_D;
    case KC_F: return SC_F;
    case KC_G: return SC_G;
    case KC_H: return SC_H;
    case KC_J: return SC_J;
    case KC_K: return SC_K;
    case KC_L: return SC_L;
    case KC_SEMICOLON: return SC_SEMICOLON;
    case KC_QUOTE: return SC_QUOTE;
    case KC_GRAVE: return SC_GRAVE;
    case KC_LEFT_SHIFT: return SC_LEFT_SHIFT;
    case KC_BACKSLASH: return SC_BACKSLASH;
    case KC_NONUS_HASH: return SC_NONUS_HASH;
    case KC_Z: return SC_Z;
    case KC_X: return SC_X;
    case KC_C: return SC_C;
    case KC_V: return SC_V;
    case KC_B: return SC_B;
    case KC_N: return SC_N;
    case KC_M: return SC_M;
    case KC_COMMA: return SC_COMMA;
    case KC_DOT: return SC_DOT;
    case KC_SLASH: return SC_SLASH;
    case KC_RIGHT_SHIFT: return SC_RIGHT_SHIFT;
    case KC_KP_ASTERISK: return SC_KP_ASTERISK;
    case KC_LEFT_ALT: return SC_LEFT_ALT;
    case KC_SPACE: return SC_SPACE;
    case KC_CAPS_LOCK: return SC_CAPS_LOCK;
    case KC_F1: return SC_F1;
    case KC_F2: return SC_F2;
    case KC_F3: return SC_F3;
    case KC_F4: return SC_F4;
    case KC_F5: return SC_F5;
    case KC_F6: return SC_F6;
    case KC_F7: return SC_F7;
    case KC_F8: return SC_F8;
    case KC_F9: return SC_F9;
    case KC_F10: return SC_F10;
    case KC_PAUSE: return SC_PAUSE;
    case KC_SCROLL_LOCK: return SC_SCROLL_LOCK;
    case KC_KP_7: return SC_KP_7;
    case KC_KP_8: return SC_KP_8;
    case KC_KP_9: return SC_KP_9;
    case KC_KP_MINUS: return SC_KP_MINUS;
    case KC_KP_4: return SC_KP_4;
    case KC_KP_5: return SC_KP_5;
    case KC_KP_6: return SC_KP_6;
    case KC_KP_PLUS: return SC_KP_PLUS;
    case KC_KP_1: return SC_KP_1;
    case KC_KP_2: return SC_KP_2;
    case KC_KP_3: return SC_KP_3;
    case KC_KP_0: return SC_KP_0;
    case KC_KP_DOT: return SC_KP_DOT;
    case KC_SYSTEM_REQUEST: return SC_SYSTEM_REQUEST;
    case KC_NONUS_BACKSLASH: return SC_NONUS_BACKSLASH;
    case KC_F11: return SC_F11;
    case KC_F12: return SC_F12;
    case KC_KP_EQUAL: return SC_KP_EQUAL;
    case KC_INTERNATIONAL_6: return SC_INTERNATIONAL_6;
    case KC_F13: return SC_F13;
    case KC_F14: return SC_F14;
    case KC_F15: return SC_F15;
    case KC_F16: return SC_F16;
    case KC_F17: return SC_F17;
    case KC_F18: return SC_F18;
    case KC_F19: return SC_F19;
    case KC_F20: return SC_F20;
    case KC_F21: return SC_F21;
    case KC_F22: return SC_F22;
    case KC_F23: return SC_F23;
    case KC_INTERNATIONAL_2: return SC_INTERNATIONAL_2;
    case KC_INTERNATIONAL_1: return SC_INTERNATIONAL_1;
    case KC_F24: return SC_F24;
    case KC_LANGUAGE_5: return SC_LANGUAGE_5;
    case KC_LANGUAGE_4: return SC_LANGUAGE_4;
    case KC_LANGUAGE_3: return SC_LANGUAGE_3;
    case KC_INTERNATIONAL_4: return SC_INTERNATIONAL_4;
    case KC_INTERNATIONAL_5: return SC_INTERNATIONAL_5;
    case KC_INTERNATIONAL_3: return SC_INTERNATIONAL_3;
    case KC_KP_COMMA: return SC_KP_COMMA;
    case KC_LANGUAGE_2: return SC_LANGUAGE_2;  // or 0x0071
    case KC_LANGUAGE_1: return SC_LANGUAGE_1;  // or 0x0072
    case KC_MEDIA_PREV_TRACK: return SC_MEDIA_PREV_TRACK;
    case KC_MEDIA_NEXT_TRACK: return SC_MEDIA_NEXT_TRACK;
    case KC_KP_ENTER: return SC_KP_ENTER;
    case KC_RIGHT_CTRL: return SC_RIGHT_CTRL;
    case KC_AUDIO_MUTE: return SC_AUDIO_MUTE;
    case KC_CALCULATOR: return SC_CALCULATOR;
    case KC_MEDIA_PLAY_PAUSE: return SC_MEDIA_PLAY_PAUSE;
    case KC_MEDIA_STOP: return SC_MEDIA_STOP;
    case KC_AUDIO_VOL_DOWN: return SC_AUDIO_VOL_DOWN;
    case KC_AUDIO_VOL_UP: return SC_AUDIO_VOL_UP;
    case KC_WWW_HOME: return SC_WWW_HOME;
    case KC_KP_SLASH: return SC_KP_SLASH;
    case KC_PRINT_SCREEN: return SC_PRINT_SCREEN;
    case KC_RIGHT_ALT: return SC_RIGHT_ALT;
    case KC_NUM_LOCK: return SC_NUM_LOCK;
    case KC_CANCEL: return SC_CANCEL;
    case KC_HOME: return SC_HOME;
    case KC_UP: return SC_UP;
    case KC_PAGE_UP: return SC_PAGE_UP;
    case KC_LEFT: return SC_LEFT;
    case KC_RIGHT: return SC_RIGHT;
    case KC_END: return SC_END;
    case KC_DOWN: return SC_DOWN;
    case KC_PAGE_DOWN: return SC_PAGE_DOWN;
    case KC_INSERT: return SC_INSERT;
    case KC_DELETE: return SC_DELETE;
    case KC_LEFT_GUI: return SC_LEFT_GUI;
    case KC_RIGHT_GUI: return SC_RIGHT_GUI;
    case KC_APPLICATION: return SC_APPLICATION;
    case KC_SYSTEM_POWER: return SC_SYSTEM_POWER;
    case KC_SYSTEM_SLEEP: return SC_SYSTEM_SLEEP;
    case KC_SYSTEM_WAKE: return SC_SYSTEM_WAKE;
    case KC_WWW_SEARCH: return SC_WWW_SEARCH;
    case KC_WWW_FAVORITES: return SC_WWW_FAVORITES;
    case KC_WWW_REFRESH: return SC_WWW_REFRESH;
    case KC_WWW_STOP: return SC_WWW_STOP;
    case KC_WWW_FORWARD: return SC_WWW_FORWARD;
    case KC_WWW_BACK: return SC_WWW_BACK;
    case KC_MY_COMPUTER: return SC_MY_COMPUTER;
    case KC_MAIL: return SC_MAIL;
    case KC_MEDIA_SELECT: return SC_MEDIA_SELECT;
  }
  return SC_NO;
}

/**
 * @brief Generic Desktop PageのコードからWin32のスキャンコードに変換する
 *
 * @param usage Generic Desktop Pageのコード
 * @return Scancode Win32のスキャンコード
 */
constexpr Scancode to_scancode(desktop_usages usage) noexcept {
  using enum Scancode;
  switch (usage) {
    case SYSTEM_POWER_DOWN: return SC_SYSTEM_POWER;
    case SYSTEM_SLEEP: return SC_SYSTEM_SLEEP;
    case SYSTEM_WAKE_UP: return SC_SYSTEM_WAKE;
    case SYSTEM_RESTART:
    case SYSTEM_DISPLAY_TOGGLE_INT_EXT: return SC_NO;
  }
  return SC_NO;
}

/**
 * @brief Consumer PageのコードからWin32のスキャンコードに変換する
 *
 * @param usage Consumer Pageのコード
 * @return Scancode Win32のスキャンコード
 */
constexpr Scancode to_scancode(consumer_usages usage) noexcept {
  using enum Scancode;
  switch (usage) {
    case SNAPSHOT:
    case BRIGHTNESS_UP:    // KC_BRIGHTNESS_UP
    case BRIGHTNESS_DOWN:  // KC_BRIGHTNESS_DOWN
    case TRANSPORT_RECORD:
    case TRANSPORT_FAST_FORWARD:          // KC_MEDIA_FAST_FORWARD
    case TRANSPORT_REWIND: return SC_NO;  // KC_MEDIA_REWIND
    case TRANSPORT_NEXT_TRACK: return SC_MEDIA_NEXT_TRACK;
    case TRANSPORT_PREV_TRACK: return SC_MEDIA_PREV_TRACK;
    case TRANSPORT_STOP: return SC_MEDIA_STOP;
    case TRANSPORT_EJECT:  // KC_MEDIA_EJECT
    case TRANSPORT_RANDOM_PLAY:
    case TRANSPORT_STOP_EJECT: return SC_NO;
    case TRANSPORT_PLAY_PAUSE: return SC_MEDIA_PLAY_PAUSE;
    case AUDIO_MUTE: return SC_AUDIO_MUTE;
    case AUDIO_VOL_UP: return SC_AUDIO_VOL_UP;
    case AUDIO_VOL_DOWN: return SC_AUDIO_VOL_DOWN;
    case AL_CC_CONFIG: return SC_MEDIA_SELECT;
    case AL_EMAIL: return SC_MAIL;
    case AL_CALCULATOR: return SC_CALCULATOR;
    case AL_LOCAL_BROWSER: return SC_MY_COMPUTER;
    case AL_LOCK:
    case AL_CONTROL_PANEL:  // KC_CONTROL_PANEL
    case AL_ASSISTANT:      // KC_ASSISTANT
    case AL_KEYBOARD_LAYOUT:
    case AC_NEW:
    case AC_OPEN:
    case AC_CLOSE:
    case AC_EXIT:
    case AC_MAXIMIZE:
    case AC_MINIMIZE:
    case AC_SAVE:
    case AC_PRINT:
    case AC_PROPERTIES:
    case AC_UNDO:
    case AC_COPY:
    case AC_CUT:
    case AC_PASTE:
    case AC_SELECT_ALL:
    case AC_FIND: return SC_NO;
    case AC_SEARCH: return SC_WWW_SEARCH;
    case AC_HOME: return SC_WWW_HOME;
    case AC_BACK: return SC_WWW_BACK;
    case AC_FORWARD: return SC_WWW_FORWARD;
    case AC_STOP: return SC_WWW_STOP;
    case AC_REFRESH: return SC_WWW_REFRESH;
    case AC_BOOKMARKS: return SC_WWW_FAVORITES;
    case AC_NEXT_KEYBOARD_LAYOUT_SELECT:
    case AC_DESKTOP_SHOW_ALL_WINDOWS:
    case AC_SOFT_KEY_LEFT: return SC_NO;
  }
  return SC_NO;
}
}  // namespace quark::inline win32
