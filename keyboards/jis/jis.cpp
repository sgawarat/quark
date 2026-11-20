// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file jis.cpp
 * @brief JIS配列
 */
#include "jis.hpp"  // IWYU pragma: keep

extern "C" {
#include <keycodes.h>         // IWYU pragma: export
#include <keymap_japanese.h>  // IWYU pragma: export
}

#include "quark/quantum/key.hpp"

namespace quark::jis {
namespace {
// キー配置
// NOLINTBEGIN(readability-trailing-comma)
constexpr auto layout = define_layout({{
  {KC_ESCAPE, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
    KC_PRINT_SCREEN, KC_SCROLL_LOCK, KC_PAUSE},
  {JP_ZKHK, JP_1, JP_2, JP_3, JP_4, JP_5, JP_6, JP_7, JP_8, JP_9, JP_0, JP_MINS, JP_CIRC, JP_YEN,
    KC_BACKSPACE, KC_INSERT, KC_HOME, KC_PAGE_UP, KC_NUM_LOCK, KC_KP_SLASH, KC_KP_ASTERISK,
    KC_KP_MINUS},
  {KC_TAB, JP_Q, JP_W, JP_E, JP_R, JP_T, JP_Y, JP_U, JP_I, JP_O, JP_P, JP_AT, JP_LBRC, KC_DELETE,
    KC_END, KC_PAGE_DOWN, KC_KP_7, KC_KP_8, KC_KP_9},
  {KC_CAPS_LOCK, JP_A, JP_S, JP_D, JP_F, JP_G, JP_H, JP_J, JP_K, JP_L, JP_SCLN, JP_COLN, JP_RBRC,
    KC_ENTER, KC_KP_4, KC_KP_5, KC_KP_6, KC_KP_PLUS},
  {KC_LEFT_SHIFT, JP_Z, JP_X, JP_C, JP_V, JP_B, JP_N, JP_M, JP_COMM, JP_DOT, JP_SLSH, JP_BSLS,
    KC_RIGHT_SHIFT, KC_UP, KC_KP_1, KC_KP_2, KC_KP_3},
  {KC_LEFT_CTRL, KC_LEFT_GUI, KC_LEFT_ALT, JP_MHEN, KC_SPACE, JP_HENK, JP_KANA, KC_RIGHT_ALT,
    KC_RIGHT_GUI, KC_APPLICATION, KC_RIGHT_CTRL, KC_LEFT, KC_DOWN, KC_RIGHT, KC_KP_0, KC_KP_DOT,
    KC_KP_ENTER},
}});
// NOLINTEND(readability-trailing-comma)
}  // namespace
}  // namespace quark::jis

namespace quark::inline quantum {
using namespace jis;

NonHoldable get_key_property([[maybe_unused]] KeyCode keycode, KeyPropertyTag<NonHoldable>) {
#ifdef _WIN32
  // IMEを通るキーは離した時にキーイベントを発生させないので押した段階で離してもらう
  switch (+keycode) {
    case JP_ZKHK:
    case JP_EISU:
    case JP_KANA: return NonHoldable::YES;
  }
#endif
  return NonHoldable::NO;
}

keypos_t get_key_property(KeyCode keycode, KeyPropertyTag<keypos_t>) {
#if QUARK_KEYBOARD_JIS_EXTRA_ROWS > 0
  if (const auto opt = get_extra_key_property<keypos_t>(keycode); opt) return *opt;
#endif
  return layout.at(+keycode);
}
}  // namespace quark::inline quantum
