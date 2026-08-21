// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keymap.c
 * @brief キーマップ
 */
#include QMK_KEYBOARD_H

#include <quantum.h>

enum Layer {
  L_EN,        // 英語
  L_SHIFT_EN,  // シフト付き英語
  L_JA,        // 日本語
  L_SHIFT_JA,  // シフト付き日本語
  L_QWERTY,    // QWERTY
  L_LTHUMB,    // 左親指
  L_RTHUMB,    // 右親指
};

enum Key {
  // 修飾キー
  LM_LCTL = LM(L_QWERTY, MOD_LCTL),
  LM_LSFT = LM(L_SHIFT_EN, MOD_LSFT),
  LM_LALT = LM(L_QWERTY, MOD_LALT),
  LM_LGUI = LM(L_QWERTY, MOD_LGUI),
  LM_RCTL = LM(L_QWERTY, MOD_RCTL),
  LM_RSFT = LM(L_SHIFT_EN, MOD_RSFT),
  LM_RALT = LM(L_QWERTY, MOD_RALT),
  LM_RGUI = LM(L_QWERTY, MOD_RGUI),

  // 親指TAPキー
  LT_LTHUMB = LT(L_LTHUMB, JP_MHEN),
  LT_RTHUMB = LT(L_RTHUMB, JP_HENK),
  LT_RRTHUMB = JP_KANA,  // 予備

  // シフト中でもシフトせずに入力したいキー
  JP_UNSHIFTED_AT = SAFE_RANGE,
  JP_UNSHIFTED_CIRC,
  JP_UNSHIFTED_COLN,

  // マクロ
  M_YA,
  M_YU,
  M_YE,
  M_YO,
  M_RIGHT_DOUBLE_ARROW,
  M_RIGHT_ARROW,
  M_OPENING_BLOCK_COMMENT,
};

/* clang-format off */

/**
 * @brief 必要なところだけを記述するためのレイアウト
 */
#define LAYOUT_simple( \
    HANKAKU_ZENKAKU, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, MINUS, CIRCUMFLEX, YEN, \
    Q, W, E, R, T, Y, U, I, O, P, AT, LEFT_BRACKET, \
    A, S, D, F, G, H, J, K, L, SEMICOLON, COLON, RIGHT_BRACKET, \
    Z, X, C, V, B, N, M, COMMA, DOT, SLASH, BACKSLASH \
) { \
  {KC_ESCAPE, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_PRINT_SCREEN, KC_SCROLL_LOCK, KC_PAUSE}, \
  {HANKAKU_ZENKAKU, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, MINUS, CIRCUMFLEX, YEN, KC_BACKSPACE, KC_INSERT, KC_HOME, KC_PAGE_UP, KC_NUM_LOCK, KC_KP_SLASH, KC_KP_ASTERISK, KC_KP_MINUS}, \
  {KC_TAB, Q, W, E, R, T, Y, U, I, O, P, AT, LEFT_BRACKET, KC_DELETE, KC_END, KC_PAGE_DOWN, KC_KP_7, KC_KP_8, KC_KP_9}, \
  {KC_CAPS_LOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, COLON, RIGHT_BRACKET, KC_ENTER, KC_KP_4, KC_KP_5, KC_KP_6, KC_KP_PLUS}, \
  {LM_LSFT, Z, X, C, V, B, N, M, COMMA, DOT, SLASH, BACKSLASH, LM_RSFT, KC_UP, KC_KP_1, KC_KP_2, KC_KP_3}, \
  {LM_LCTL, LM_LGUI, LM_LALT, LT_LTHUMB, KC_SPACE, LT_RTHUMB, LT_RRTHUMB, LM_RALT, LM_RGUI, KC_APPLICATION, LM_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT, KC_KP_0, KC_KP_DOT, KC_KP_ENTER} \
}

/* clang-format on */

