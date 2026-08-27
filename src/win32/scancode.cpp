// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file scancode.cpp
 * @brief PS/2 Set1 スキャンコード
 */
#include "scancode.hpp"

extern "C" {
#include <keycode.h>
}  // extern "C"

namespace quark::inline win32 {
Scancode keycode_to_scancode(uint8_t keycode) noexcept {
  switch (keycode) {
    case KC_ESCAPE: return Scancode{0x0001};
    case KC_1: return Scancode{0x0002};
    case KC_2: return Scancode{0x0003};
    case KC_3: return Scancode{0x0004};
    case KC_4: return Scancode{0x0005};
    case KC_5: return Scancode{0x0006};
    case KC_6: return Scancode{0x0007};
    case KC_7: return Scancode{0x0008};
    case KC_8: return Scancode{0x0009};
    case KC_9: return Scancode{0x000a};
    case KC_0: return Scancode{0x000b};
    case KC_MINUS: return Scancode{0x000c};
    case KC_EQUAL: return Scancode{0x000d};
    case KC_BACKSPACE: return Scancode{0x000e};
    case KC_TAB: return Scancode{0x000f};
    case KC_Q: return Scancode{0x0010};
    case KC_W: return Scancode{0x0011};
    case KC_E: return Scancode{0x0012};
    case KC_R: return Scancode{0x0013};
    case KC_T: return Scancode{0x0014};
    case KC_Y: return Scancode{0x0015};
    case KC_U: return Scancode{0x0016};
    case KC_I: return Scancode{0x0017};
    case KC_O: return Scancode{0x0018};
    case KC_P: return Scancode{0x0019};
    case KC_LEFT_BRACKET: return Scancode{0x001a};
    case KC_RIGHT_BRACKET: return Scancode{0x001b};
    case KC_ENTER: return Scancode{0x001c};
    case KC_LEFT_CTRL: return Scancode{0x001d};
    case KC_A: return Scancode{0x001e};
    case KC_S: return Scancode{0x001f};
    case KC_D: return Scancode{0x0020};
    case KC_F: return Scancode{0x0021};
    case KC_G: return Scancode{0x0022};
    case KC_H: return Scancode{0x0023};
    case KC_J: return Scancode{0x0024};
    case KC_K: return Scancode{0x0025};
    case KC_L: return Scancode{0x0026};
    case KC_SEMICOLON: return Scancode{0x0027};
    case KC_QUOTE: return Scancode{0x0028};
    case KC_GRAVE: return Scancode{0x0029};
    case KC_LEFT_SHIFT: return Scancode{0x002a};
    case KC_BACKSLASH: [[fallthrough]];
    case KC_NONUS_HASH: return Scancode{0x002b};
    case KC_Z: return Scancode{0x002c};
    case KC_X: return Scancode{0x002d};
    case KC_C: return Scancode{0x002e};
    case KC_V: return Scancode{0x002f};
    case KC_B: return Scancode{0x0030};
    case KC_N: return Scancode{0x0031};
    case KC_M: return Scancode{0x0032};
    case KC_COMMA: return Scancode{0x0033};
    case KC_DOT: return Scancode{0x0034};
    case KC_SLASH: return Scancode{0x0035};
    case KC_RIGHT_SHIFT: return Scancode{0x0036};
    case KC_KP_ASTERISK: return Scancode{0x0037};
    case KC_LEFT_ALT: return Scancode{0x0038};
    case KC_SPACE: return Scancode{0x0039};
    case KC_CAPS_LOCK: return Scancode{0x003a};
    case KC_F1: return Scancode{0x003b};
    case KC_F2: return Scancode{0x003c};
    case KC_F3: return Scancode{0x003d};
    case KC_F4: return Scancode{0x003e};
    case KC_F5: return Scancode{0x003f};
    case KC_F6: return Scancode{0x0040};
    case KC_F7: return Scancode{0x0041};
    case KC_F8: return Scancode{0x0042};
    case KC_F9: return Scancode{0x0043};
    case KC_F10: return Scancode{0x0044};
    case KC_PAUSE: return Scancode{0x0045};
    case KC_SCROLL_LOCK: return Scancode{0x0046};
    case KC_KP_7: return Scancode{0x0047};
    case KC_KP_8: return Scancode{0x0048};
    case KC_KP_9: return Scancode{0x0049};
    case KC_KP_MINUS: return Scancode{0x004a};
    case KC_KP_4: return Scancode{0x004b};
    case KC_KP_5: return Scancode{0x004c};
    case KC_KP_6: return Scancode{0x004d};
    case KC_KP_PLUS: return Scancode{0x004e};
    case KC_KP_1: return Scancode{0x004f};
    case KC_KP_2: return Scancode{0x0050};
    case KC_KP_3: return Scancode{0x0051};
    case KC_KP_0: return Scancode{0x0052};
    case KC_KP_DOT: return Scancode{0x0053};
    case KC_SYSTEM_REQUEST: return Scancode{0x0054};
    case KC_NONUS_BACKSLASH: return Scancode{0x0056};
    case KC_F11: return Scancode{0x0057};
    case KC_F12: return Scancode{0x0058};
    case KC_KP_EQUAL: return Scancode{0x0059};
    case KC_INTERNATIONAL_6: return Scancode{0x005c};
    case KC_F13: return Scancode{0x0064};
    case KC_F14: return Scancode{0x0065};
    case KC_F15: return Scancode{0x0066};
    case KC_F16: return Scancode{0x0067};
    case KC_F17: return Scancode{0x0068};
    case KC_F18: return Scancode{0x0069};
    case KC_F19: return Scancode{0x006a};
    case KC_F20: return Scancode{0x006b};
    case KC_F21: return Scancode{0x006c};
    case KC_F22: return Scancode{0x006d};
    case KC_F23: return Scancode{0x006e};
    case KC_INTERNATIONAL_2: return Scancode{0x0070};
    case KC_INTERNATIONAL_1: return Scancode{0x0073};
    case KC_F24: return Scancode{0x0076};
    case KC_LANGUAGE_5: return Scancode{0x0076};
    case KC_LANGUAGE_4: return Scancode{0x0077};
    case KC_LANGUAGE_3: return Scancode{0x0078};
    case KC_INTERNATIONAL_4: return Scancode{0x0079};
    case KC_INTERNATIONAL_5: return Scancode{0x007b};
    case KC_INTERNATIONAL_3: return Scancode{0x007d};
    case KC_KP_COMMA: return Scancode{0x007e};
    case KC_LANGUAGE_2: return Scancode{0x00f1};  // or 0x0071
    case KC_LANGUAGE_1: return Scancode{0x00f2};  // or 0x0072
    case KC_MEDIA_PREV_TRACK: return Scancode{0xe010};
    case KC_MEDIA_NEXT_TRACK: return Scancode{0xe019};
    case KC_KP_ENTER: return Scancode{0xe01c};
    case KC_RIGHT_CTRL: return Scancode{0xe01d};
    case KC_AUDIO_MUTE: return Scancode{0xe020};
    case KC_CALCULATOR: return Scancode{0xe021};
    case KC_MEDIA_PLAY_PAUSE: return Scancode{0xe022};
    case KC_MEDIA_STOP: return Scancode{0xe024};
    case KC_AUDIO_VOL_DOWN: return Scancode{0xe02e};
    case KC_AUDIO_VOL_UP: return Scancode{0xe030};
    case KC_WWW_HOME: return Scancode{0xe032};
    case KC_KP_SLASH: return Scancode{0xe035};
    case KC_PRINT_SCREEN: return Scancode{0xe037};
    case KC_RIGHT_ALT: return Scancode{0xe038};
    case KC_NUM_LOCK: return Scancode{0xe045};
    case KC_CANCEL: return Scancode{0xe046};
    case KC_HOME: return Scancode{0xe047};
    case KC_UP: return Scancode{0xe048};
    case KC_PAGE_UP: return Scancode{0xe049};
    case KC_LEFT: return Scancode{0xe04b};
    case KC_RIGHT: return Scancode{0xe04d};
    case KC_END: return Scancode{0xe04f};
    case KC_DOWN: return Scancode{0xe050};
    case KC_PAGE_DOWN: return Scancode{0xe051};
    case KC_INSERT: return Scancode{0xe052};
    case KC_DELETE: return Scancode{0xe053};
    case KC_LEFT_GUI: return Scancode{0xe05b};
    case KC_RIGHT_GUI: return Scancode{0xe05c};
    case KC_APPLICATION: return Scancode{0xe05d};
    case KC_SYSTEM_POWER: [[fallthrough]];
    case KC_KB_POWER: return Scancode{0xe05e};
    case KC_SYSTEM_SLEEP: return Scancode{0xe05f};
    case KC_SYSTEM_WAKE: return Scancode{0xe063};
    case KC_WWW_SEARCH: return Scancode{0xe065};
    case KC_WWW_FAVORITES: return Scancode{0xe066};
    case KC_WWW_REFRESH: return Scancode{0xe067};
    case KC_WWW_STOP: return Scancode{0xe068};
    case KC_WWW_FORWARD: return Scancode{0xe069};
    case KC_WWW_BACK: return Scancode{0xe06a};
    case KC_MY_COMPUTER: return Scancode{0xe06b};
    case KC_MAIL: return Scancode{0xe06c};
    case KC_MEDIA_SELECT: return Scancode{0xe06d};
    default: break;
  }
  return Scancode{0};
}
}  // namespace quark::inline win32
