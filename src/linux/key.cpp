// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file key.cpp
 * @brief Linuxにおけるキー表現
 */
#include "quark/linux/key.hpp"

#include "quark/quantum/key.hpp"

namespace quark::inline evdev {
Key to_key(KeyCode keycode) noexcept {
  switch (+keycode) {
    case KC_A: return Key::A;
    case KC_B: return Key::B;
    case KC_C: return Key::C;
    case KC_D: return Key::D;
    case KC_E: return Key::E;
    case KC_F: return Key::F;
    case KC_G: return Key::G;
    case KC_H: return Key::H;
    case KC_I: return Key::I;
    case KC_J: return Key::J;
    case KC_K: return Key::K;
    case KC_L: return Key::L;
    case KC_M: return Key::M;
    case KC_N: return Key::N;
    case KC_O: return Key::O;
    case KC_P: return Key::P;
    case KC_Q: return Key::Q;
    case KC_R: return Key::R;
    case KC_S: return Key::S;
    case KC_T: return Key::T;
    case KC_U: return Key::U;
    case KC_V: return Key::V;
    case KC_W: return Key::W;
    case KC_X: return Key::X;
    case KC_Y: return Key::Y;
    case KC_Z: return Key::Z;
    case KC_1: return Key::DIGIT_1;
    case KC_2: return Key::DIGIT_2;
    case KC_3: return Key::DIGIT_3;
    case KC_4: return Key::DIGIT_4;
    case KC_5: return Key::DIGIT_5;
    case KC_6: return Key::DIGIT_6;
    case KC_7: return Key::DIGIT_7;
    case KC_8: return Key::DIGIT_8;
    case KC_9: return Key::DIGIT_9;
    case KC_0: return Key::DIGIT_0;
    case KC_ENTER: return Key::ENTER;
    case KC_ESCAPE: return Key::ESCAPE;
    case KC_BACKSPACE: return Key::BACKSPACE;
    case KC_TAB: return Key::TAB;
    case KC_SPACE: return Key::SPACE;
    case KC_MINUS: return Key::MINUS;
    case KC_EQUAL: return Key::EQUAL;
    case KC_LEFT_BRACKET: return Key::LEFT_BRACKET;
    case KC_RIGHT_BRACKET: return Key::RIGHT_BRACKET;
    case KC_BACKSLASH:
    case KC_NONUS_HASH: return Key::NONUS_HASH;
    case KC_SEMICOLON: return Key::SEMICOLON;
    case KC_QUOTE: return Key::QUOTE;
    case KC_GRAVE: return Key::GRAVE;
    case KC_COMMA: return Key::COMMA;
    case KC_DOT: return Key::DOT;
    case KC_SLASH: return Key::SLASH;
    case KC_CAPS_LOCK: return Key::CAPS_LOCK;
    case KC_F1: return Key::F1;
    case KC_F2: return Key::F2;
    case KC_F3: return Key::F3;
    case KC_F4: return Key::F4;
    case KC_F5: return Key::F5;
    case KC_F6: return Key::F6;
    case KC_F7: return Key::F7;
    case KC_F8: return Key::F8;
    case KC_F9: return Key::F9;
    case KC_F10: return Key::F10;
    case KC_F11: return Key::F11;
    case KC_F12: return Key::F12;
    case KC_PRINT_SCREEN: return Key::PRINT_SCREEN;
    case KC_SCROLL_LOCK: return Key::SCROLL_LOCK;
    case KC_PAUSE: return Key::PAUSE;
    case KC_INSERT: return Key::INSERT;
    case KC_HOME: return Key::HOME;
    case KC_PAGE_UP: return Key::PAGE_UP;
    case KC_DELETE: return Key::DELETE;
    case KC_END: return Key::END;
    case KC_PAGE_DOWN: return Key::PAGE_DOWN;
    case KC_RIGHT: return Key::RIGHT;
    case KC_LEFT: return Key::LEFT;
    case KC_DOWN: return Key::DOWN;
    case KC_UP: return Key::UP;
    case KC_NUM_LOCK: return Key::NUM_LOCK;
    case KC_KP_SLASH: return Key::KP_SLASH;
    case KC_KP_ASTERISK: return Key::KP_ASTERISK;
    case KC_KP_MINUS: return Key::KP_MINUS;
    case KC_KP_PLUS: return Key::KP_PLUS;
    case KC_KP_ENTER: return Key::KP_ENTER;
    case KC_KP_1: return Key::KP_1;
    case KC_KP_2: return Key::KP_2;
    case KC_KP_3: return Key::KP_3;
    case KC_KP_4: return Key::KP_4;
    case KC_KP_5: return Key::KP_5;
    case KC_KP_6: return Key::KP_6;
    case KC_KP_7: return Key::KP_7;
    case KC_KP_8: return Key::KP_8;
    case KC_KP_9: return Key::KP_9;
    case KC_KP_0: return Key::KP_0;
    case KC_KP_DOT: return Key::KP_DOT;
    case KC_NONUS_BACKSLASH: return Key::NONUS_BACKSLASH;
    case KC_APPLICATION: return Key::APPLICATION;
    case KC_KB_POWER: return Key::KB_POWER;
    case KC_KP_EQUAL: return Key::KP_EQUAL;
    case KC_F13: return Key::F13;
    case KC_F14: return Key::F14;
    case KC_F15: return Key::F15;
    case KC_F16: return Key::F16;
    case KC_F17: return Key::F17;
    case KC_F18: return Key::F18;
    case KC_F19: return Key::F19;
    case KC_F20: return Key::F20;
    case KC_F21: return Key::F21;
    case KC_F22: return Key::F22;
    case KC_F23: return Key::F23;
    case KC_F24: return Key::F24;
    case KC_EXECUTE: return Key::EXECUTE;
    case KC_HELP: return Key::HELP;
    case KC_MENU: return Key::MENU;
    case KC_SELECT: return Key::SELECT;
    case KC_STOP: return Key::STOP;
    case KC_AGAIN: return Key::AGAIN;
    case KC_UNDO: return Key::UNDO;
    case KC_CUT: return Key::CUT;
    case KC_COPY: return Key::COPY;
    case KC_PASTE: return Key::PASTE;
    case KC_FIND: return Key::FIND;
    case KC_KB_MUTE: return Key::KB_MUTE;
    case KC_KB_VOLUME_UP: return Key::KB_VOLUME_UP;
    case KC_KB_VOLUME_DOWN: return Key::KB_VOLUME_DOWN;
    // case KC_LOCKING_CAPS_LOCK:
    // case KC_LOCKING_NUM_LOCK:
    // case KC_LOCKING_SCROLL_LOCK:
    case KC_KP_COMMA: return Key::KP_COMMA;
    // case KC_KP_EQUAL_AS400:
    case KC_INTERNATIONAL_1: return Key::INTERNATIONAL_1;
    case KC_INTERNATIONAL_2: return Key::INTERNATIONAL_2;
    case KC_INTERNATIONAL_3: return Key::INTERNATIONAL_3;
    case KC_INTERNATIONAL_4: return Key::INTERNATIONAL_4;
    case KC_INTERNATIONAL_5: return Key::INTERNATIONAL_5;
    case KC_INTERNATIONAL_6: return Key::INTERNATIONAL_6;
    // case KC_INTERNATIONAL_7:
    // case KC_INTERNATIONAL_8:
    // case KC_INTERNATIONAL_9:
    case KC_LANGUAGE_1: return Key::LANGUAGE_1;
    case KC_LANGUAGE_2: return Key::LANGUAGE_2;
    case KC_LANGUAGE_3: return Key::LANGUAGE_3;
    case KC_LANGUAGE_4: return Key::LANGUAGE_4;
    case KC_LANGUAGE_5: return Key::LANGUAGE_5;
    // case KC_LANGUAGE_6:
    // case KC_LANGUAGE_7:
    // case KC_LANGUAGE_8:
    // case KC_LANGUAGE_9:
    // case KC_ALTERNATE_ERASE:
    // case KC_SYSTEM_REQUEST:
    // case KC_CANCEL:
    case KC_CLEAR: return Key::CLEAR;
    // case KC_PRIOR:
    // case KC_RETURN:
    // case KC_SEPARATOR:
    // case KC_OUT:
    // case KC_OPER:
    // case KC_CLEAR_AGAIN:
    // case KC_CRSEL:
    // case KC_EXSEL:
    case KC_SYSTEM_POWER: return Key::SYSTEM_POWER;
    case KC_SYSTEM_SLEEP: return Key::SYSTEM_SLEEP;
    case KC_SYSTEM_WAKE: return Key::SYSTEM_WAKE;
    case KC_AUDIO_MUTE: return Key::AUDIO_MUTE;
    case KC_AUDIO_VOL_UP: return Key::AUDIO_VOL_UP;
    case KC_AUDIO_VOL_DOWN: return Key::AUDIO_VOL_DOWN;
    case KC_MEDIA_NEXT_TRACK: return Key::MEDIA_NEXT_TRACK;
    case KC_MEDIA_PREV_TRACK: return Key::MEDIA_PREV_TRACK;
    case KC_MEDIA_STOP: return Key::MEDIA_STOP;
    case KC_MEDIA_PLAY_PAUSE: return Key::MEDIA_PLAY_PAUSE;
    case KC_MEDIA_SELECT: return Key::MEDIA_SELECT;
    case KC_MEDIA_EJECT: return Key::MEDIA_EJECT;
    case KC_MAIL: return Key::MAIL;
    case KC_CALCULATOR: return Key::CALCULATOR;
    case KC_MY_COMPUTER: return Key::MY_COMPUTER;
    case KC_WWW_SEARCH: return Key::WWW_SEARCH;
    case KC_WWW_HOME: return Key::WWW_HOME;
    case KC_WWW_BACK: return Key::WWW_BACK;
    case KC_WWW_FORWARD: return Key::WWW_FORWARD;
    case KC_WWW_STOP: return Key::WWW_STOP;
    case KC_WWW_REFRESH: return Key::WWW_REFRESH;
    case KC_WWW_FAVORITES: return Key::WWW_FAVORITES;
    case KC_MEDIA_FAST_FORWARD: return Key::MEDIA_FAST_FORWARD;
    case KC_MEDIA_REWIND: return Key::MEDIA_REWIND;
    case KC_BRIGHTNESS_UP: return Key::BRIGHTNESS_UP;
    case KC_BRIGHTNESS_DOWN: return Key::BRIGHTNESS_DOWN;
    // case KC_CONTROL_PANEL:
    // case KC_ASSISTANT:
    // case KC_MISSION_CONTROL:
    // case KC_LAUNCHPAD:
    case KC_LEFT_CTRL: return Key::LEFT_CTRL;
    case KC_LEFT_SHIFT: return Key::LEFT_SHIFT;
    case KC_LEFT_ALT: return Key::LEFT_ALT;
    case KC_LEFT_GUI: return Key::LEFT_GUI;
    case KC_RIGHT_CTRL: return Key::RIGHT_CTRL;
    case KC_RIGHT_SHIFT: return Key::RIGHT_SHIFT;
    case KC_RIGHT_ALT: return Key::RIGHT_ALT;
    case KC_RIGHT_GUI: return Key::RIGHT_GUI;
  }
  return Key::NO;
}

KeyCode to_keycode(Key key) noexcept {
  switch (key) {
    case Key::NO: return KC_NO;
    case Key::A: return KC_A;
    case Key::B: return KC_B;
    case Key::C: return KC_C;
    case Key::D: return KC_D;
    case Key::E: return KC_E;
    case Key::F: return KC_F;
    case Key::G: return KC_G;
    case Key::H: return KC_H;
    case Key::I: return KC_I;
    case Key::J: return KC_J;
    case Key::K: return KC_K;
    case Key::L: return KC_L;
    case Key::M: return KC_M;
    case Key::N: return KC_N;
    case Key::O: return KC_O;
    case Key::P: return KC_P;
    case Key::Q: return KC_Q;
    case Key::R: return KC_R;
    case Key::S: return KC_S;
    case Key::T: return KC_T;
    case Key::U: return KC_U;
    case Key::V: return KC_V;
    case Key::W: return KC_W;
    case Key::X: return KC_X;
    case Key::Y: return KC_Y;
    case Key::Z: return KC_Z;
    case Key::DIGIT_1: return KC_1;
    case Key::DIGIT_2: return KC_2;
    case Key::DIGIT_3: return KC_3;
    case Key::DIGIT_4: return KC_4;
    case Key::DIGIT_5: return KC_5;
    case Key::DIGIT_6: return KC_6;
    case Key::DIGIT_7: return KC_7;
    case Key::DIGIT_8: return KC_8;
    case Key::DIGIT_9: return KC_9;
    case Key::DIGIT_0: return KC_0;
    case Key::ENTER: return KC_ENTER;
    case Key::ESCAPE: return KC_ESCAPE;
    case Key::BACKSPACE: return KC_BACKSPACE;
    case Key::TAB: return KC_TAB;
    case Key::SPACE: return KC_SPACE;
    case Key::MINUS: return KC_MINUS;
    case Key::EQUAL: return KC_EQUAL;
    case Key::LEFT_BRACKET: return KC_LEFT_BRACKET;
    case Key::RIGHT_BRACKET: return KC_RIGHT_BRACKET;
    case Key::BACKSLASH: return KC_BACKSLASH;
    // case Key::NONUS_HASH: return KC_NONUS_HASH;
    case Key::SEMICOLON: return KC_SEMICOLON;
    case Key::QUOTE: return KC_QUOTE;
    case Key::GRAVE: return KC_GRAVE;
    case Key::COMMA: return KC_COMMA;
    case Key::DOT: return KC_DOT;
    case Key::SLASH: return KC_SLASH;
    case Key::CAPS_LOCK: return KC_CAPS_LOCK;
    case Key::F1: return KC_F1;
    case Key::F2: return KC_F2;
    case Key::F3: return KC_F3;
    case Key::F4: return KC_F4;
    case Key::F5: return KC_F5;
    case Key::F6: return KC_F6;
    case Key::F7: return KC_F7;
    case Key::F8: return KC_F8;
    case Key::F9: return KC_F9;
    case Key::F10: return KC_F10;
    case Key::F11: return KC_F11;
    case Key::F12: return KC_F12;
    case Key::PRINT_SCREEN: return KC_PRINT_SCREEN;
    case Key::SCROLL_LOCK: return KC_SCROLL_LOCK;
    case Key::PAUSE: return KC_PAUSE;
    case Key::INSERT: return KC_INSERT;
    case Key::HOME: return KC_HOME;
    case Key::PAGE_UP: return KC_PAGE_UP;
    case Key::DELETE: return KC_DELETE;
    case Key::END: return KC_END;
    case Key::PAGE_DOWN: return KC_PAGE_DOWN;
    case Key::RIGHT: return KC_RIGHT;
    case Key::LEFT: return KC_LEFT;
    case Key::DOWN: return KC_DOWN;
    case Key::UP: return KC_UP;
    case Key::NUM_LOCK: return KC_NUM_LOCK;
    case Key::KP_SLASH: return KC_KP_SLASH;
    case Key::KP_ASTERISK: return KC_KP_ASTERISK;
    case Key::KP_MINUS: return KC_KP_MINUS;
    case Key::KP_PLUS: return KC_KP_PLUS;
    case Key::KP_ENTER: return KC_KP_ENTER;
    case Key::KP_1: return KC_KP_1;
    case Key::KP_2: return KC_KP_2;
    case Key::KP_3: return KC_KP_3;
    case Key::KP_4: return KC_KP_4;
    case Key::KP_5: return KC_KP_5;
    case Key::KP_6: return KC_KP_6;
    case Key::KP_7: return KC_KP_7;
    case Key::KP_8: return KC_KP_8;
    case Key::KP_9: return KC_KP_9;
    case Key::KP_0: return KC_KP_0;
    case Key::KP_DOT: return KC_KP_DOT;
    case Key::NONUS_BACKSLASH: return KC_NONUS_BACKSLASH;
    case Key::APPLICATION: return KC_APPLICATION;
    // case Key::KB_POWER: return KC_KB_POWER;
    case Key::KP_EQUAL: return KC_KP_EQUAL;
    case Key::F13: return KC_F13;
    case Key::F14: return KC_F14;
    case Key::F15: return KC_F15;
    case Key::F16: return KC_F16;
    case Key::F17: return KC_F17;
    case Key::F18: return KC_F18;
    case Key::F19: return KC_F19;
    case Key::F20: return KC_F20;
    case Key::F21: return KC_F21;
    case Key::F22: return KC_F22;
    case Key::F23: return KC_F23;
    case Key::F24: return KC_F24;
    case Key::EXECUTE: return KC_EXECUTE;
    case Key::HELP: return KC_HELP;
    case Key::MENU: return KC_MENU;
    case Key::SELECT: return KC_SELECT;
    case Key::STOP: return KC_STOP;
    case Key::AGAIN: return KC_AGAIN;
    case Key::UNDO: return KC_UNDO;
    case Key::CUT: return KC_CUT;
    case Key::COPY: return KC_COPY;
    case Key::PASTE: return KC_PASTE;
    case Key::FIND: return KC_FIND;
    // case Key::KB_MUTE: return KC_KB_MUTE;
    // case Key::KB_VOLUME_UP: return KC_KB_VOLUME_UP;
    // case Key::KB_VOLUME_DOWN: return KC_KB_VOLUME_DOWN;
    case Key::KP_COMMA: return KC_KP_COMMA;
    case Key::INTERNATIONAL_1: return KC_INTERNATIONAL_1;
    case Key::INTERNATIONAL_2: return KC_INTERNATIONAL_2;
    case Key::INTERNATIONAL_3: return KC_INTERNATIONAL_3;
    case Key::INTERNATIONAL_4: return KC_INTERNATIONAL_4;
    case Key::INTERNATIONAL_5: return KC_INTERNATIONAL_5;
    case Key::INTERNATIONAL_6: return KC_INTERNATIONAL_6;
    case Key::LANGUAGE_1: return KC_LANGUAGE_1;
    case Key::LANGUAGE_2: return KC_LANGUAGE_2;
    case Key::LANGUAGE_3: return KC_LANGUAGE_3;
    case Key::LANGUAGE_4: return KC_LANGUAGE_4;
    case Key::LANGUAGE_5: return KC_LANGUAGE_5;
    case Key::CLEAR: return KC_CLEAR;
    case Key::SYSTEM_POWER: return KC_SYSTEM_POWER;
    case Key::SYSTEM_SLEEP: return KC_SYSTEM_SLEEP;
    case Key::SYSTEM_WAKE: return KC_SYSTEM_WAKE;
    case Key::AUDIO_MUTE: return KC_AUDIO_MUTE;
    case Key::AUDIO_VOL_UP: return KC_AUDIO_VOL_UP;
    case Key::AUDIO_VOL_DOWN: return KC_AUDIO_VOL_DOWN;
    case Key::MEDIA_NEXT_TRACK: return KC_MEDIA_NEXT_TRACK;
    case Key::MEDIA_PREV_TRACK: return KC_MEDIA_PREV_TRACK;
    case Key::MEDIA_STOP: return KC_MEDIA_STOP;
    case Key::MEDIA_PLAY_PAUSE: return KC_MEDIA_PLAY_PAUSE;
    case Key::MEDIA_SELECT: return KC_MEDIA_SELECT;
    case Key::MEDIA_EJECT: return KC_MEDIA_EJECT;
    case Key::MAIL: return KC_MAIL;
    case Key::CALCULATOR: return KC_CALCULATOR;
    case Key::MY_COMPUTER: return KC_MY_COMPUTER;
    case Key::WWW_SEARCH: return KC_WWW_SEARCH;
    case Key::WWW_HOME: return KC_WWW_HOME;
    case Key::WWW_BACK: return KC_WWW_BACK;
    case Key::WWW_FORWARD: return KC_WWW_FORWARD;
    // case Key::WWW_STOP: return KC_WWW_STOP;
    case Key::WWW_REFRESH: return KC_WWW_REFRESH;
    case Key::WWW_FAVORITES: return KC_WWW_FAVORITES;
    case Key::MEDIA_FAST_FORWARD: return KC_MEDIA_FAST_FORWARD;
    case Key::MEDIA_REWIND: return KC_MEDIA_REWIND;
    case Key::BRIGHTNESS_UP: return KC_BRIGHTNESS_UP;
    case Key::BRIGHTNESS_DOWN: return KC_BRIGHTNESS_DOWN;
    case Key::LEFT_CTRL: return KC_LEFT_CTRL;
    case Key::LEFT_SHIFT: return KC_LEFT_SHIFT;
    case Key::LEFT_ALT: return KC_LEFT_ALT;
    case Key::LEFT_GUI: return KC_LEFT_GUI;
    case Key::RIGHT_CTRL: return KC_RIGHT_CTRL;
    case Key::RIGHT_SHIFT: return KC_RIGHT_SHIFT;
    case Key::RIGHT_ALT: return KC_RIGHT_ALT;
    case Key::RIGHT_GUI: return KC_RIGHT_GUI;
  }
  return KC_NO;
}
}  // namespace quark::inline evdev