/**
 * @brief キーマップ
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // 英語（調整版Colemak）
    [L_EN] = LAYOUT_simple(
        JP_GRV, JP_1, JP_2, JP_3, JP_4, JP_5, JP_6, JP_7, JP_8, JP_9, JP_0,
        JP_LBRC, JP_RBRC, JP_YEN, JP_Q, JP_W, JP_F, JP_P, JP_G, JP_J, JP_L,
        JP_U, JP_Y, JP_SCLN, JP_QUOT, JP_EQL, JP_A, JP_R, JP_S, JP_T, JP_D,
        JP_H, JP_N, JP_E, JP_I, JP_O, JP_UNDS, KC_ENT, JP_Z, JP_X, JP_C, JP_V,
        JP_B, JP_K, JP_M, JP_COMM, JP_DOT, JP_SLSH, LM_RSFT),

    // シフト付き英語（調整版Colemak）
    [L_SHIFT_EN] = LAYOUT_simple(
        JP_TILD, JP_EXLM, JP_UNSHIFTED_AT, JP_HASH, JP_DLR, JP_PERC,
        JP_UNSHIFTED_CIRC, JP_AMPR, JP_ASTR, JP_LPRN, JP_RPRN, JP_LBRC, JP_RBRC,
        JP_PIPE, S(JP_Q), S(JP_W), S(JP_F), S(JP_P), S(JP_G), S(JP_J), S(JP_L),
        S(JP_U), S(JP_Y), JP_UNSHIFTED_COLN, JP_DQUO, JP_PLUS, S(JP_A), S(JP_R),
        S(JP_S), S(JP_T), S(JP_D), S(JP_H), S(JP_N), S(JP_E), S(JP_I), S(JP_O),
        JP_EQL, S(KC_ENT), S(JP_Z), S(JP_X), S(JP_C), S(JP_V), S(JP_B), S(JP_K),
        S(JP_M), JP_LABK, JP_RABK, JP_QUES, LM_RSFT),

    // 日本語（たちあおい v2.4.1）
    [L_JA] = LAYOUT_simple(
        JP_ZKHK, JP_1, JP_2, JP_3, JP_4, JP_5, JP_6, JP_7, JP_8, JP_9, JP_0,
        JP_MINS, JP_CIRC, JP_YEN, JP_P, JP_R, JP_D, JP_G, JP_F, JP_L, M_YO,
        JP_U, JP_O, JP_Q, JP_AT, JP_LBRC, JP_N, JP_S, JP_T, JP_K, JP_H, M_YU,
        JP_A, JP_I, JP_E, JP_SCLN, JP_COLN, JP_RBRC, JP_W, JP_Z, JP_B, JP_M,
        JP_V, M_YA, M_YE, JP_COMM, JP_DOT, JP_SLSH, JP_BSLS),

    // シフト付き日本語（調整版Colemak）
    [L_SHIFT_JA] = LAYOUT_simple(
        JP_TILD, JP_EXLM, JP_UNSHIFTED_AT, JP_HASH, JP_DLR, JP_PERC,
        JP_UNSHIFTED_CIRC, JP_AMPR, JP_ASTR, JP_LPRN, JP_RPRN, JP_LBRC, JP_RBRC,
        JP_PIPE, S(JP_Q), S(JP_W), S(JP_F), S(JP_P), S(JP_G), S(JP_J), S(JP_L),
        S(JP_U), S(JP_Y), JP_UNSHIFTED_COLN, JP_DQUO, JP_PLUS, S(JP_A), S(JP_R),
        S(JP_S), S(JP_T), S(JP_D), S(JP_H), S(JP_N), S(JP_E), S(JP_I), S(JP_O),
        JP_EQL, S(KC_ENT), S(JP_Z), S(JP_X), S(JP_C), S(JP_V), S(JP_B), S(JP_K),
        S(JP_M), JP_LABK, JP_RABK, JP_QUES, LM_RSFT),

    // QWERTY（Ctrl,Alt,Gui用）
    [L_QWERTY] = LAYOUT_simple(
        JP_ZKHK, JP_1, JP_2, JP_3, JP_4, JP_5, JP_6, JP_7, JP_8, JP_9, JP_0,
        JP_MINS, JP_CIRC, JP_YEN, JP_Q, JP_W, JP_E, JP_R, JP_T, JP_Y, JP_U,
        JP_I, JP_O, JP_P, JP_AT, JP_LBRC, JP_A, JP_S, JP_D, JP_F, JP_G, JP_H,
        JP_J, JP_K, JP_L, JP_SCLN, JP_COLN, JP_RBRC, JP_Z, JP_X, JP_C, JP_V,
        JP_B, JP_N, JP_M, JP_COMM, JP_DOT, JP_SLSH, JP_BSLS),

    // 左親指（記号類）
    [L_LTHUMB] = LAYOUT_simple(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, JP_MINS, JP_PLUS, KC_NO, KC_NO,
        KC_NO, KC_NO, JP_PIPE, JP_TILD, KC_NO, JP_GRV, KC_NO, JP_LBRC, JP_ASTR,
        JP_LCBR, JP_LPRN, JP_HASH, JP_DLR, JP_RPRN, JP_RCBR, JP_AMPR, JP_RBRC,
        M_RIGHT_DOUBLE_ARROW, KC_NO, KC_NO, JP_PERC, JP_EXLM, JP_BSLS, KC_NO,
        KC_NO, JP_AT, JP_CIRC, M_RIGHT_ARROW, M_OPENING_BLOCK_COMMENT, KC_NO),

    // 右親指（記号類）
    [L_RTHUMB] = LAYOUT_simple(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, JP_MINS, JP_PLUS, KC_NO, KC_NO,
        KC_NO, KC_NO, JP_PIPE, JP_TILD, KC_NO, JP_GRV, KC_NO, JP_LBRC, JP_ASTR,
        JP_LCBR, JP_LPRN, JP_HASH, JP_DLR, JP_RPRN, JP_RCBR, JP_AMPR, JP_RBRC,
        M_RIGHT_DOUBLE_ARROW, KC_NO, KC_NO, JP_PERC, JP_EXLM, JP_BSLS, KC_NO,
        KC_NO, JP_AT, JP_CIRC, M_RIGHT_ARROW, M_OPENING_BLOCK_COMMENT, KC_NO),
};

/**
 * @brief シフトキーを一時的に無効にしてからキーを処理する
 *
 * @param pressed キーが押されているか
 * @param keycode 操作するキーのコード
 */
