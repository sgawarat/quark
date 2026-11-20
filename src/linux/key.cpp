// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.cpp
 * @brief Linuxにおけるキー表現
 */
#include "quark/linux/key.hpp"

#include <linux/input-event-codes.h>

#include "quark/quantum/key.hpp"

namespace quark::inline evdev {
Key to_key(KeyCode keycode) noexcept {
  switch (+keycode) {
    case KC_A: return Key{KEY_A};
    case KC_B: return Key{KEY_B};
    case KC_C: return Key{KEY_C};
    case KC_D: return Key{KEY_D};
    case KC_E: return Key{KEY_E};
    case KC_F: return Key{KEY_F};
    case KC_G: return Key{KEY_G};
    case KC_H: return Key{KEY_H};
    case KC_I: return Key{KEY_I};
    case KC_J: return Key{KEY_J};
    case KC_K: return Key{KEY_K};
    case KC_L: return Key{KEY_L};
    case KC_M: return Key{KEY_M};
    case KC_N: return Key{KEY_N};
    case KC_O: return Key{KEY_O};
    case KC_P: return Key{KEY_P};
    case KC_Q: return Key{KEY_Q};
    case KC_R: return Key{KEY_R};
    case KC_S: return Key{KEY_S};
    case KC_T: return Key{KEY_T};
    case KC_U: return Key{KEY_U};
    case KC_V: return Key{KEY_V};
    case KC_W: return Key{KEY_W};
    case KC_X: return Key{KEY_X};
    case KC_Y: return Key{KEY_Y};
    case KC_Z: return Key{KEY_Z};
    case KC_1: return Key{KEY_1};
    case KC_2: return Key{KEY_2};
    case KC_3: return Key{KEY_3};
    case KC_4: return Key{KEY_4};
    case KC_5: return Key{KEY_5};
    case KC_6: return Key{KEY_6};
    case KC_7: return Key{KEY_7};
    case KC_8: return Key{KEY_8};
    case KC_9: return Key{KEY_9};
    case KC_0: return Key{KEY_0};
    case KC_ENTER: return Key{KEY_ENTER};
    case KC_ESCAPE: return Key{KEY_ESC};
    case KC_BACKSPACE: return Key{KEY_BACKSPACE};
    case KC_TAB: return Key{KEY_TAB};
    case KC_SPACE: return Key{KEY_SPACE};
    case KC_MINUS: return Key{KEY_MINUS};
    case KC_EQUAL: return Key{KEY_EQUAL};
    case KC_LEFT_BRACKET: return Key{KEY_LEFTBRACE};
    case KC_RIGHT_BRACKET: return Key{KEY_RIGHTBRACE};
    case KC_BACKSLASH:
    case KC_NONUS_HASH: return Key{KEY_BACKSLASH};
    case KC_SEMICOLON: return Key{KEY_SEMICOLON};
    case KC_QUOTE: return Key{KEY_APOSTROPHE};
    case KC_GRAVE: return Key{KEY_GRAVE};
    case KC_COMMA: return Key{KEY_COMMA};
    case KC_DOT: return Key{KEY_DOT};
    case KC_SLASH: return Key{KEY_SLASH};
    case KC_CAPS_LOCK: return Key{KEY_CAPSLOCK};
    case KC_F1: return Key{KEY_F1};
    case KC_F2: return Key{KEY_F2};
    case KC_F3: return Key{KEY_F3};
    case KC_F4: return Key{KEY_F4};
    case KC_F5: return Key{KEY_F5};
    case KC_F6: return Key{KEY_F6};
    case KC_F7: return Key{KEY_F7};
    case KC_F8: return Key{KEY_F8};
    case KC_F9: return Key{KEY_F9};
    case KC_F10: return Key{KEY_F10};
    case KC_F11: return Key{KEY_F11};
    case KC_F12: return Key{KEY_F12};
    case KC_PRINT_SCREEN: return Key{KEY_SYSRQ};
    case KC_SCROLL_LOCK: return Key{KEY_SCROLLLOCK};
    case KC_PAUSE: return Key{KEY_PAUSE};
    case KC_INSERT: return Key{KEY_INSERT};
    case KC_HOME: return Key{KEY_HOME};
    case KC_PAGE_UP: return Key{KEY_PAGEUP};
    case KC_DELETE: return Key{KEY_DELETE};
    case KC_END: return Key{KEY_END};
    case KC_PAGE_DOWN: return Key{KEY_PAGEDOWN};
    case KC_RIGHT: return Key{KEY_RIGHT};
    case KC_LEFT: return Key{KEY_LEFT};
    case KC_DOWN: return Key{KEY_DOWN};
    case KC_UP: return Key{KEY_UP};
    case KC_NUM_LOCK: return Key{KEY_NUMLOCK};
    case KC_KP_SLASH: return Key{KEY_KPSLASH};
    case KC_KP_ASTERISK: return Key{KEY_KPASTERISK};
    case KC_KP_MINUS: return Key{KEY_KPMINUS};
    case KC_KP_PLUS: return Key{KEY_KPPLUS};
    case KC_KP_ENTER: return Key{KEY_KPENTER};
    case KC_KP_1: return Key{KEY_KP1};
    case KC_KP_2: return Key{KEY_KP2};
    case KC_KP_3: return Key{KEY_KP3};
    case KC_KP_4: return Key{KEY_KP4};
    case KC_KP_5: return Key{KEY_KP5};
    case KC_KP_6: return Key{KEY_KP6};
    case KC_KP_7: return Key{KEY_KP7};
    case KC_KP_8: return Key{KEY_KP8};
    case KC_KP_9: return Key{KEY_KP9};
    case KC_KP_0: return Key{KEY_KP0};
    case KC_KP_DOT: return Key{KEY_KPDOT};
    case KC_NONUS_BACKSLASH: return Key{KEY_102ND};
    case KC_APPLICATION: return Key{KEY_COMPOSE};
    case KC_KB_POWER: return Key{KEY_POWER};
    case KC_KP_EQUAL: return Key{KEY_KPEQUAL};
    case KC_F13: return Key{KEY_F13};
    case KC_F14: return Key{KEY_F14};
    case KC_F15: return Key{KEY_F15};
    case KC_F16: return Key{KEY_F16};
    case KC_F17: return Key{KEY_F17};
    case KC_F18: return Key{KEY_F18};
    case KC_F19: return Key{KEY_F19};
    case KC_F20: return Key{KEY_F20};
    case KC_F21: return Key{KEY_F21};
    case KC_F22: return Key{KEY_F22};
    case KC_F23: return Key{KEY_F23};
    case KC_F24: return Key{KEY_F24};
    case KC_EXECUTE: return Key{KEY_OPEN};
    case KC_HELP: return Key{KEY_HELP};
    case KC_MENU: return Key{KEY_PROPS};
    case KC_SELECT: return Key{KEY_FRONT};
    case KC_STOP: return Key{KEY_STOP};
    case KC_AGAIN: return Key{KEY_AGAIN};
    case KC_UNDO: return Key{KEY_UNDO};
    case KC_CUT: return Key{KEY_CUT};
    case KC_COPY: return Key{KEY_COPY};
    case KC_PASTE: return Key{KEY_PASTE};
    case KC_FIND: return Key{KEY_FIND};
    case KC_KB_MUTE: return Key{KEY_MUTE};
    case KC_KB_VOLUME_UP: return Key{KEY_VOLUMEUP};
    case KC_KB_VOLUME_DOWN: return Key{KEY_VOLUMEDOWN};
    // case KC_LOCKING_CAPS_LOCK:
    // case KC_LOCKING_NUM_LOCK:
    // case KC_LOCKING_SCROLL_LOCK:
    case KC_KP_COMMA: return Key{KEY_KPCOMMA};
    // case KC_KP_EQUAL_AS400:
    case KC_INTERNATIONAL_1: return Key{KEY_RO};
    case KC_INTERNATIONAL_2: return Key{KEY_KATAKANAHIRAGANA};
    case KC_INTERNATIONAL_3: return Key{KEY_YEN};
    case KC_INTERNATIONAL_4: return Key{KEY_HENKAN};
    case KC_INTERNATIONAL_5: return Key{KEY_MUHENKAN};
    case KC_INTERNATIONAL_6: return Key{KEY_KPJPCOMMA};
    // case KC_INTERNATIONAL_7:
    // case KC_INTERNATIONAL_8:
    // case KC_INTERNATIONAL_9:
    case KC_LANGUAGE_1: return Key{KEY_HANGEUL};
    case KC_LANGUAGE_2: return Key{KEY_HANJA};
    case KC_LANGUAGE_3: return Key{KEY_KATAKANA};
    case KC_LANGUAGE_4: return Key{KEY_HIRAGANA};
    case KC_LANGUAGE_5: return Key{KEY_ZENKAKUHANKAKU};
    // case KC_LANGUAGE_6:
    // case KC_LANGUAGE_7:
    // case KC_LANGUAGE_8:
    // case KC_LANGUAGE_9:
    // case KC_ALTERNATE_ERASE:
    // case KC_SYSTEM_REQUEST:
    // case KC_CANCEL:
    case KC_CLEAR: return Key{KEY_CLEAR};
    // case KC_PRIOR:
    // case KC_RETURN:
    // case KC_SEPARATOR:
    // case KC_OUT:
    // case KC_OPER:
    // case KC_CLEAR_AGAIN:
    // case KC_CRSEL:
    // case KC_EXSEL:
    case KC_SYSTEM_POWER: return Key{KEY_POWER};
    case KC_SYSTEM_SLEEP: return Key{KEY_SLEEP};
    case KC_SYSTEM_WAKE: return Key{KEY_WAKEUP};
    case KC_AUDIO_MUTE: return Key{KEY_MUTE};
    case KC_AUDIO_VOL_UP: return Key{KEY_VOLUMEUP};
    case KC_AUDIO_VOL_DOWN: return Key{KEY_VOLUMEDOWN};
    case KC_MEDIA_NEXT_TRACK: return Key{KEY_NEXTSONG};
    case KC_MEDIA_PREV_TRACK: return Key{KEY_PREVIOUSSONG};
    case KC_MEDIA_STOP: return Key{KEY_STOPCD};
    case KC_MEDIA_PLAY_PAUSE: return Key{KEY_PLAYPAUSE};
    case KC_MEDIA_SELECT: return Key{KEY_CONFIG};
    case KC_MEDIA_EJECT: return Key{KEY_EJECTCD};
    case KC_MAIL: return Key{KEY_MAIL};
    case KC_CALCULATOR: return Key{KEY_CALC};
    case KC_MY_COMPUTER: return Key{KEY_FILE};
    case KC_WWW_SEARCH: return Key{KEY_SEARCH};
    case KC_WWW_HOME: return Key{KEY_HOMEPAGE};
    case KC_WWW_BACK: return Key{KEY_BACK};
    case KC_WWW_FORWARD: return Key{KEY_FORWARD};
    case KC_WWW_STOP: return Key{KEY_STOP};
    case KC_WWW_REFRESH: return Key{KEY_REFRESH};
    case KC_WWW_FAVORITES: return Key{KEY_BOOKMARKS};
    case KC_MEDIA_FAST_FORWARD: return Key{KEY_FASTFORWARD};
    case KC_MEDIA_REWIND: return Key{KEY_REWIND};
    case KC_BRIGHTNESS_UP: return Key{KEY_BRIGHTNESSUP};
    case KC_BRIGHTNESS_DOWN: return Key{KEY_BRIGHTNESSDOWN};
    // case KC_CONTROL_PANEL:
    // case KC_ASSISTANT:
    // case KC_MISSION_CONTROL:
    // case KC_LAUNCHPAD:
    case KC_LEFT_CTRL: return Key{KEY_LEFTCTRL};
    case KC_LEFT_SHIFT: return Key{KEY_LEFTSHIFT};
    case KC_LEFT_ALT: return Key{KEY_LEFTALT};
    case KC_LEFT_GUI: return Key{KEY_LEFTMETA};
    case KC_RIGHT_CTRL: return Key{KEY_RIGHTCTRL};
    case KC_RIGHT_SHIFT: return Key{KEY_RIGHTSHIFT};
    case KC_RIGHT_ALT: return Key{KEY_RIGHTALT};
    case KC_RIGHT_GUI: return Key{KEY_RIGHTMETA};
  };
  return Key::NO;
}

// Key to_key(UsagePage page, UsageId id) noexcept {
//   switch (page) {
//     case UsagePage::GENERIC_DESKTOP: {
//       switch (static_cast<uint16_t>(id)) {
//         case SYSTEM_POWER_DOWN: return Key{KEY_POWER};
//         case SYSTEM_SLEEP: return Key{KEY_SLEEP};
//         case SYSTEM_WAKE_UP: return Key{KEY_WAKEUP};
//         case SYSTEM_RESTART:
//         case SYSTEM_DISPLAY_TOGGLE_INT_EXT: return Key{0};
//       }
//       return Key{0};
//     }
//     case UsagePage::CONSUMER: {
//       switch (static_cast<uint16_t>(id)) {
//         case SNAPSHOT: return Key{KEY_PRINT};
//         case BRIGHTNESS_UP: return Key{KEY_BRIGHTNESSUP};
//         case BRIGHTNESS_DOWN: return Key{KEY_BRIGHTNESSDOWN};
//         case TRANSPORT_RECORD: return Key{KEY_RECORD};
//         case TRANSPORT_FAST_FORWARD: return Key{KEY_FASTFORWARD};
//         case TRANSPORT_REWIND: return Key{KEY_REWIND};
//         case TRANSPORT_NEXT_TRACK: return Key{KEY_NEXTSONG};
//         case TRANSPORT_PREV_TRACK: return Key{KEY_PREVIOUSSONG};
//         case TRANSPORT_STOP: return Key{KEY_STOPCD};
//         case TRANSPORT_EJECT: return Key{KEY_EJECTCD};
//         case TRANSPORT_RANDOM_PLAY:
//         case TRANSPORT_STOP_EJECT: return Key{0};
//         case TRANSPORT_PLAY_PAUSE: return Key{KEY_PLAYPAUSE};
//         case AUDIO_MUTE: return Key{KEY_MUTE};
//         case AUDIO_VOL_UP: return Key{KEY_VOLUMEUP};
//         case AUDIO_VOL_DOWN: return Key{KEY_VOLUMEDOWN};
//         case AL_CC_CONFIG: return Key{KEY_CONFIG};
//         case AL_EMAIL: return Key{KEY_MAIL};
//         case AL_CALCULATOR: return Key{KEY_CALC};
//         case AL_LOCAL_BROWSER: return Key{KEY_FILE};
//         case AL_LOCK: return Key{KEY_COFFEE};
//         case AL_CONTROL_PANEL:
//         case AL_ASSISTANT:
//         case AL_KEYBOARD_LAYOUT: return Key{0};
//         case AC_NEW: return Key{KEY_NEW};
//         case AC_OPEN: return Key{KEY_OPEN};
//         case AC_CLOSE: return Key{KEY_CLOSE};
//         case AC_EXIT: return Key{KEY_EXIT};
//         case AC_MAXIMIZE:
//         case AC_MINIMIZE: return Key{0};
//         case AC_SAVE: return Key{KEY_SAVE};
//         case AC_PRINT: return Key{KEY_PRINT};
//         case AC_PROPERTIES: return Key{KEY_PROPS};
//         case AC_UNDO: return Key{KEY_UNDO};
//         case AC_COPY: return Key{KEY_COPY};
//         case AC_CUT: return Key{KEY_CUT};
//         case AC_PASTE: return Key{KEY_PASTE};
//         case AC_SELECT_ALL: return Key{0};
//         case AC_FIND: return Key{KEY_FIND};
//         case AC_SEARCH: return Key{KEY_SEARCH};
//         case AC_HOME: return Key{KEY_HOMEPAGE};
//         case AC_BACK: return Key{KEY_BACK};
//         case AC_FORWARD: return Key{KEY_FORWARD};
//         case AC_STOP: return Key{KEY_STOP};
//         case AC_REFRESH: return Key{KEY_REFRESH};
//         case AC_BOOKMARKS: return Key{KEY_BOOKMARKS};
//         case AC_NEXT_KEYBOARD_LAYOUT_SELECT:
//         case AC_DESKTOP_SHOW_ALL_WINDOWS:
//         case AC_SOFT_KEY_LEFT: return Key{0};
//       }
//       return Key{0};
//     }
//   }
//   return Key{0};
// }

KeyCode to_keycode(Key key) noexcept {
  switch (+key) {
    case KEY_A: return KC_A;
    case KEY_B: return KC_B;
    case KEY_C: return KC_C;
    case KEY_D: return KC_D;
    case KEY_E: return KC_E;
    case KEY_F: return KC_F;
    case KEY_G: return KC_G;
    case KEY_H: return KC_H;
    case KEY_I: return KC_I;
    case KEY_J: return KC_J;
    case KEY_K: return KC_K;
    case KEY_L: return KC_L;
    case KEY_M: return KC_M;
    case KEY_N: return KC_N;
    case KEY_O: return KC_O;
    case KEY_P: return KC_P;
    case KEY_Q: return KC_Q;
    case KEY_R: return KC_R;
    case KEY_S: return KC_S;
    case KEY_T: return KC_T;
    case KEY_U: return KC_U;
    case KEY_V: return KC_V;
    case KEY_W: return KC_W;
    case KEY_X: return KC_X;
    case KEY_Y: return KC_Y;
    case KEY_Z: return KC_Z;
    case KEY_1: return KC_1;
    case KEY_2: return KC_2;
    case KEY_3: return KC_3;
    case KEY_4: return KC_4;
    case KEY_5: return KC_5;
    case KEY_6: return KC_6;
    case KEY_7: return KC_7;
    case KEY_8: return KC_8;
    case KEY_9: return KC_9;
    case KEY_0: return KC_0;
    case KEY_ENTER: return KC_ENTER;
    case KEY_ESC: return KC_ESCAPE;
    case KEY_BACKSPACE: return KC_BACKSPACE;
    case KEY_TAB: return KC_TAB;
    case KEY_SPACE: return KC_SPACE;
    case KEY_MINUS: return KC_MINUS;
    case KEY_EQUAL: return KC_EQUAL;
    case KEY_LEFTBRACE: return KC_LEFT_BRACKET;
    case KEY_RIGHTBRACE: return KC_RIGHT_BRACKET;
    case KEY_BACKSLASH: return KC_BACKSLASH;
    // case KEY_BACKSLASH: return KC_NONUS_HASH;
    case KEY_SEMICOLON: return KC_SEMICOLON;
    case KEY_APOSTROPHE: return KC_QUOTE;
    case KEY_GRAVE: return KC_GRAVE;
    case KEY_COMMA: return KC_COMMA;
    case KEY_DOT: return KC_DOT;
    case KEY_SLASH: return KC_SLASH;
    case KEY_CAPSLOCK: return KC_CAPS_LOCK;
    case KEY_F1: return KC_F1;
    case KEY_F2: return KC_F2;
    case KEY_F3: return KC_F3;
    case KEY_F4: return KC_F4;
    case KEY_F5: return KC_F5;
    case KEY_F6: return KC_F6;
    case KEY_F7: return KC_F7;
    case KEY_F8: return KC_F8;
    case KEY_F9: return KC_F9;
    case KEY_F10: return KC_F10;
    case KEY_F11: return KC_F11;
    case KEY_F12: return KC_F12;
    case KEY_SYSRQ: return KC_PRINT_SCREEN;
    case KEY_SCROLLLOCK: return KC_SCROLL_LOCK;
    case KEY_PAUSE: return KC_PAUSE;
    case KEY_INSERT: return KC_INSERT;
    case KEY_HOME: return KC_HOME;
    case KEY_PAGEUP: return KC_PAGE_UP;
    case KEY_DELETE: return KC_DELETE;
    case KEY_END: return KC_END;
    case KEY_PAGEDOWN: return KC_PAGE_DOWN;
    case KEY_RIGHT: return KC_RIGHT;
    case KEY_LEFT: return KC_LEFT;
    case KEY_DOWN: return KC_DOWN;
    case KEY_UP: return KC_UP;
    case KEY_NUMLOCK: return KC_NUM_LOCK;
    case KEY_KPSLASH: return KC_KP_SLASH;
    case KEY_KPASTERISK: return KC_KP_ASTERISK;
    case KEY_KPMINUS: return KC_KP_MINUS;
    case KEY_KPPLUS: return KC_KP_PLUS;
    case KEY_KPENTER: return KC_KP_ENTER;
    case KEY_KP1: return KC_KP_1;
    case KEY_KP2: return KC_KP_2;
    case KEY_KP3: return KC_KP_3;
    case KEY_KP4: return KC_KP_4;
    case KEY_KP5: return KC_KP_5;
    case KEY_KP6: return KC_KP_6;
    case KEY_KP7: return KC_KP_7;
    case KEY_KP8: return KC_KP_8;
    case KEY_KP9: return KC_KP_9;
    case KEY_KP0: return KC_KP_0;
    case KEY_KPDOT: return KC_KP_DOT;
    case KEY_102ND: return KC_NONUS_BACKSLASH;
    case KEY_COMPOSE: return KC_APPLICATION;
    // case KEY_POWER: return KC_KB_POWER;
    case KEY_KPEQUAL: return KC_KP_EQUAL;
    case KEY_F13: return KC_F13;
    case KEY_F14: return KC_F14;
    case KEY_F15: return KC_F15;
    case KEY_F16: return KC_F16;
    case KEY_F17: return KC_F17;
    case KEY_F18: return KC_F18;
    case KEY_F19: return KC_F19;
    case KEY_F20: return KC_F20;
    case KEY_F21: return KC_F21;
    case KEY_F22: return KC_F22;
    case KEY_F23: return KC_F23;
    case KEY_F24: return KC_F24;
    case KEY_OPEN: return KC_EXECUTE;
    case KEY_HELP: return KC_HELP;
    case KEY_PROPS: return KC_MENU;
    case KEY_FRONT: return KC_SELECT;
    case KEY_STOP: return KC_STOP;
    case KEY_AGAIN: return KC_AGAIN;
    case KEY_UNDO: return KC_UNDO;
    case KEY_CUT: return KC_CUT;
    case KEY_COPY: return KC_COPY;
    case KEY_PASTE: return KC_PASTE;
    case KEY_FIND: return KC_FIND;
    // case KEY_MUTE: return KC_KB_MUTE;
    // case KEY_VOLUMEUP: return KC_KB_VOLUME_UP;
    // case KEY_VOLUMEDOWN: return KC_KB_VOLUME_DOWN;
    case KEY_KPCOMMA: return KC_KP_COMMA;
    case KEY_RO: return KC_INTERNATIONAL_1;
    case KEY_KATAKANAHIRAGANA: return KC_INTERNATIONAL_2;
    case KEY_YEN: return KC_INTERNATIONAL_3;
    case KEY_HENKAN: return KC_INTERNATIONAL_4;
    case KEY_MUHENKAN: return KC_INTERNATIONAL_5;
    case KEY_KPJPCOMMA: return KC_INTERNATIONAL_6;
    case KEY_HANGEUL: return KC_LANGUAGE_1;
    case KEY_HANJA: return KC_LANGUAGE_2;
    case KEY_KATAKANA: return KC_LANGUAGE_3;
    case KEY_HIRAGANA: return KC_LANGUAGE_4;
    case KEY_ZENKAKUHANKAKU: return KC_LANGUAGE_5;
    case KEY_CLEAR: return KC_CLEAR;
    case KEY_POWER: return KC_SYSTEM_POWER;
    case KEY_SLEEP: return KC_SYSTEM_SLEEP;
    case KEY_WAKEUP: return KC_SYSTEM_WAKE;
    case KEY_MUTE: return KC_AUDIO_MUTE;
    case KEY_VOLUMEUP: return KC_AUDIO_VOL_UP;
    case KEY_VOLUMEDOWN: return KC_AUDIO_VOL_DOWN;
    case KEY_NEXTSONG: return KC_MEDIA_NEXT_TRACK;
    case KEY_PREVIOUSSONG: return KC_MEDIA_PREV_TRACK;
    case KEY_STOPCD: return KC_MEDIA_STOP;
    case KEY_PLAYPAUSE: return KC_MEDIA_PLAY_PAUSE;
    case KEY_CONFIG: return KC_MEDIA_SELECT;
    case KEY_EJECTCD: return KC_MEDIA_EJECT;
    case KEY_MAIL: return KC_MAIL;
    case KEY_CALC: return KC_CALCULATOR;
    case KEY_FILE: return KC_MY_COMPUTER;
    case KEY_SEARCH: return KC_WWW_SEARCH;
    case KEY_HOMEPAGE: return KC_WWW_HOME;
    case KEY_BACK: return KC_WWW_BACK;
    case KEY_FORWARD: return KC_WWW_FORWARD;
    // case KEY_STOP: return KC_WWW_STOP;
    case KEY_REFRESH: return KC_WWW_REFRESH;
    case KEY_BOOKMARKS: return KC_WWW_FAVORITES;
    case KEY_FASTFORWARD: return KC_MEDIA_FAST_FORWARD;
    case KEY_REWIND: return KC_MEDIA_REWIND;
    case KEY_BRIGHTNESSUP: return KC_BRIGHTNESS_UP;
    case KEY_BRIGHTNESSDOWN: return KC_BRIGHTNESS_DOWN;
    case KEY_LEFTCTRL: return KC_LEFT_CTRL;
    case KEY_LEFTSHIFT: return KC_LEFT_SHIFT;
    case KEY_LEFTALT: return KC_LEFT_ALT;
    case KEY_LEFTMETA: return KC_LEFT_GUI;
    case KEY_RIGHTCTRL: return KC_RIGHT_CTRL;
    case KEY_RIGHTSHIFT: return KC_RIGHT_SHIFT;
    case KEY_RIGHTALT: return KC_RIGHT_ALT;
    case KEY_RIGHTMETA: return KC_RIGHT_GUI;
  };
  return KC_NO;
}
}  // namespace quark::inline evdev
