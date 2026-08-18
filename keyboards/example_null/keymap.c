/**
 * @file keymap.c
 * @brief キーマップ
 * @copyright Copyright 2026 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  /* clang-format: off */
  LAYOUT(
    KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR, KC_SCRL, KC_PAUSE,
    JP_ZKHK, JP_1, JP_2, JP_3, JP_4, JP_5, JP_6, JP_7, JP_8, JP_9, JP_0, JP_MINS, JP_CIRC, JP_YEN, KC_BSPC, KC_INS, KC_HOME, KC_PGUP, KC_NUM, KC_PSLS, KC_PAST, KC_PMNS,
    KC_TAB, JP_Q, JP_W, JP_E, JP_R, JP_T, JP_Y, JP_U, JP_I, JP_O, JP_P, JP_AT, JP_LBRC, KC_DEL, KC_END, KC_PGDN, KC_P7, KC_P8, KC_P9,
    KC_CAPS, JP_A, JP_S, JP_D, JP_F, JP_G, JP_H, JP_J, JP_K, JP_L, JP_SCLN, JP_COLN, JP_RBRC, KC_ENT, KC_P4, KC_P5, KC_P6, KC_PPLS,
    KC_LSFT, JP_Z, JP_X, JP_C, JP_V, JP_B, JP_N, JP_M, JP_COMM, JP_DOT, JP_SLSH, JP_BSLS, KC_RSFT, KC_UP, KC_P1, KC_P2, KC_P3,
    KC_LCTL, KC_LGUI, KC_LALT, JP_MHEN, KC_SPC, JP_HENK, JP_KANA, KC_RALT, KC_RGUI, KC_APP, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT, KC_P0, KC_PDOT, KC_PENT
  ),
  /* clang-format: on */
};