static void process_unshifted_key(bool pressed, uint16_t keycode) {
  if (pressed) {
    const uint8_t mods = get_mods();
    del_mods(MOD_MASK_SHIFT);  // シフトを無効化
    register_code16(keycode);
    add_mods(mods);  // もとに戻す
  } else {
    unregister_code16(keycode);
  }
}

/**
 * @brief 独自のキーコードを処理する
 *
 * @param keycode キーコード
 * @param record キー入力の状態
 * @return true 処理を継続する
 * @return false 処理を完了する
 */
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
  if (!record) return true;

  const bool pressed = record->event.pressed;
  const uint8_t tap_count = record->tap.count;

  switch (keycode) {
    // いずれかのShiftを押している間だけレイヤーをONにする
    case LM_LSFT:  // fallthrough
    case LM_RSFT: {
      const uint8_t mods = MOD_BIT(keycode - LM_LCTL);
      if (pressed) {
        // 最初であれば、レイヤーをONにする
        if ((get_mods() & MOD_MASK_SHIFT) == 0) {
          // 英語レイヤーは常に操作する
          layer_on(L_SHIFT_EN);

          // 日本語レイヤーは有効な場合のみ操作する
          if (layer_state_is(L_JA)) {
            layer_on(L_SHIFT_JA);
          }
        }

        register_mods(mods);
      } else {
        unregister_mods(mods);

        // 最後であれば、レイヤーをOFFにする
        if ((get_mods() & MOD_MASK_SHIFT) == 0) {
          // 英語レイヤーは常に操作する
          layer_off(L_SHIFT_EN);

          // 日本語レイヤーは有効な場合のみ操作する
          if (layer_state_is(L_JA)) {
            layer_off(L_SHIFT_JA);
          }
        }
      }
      return false;
    }

    // いずれかのCtrl,Alt,Guiを押している間だけレイヤーをONにする
    case LM_LCTL:  // fallthrough
    case LM_LALT:  // fallthrough
    case LM_LGUI:  // fallthrough
    case LM_RCTL:  // fallthrough
    case LM_RALT:  // fallthrough
    case LM_RGUI: {
      const uint8_t mods = MOD_BIT(keycode - LM_LCTL);
      if (pressed) {
        // 最初であれば、レイヤーをONにする
        if ((get_mods() & MOD_MASK_CAG) == 0) {
          layer_on(L_QWERTY);
        }

        register_mods(mods);
      } else {
        unregister_mods(mods);

        // 最後であれば、レイヤーをOFFにする
        if ((get_mods() & MOD_MASK_CAG) == 0) {
          layer_off(L_QWERTY);
        }
      }
      return false;
    }

    // 左親指TAPキー
    case LT_LTHUMB: {
      if (pressed) {
        if (tap_count > 0) {
          // 単打ならば、日本語レイヤーをOFFにしてキーを押す
          layer_off(L_JA);
          layer_off(L_SHIFT_JA);

          register_code16(JP_MHEN);
        } else {
          // 複打ならば、親指レイヤーをONにする
          layer_on(L_LTHUMB);
        }
      } else {
        if (tap_count > 0) {
          // 単打ならば、キーを離す
          unregister_code16(JP_MHEN);
        } else {
          // 複打ならば、親指レイヤーをOFFにする
          layer_off(L_LTHUMB);
        }
      }
      return false;
    }

    // 右親指TAPキー
    case LT_RTHUMB: {
      if (pressed) {
        if (tap_count > 0) {
          // 単打ならば、日本語レイヤーをONにしてキーを押す
          layer_on(L_JA);

          // 英語レイヤーからシフト状態を移植する
          if (layer_state_is(L_SHIFT_EN)) {
            layer_on(L_SHIFT_JA);
          }

          register_code16(JP_HENK);
        } else {
          // 複打ならば、親指レイヤーをONにする
          layer_on(L_RTHUMB);
        }
      } else {
        if (tap_count > 0) {
          // 単打ならば、キーを離す
          unregister_code16(JP_HENK);
        } else {
          // 複打ならば、親指レイヤーをOFFにする
          layer_off(L_RTHUMB);
        }
      }
      return false;
    }

    // もうひとつ右の親指キー
    case LT_RRTHUMB: {
      // TODO: 必要ならTAPキーにする
      if (pressed) {
        // 日本語レイヤーをOFFにしてキーを押す
        layer_off(L_JA);
        layer_off(L_SHIFT_JA);

        register_code16(JP_KANA);
      } else {
        unregister_code16(JP_KANA);
      }
      return false;
    }

    // シフトを無視した@キー
    case JP_UNSHIFTED_AT: {
      process_unshifted_key(pressed, JP_AT);
      return false;
    }

    // シフトを無視した^キー
    case JP_UNSHIFTED_CIRC: {
      process_unshifted_key(pressed, JP_CIRC);
      return false;
    }

    // シフトを無視した:キー
    case JP_UNSHIFTED_COLN: {
      process_unshifted_key(pressed, JP_COLN);
      return false;
    }

    // や
    case M_YA: {
      if (pressed) {
        tap_code16(KC_Y);
        tap_code16(KC_A);
      }
      return false;
    }

    // ゆ
    case M_YU: {
      if (pressed) {
        tap_code16(KC_Y);
        tap_code16(KC_U);
      }
      return false;
    }

    // いぇ
    case M_YE: {
      if (pressed) {
        tap_code16(KC_Y);
        tap_code16(KC_E);
      }
      return false;
    }

    // よ
    case M_YO: {
      if (pressed) {
        tap_code16(KC_Y);
        tap_code16(KC_O);
      }
      return false;
    }

    // 右向き二重矢印
    case M_RIGHT_DOUBLE_ARROW: {
      if (pressed) {
        tap_code16(JP_EQL);
        tap_code16(JP_RABK);
      }
      return false;
    }

    // 右向き矢印
    case M_RIGHT_ARROW: {
      if (pressed) {
        tap_code16(JP_MINS);
        tap_code16(JP_RABK);
      }
      return false;
    }

    // ブロックコメント（開き）
    case M_OPENING_BLOCK_COMMENT: {
      if (pressed) {
        tap_code16(JP_SLSH);
        tap_code16(JP_ASTR);
        tap_code16(JP_ASTR);
      }
      return false;
    }

    default:
      break;
  }
  return true;
}
