// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file oadg109.cpp
 * @brief 日本語配列
 */
#include <array>
#include <tmk_desktop/key.hpp>

extern "C" {
#include <keyboard.h>
}  // extern "C"

#include "oadg109.h"

namespace tmk_desktop {
namespace {
// 言語固有のキー
constexpr Key K_CIRCUMFLEX = K_EQUAL;
constexpr Key K_AT = K_LEFT_BRACKET;
constexpr Key K_LEFT_BRACKET = K_RIGHT_BRACKET;
constexpr Key K_COLON = K_QUOTE;
constexpr Key K_HANKAKU_ZENKAKU = K_GRAVE;
constexpr Key K_RIGHT_BRACKET = K_BACKSLASH;
constexpr Key K_KATAKANA_HIRAGANA = K_INTERNATIONAL_2;
constexpr Key K_BACKSLASH = K_INTERNATIONAL_1;
constexpr Key K_HENKAN = K_INTERNATIONAL_4;
constexpr Key K_MUHENKAN = K_INTERNATIONAL_5;
constexpr Key K_YEN = K_INTERNATIONAL_3;

// キー配置
constexpr std::array<std::array<Key, LAYOUT_COLS>, LAYOUT_ROWS> keys{{
    /* clang-format off */
    {K_ESCAPE, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_PRINT_SCREEN, K_SCROLL_LOCK, K_PAUSE},
    {K_HANKAKU_ZENKAKU, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_CIRCUMFLEX, K_YEN, K_BACKSPACE, K_INSERT, K_HOME, K_PAGE_UP, K_NUM_LOCK, K_KP_SLASH, K_KP_ASTERISK, K_KP_MINUS},
    {K_TAB, K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P, K_AT, K_LEFT_BRACKET, K_DELETE, K_END, K_PAGE_DOWN, K_KP_7, K_KP_8, K_KP_9},
    {K_CAPS_LOCK, K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SEMICOLON, K_COLON, K_RIGHT_BRACKET, K_ENTER, K_KP_4, K_KP_5, K_KP_6, K_KP_PLUS},
    {K_LEFT_SHIFT, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_DOT, K_SLASH, K_BACKSLASH, K_RIGHT_SHIFT, K_UP, K_KP_1, K_KP_2, K_KP_3},
    {K_LEFT_CTRL, K_LEFT_GUI, K_LEFT_ALT, K_MUHENKAN, K_SPACE, K_HENKAN, K_KATAKANA_HIRAGANA, K_RIGHT_ALT, K_RIGHT_GUI, K_APPLICATION, K_RIGHT_CTRL, K_LEFT, K_DOWN, K_RIGHT, K_KP_0, K_KP_DOT, K_KP_ENTER}
    /* clang-format on */
}};

// キーごとのmatrix上の位置
constexpr std::array layout = []() constexpr {
  std::array<keypos_t, KEY_COUNT> t;
  std::fill(t.begin(), t.end(), keypos_t{0xff, 0xff});
  for (size_t y = 0; y < keys.size(); ++y) {
    const auto& row = keys[y];
    for (size_t x = 0; x < row.size(); ++x) {
      t[row[x]] = keypos_t{static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
    }
  }
  return t;
}();
}  // namespace

bool is_tapping_key(enum Key key) noexcept {
#ifdef _WIN32
  switch (key) {
    case K_HANKAKU_ZENKAKU: [[fallthrough]];
    case K_CAPS_LOCK: [[fallthrough]];
    case K_KATAKANA_HIRAGANA: return true;
  }
#endif
  return false;
}

keypos_t key_to_keypos(enum Key key) noexcept {
  return layout[key];
}
}  // namespace tmk_desktop
