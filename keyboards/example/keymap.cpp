/**
 * @file keymap.cpp
 * @brief キーマップ
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#include <array>
#include <quark/sink.hpp>
#include <quark/win32/layouts/jp109.hpp>

extern "C" {
#include <common/action.h>
#include <common/action_util.h>
#include <common/action_layer.h>
}  // extern "C"

namespace quark {
using namespace jp109;

// 自環境でのマイクミュートキー
constexpr Key K_MICROPHONE_MUTE{0x102};

// 入れ替える物理キーのリスト（シフトで変化するもの）
constexpr std::array KEYS{
  /* clang-format off */
  K_HANKAKU_ZENKAKU, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_CIRCUMFLEX, K_YEN,
  K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P, K_AT, K_LBRACKET,
  K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SCOLON, K_COLON, K_RBRACKET,
  K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_DOT, K_SLASH, K_BSLASH,
  /* clang-format on */
};

// 入れ替える物理キーのリスト（シフトで変化しないもの）
constexpr std::array FN_KEYS{
  /* clang-format off */
  K_LCTRL, K_LSHIFT, K_LALT, K_LGUI,
  K_RCTRL, K_RSHIFT, K_RALT, K_RGUI,
  K_MUHENKAN, K_HENKAN, K_KATAKANA_HIRAGANA,
  K_MICROPHONE_MUTE,
  /* clang-format on */
};

// そのままにする物理キーのリスト
constexpr std::array OTHER_KEYS{
  /* clang-format off */
  K_ESCAPE, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
  K_BSPACE, K_TAB, K_CAPSLOCK, K_ENTER, K_SPACE, K_APPLICATION,
  K_PSCREEN, K_SCROLLLOCK, K_PAUSE,
  K_INSERT, K_HOME, K_PGUP,
  K_DELETE, K_END, K_PGDOWN,
  K_UP, K_LEFT, K_DOWN, K_RIGHT,
  K_NUMLOCK, K_KP_SLASH, K_KP_ASTERISK, K_KP_MINUS,
  K_KP_7, K_KP_8, K_KP_9,
  K_KP_4, K_KP_5, K_KP_6, K_KP_PLUS,
  K_KP_1, K_KP_2, K_KP_3,
  K_KP_0, K_KP_DOT, K_KP_ENTER,
  K_AUDIO_MUTE, K_AUDIO_VOL_DOWN, K_AUDIO_VOL_UP, K_MEDIA_PREV_TRACK, K_MEDIA_PLAY_PAUSE, K_MEDIA_NEXT_TRACK
  /* clang-format on */
};

// レイヤー番号
enum Layer : uint8_t {
  L_EN,         // 英語
  L_LSHIFT_EN,  // 左シフト付き英語
  L_RSHIFT_EN,  // 右シフト付き英語
  L_JA,         // 日本語
  L_LSHIFT_JA,  // 左シフト付き日本語
  L_RSHIFT_JA,  // 右シフト付き日本語
  L_QWERTY,     // QWERTY
  L_LTHUMB,     // 左親指
  L_RTHUMB,     // 右親指
};

constexpr uint8_t EN_LAYER_MASK = 0b000000111;  // 英語レイヤーのビットマスク
constexpr uint8_t JA_LAYER_MASK = 0b000111000;  // 日本語レイヤーのビットマスク
constexpr size_t EJ_LAYER_OFFSET = 3;  // 英日でレイヤーを切り替えるためのビットオフセット

// マクロID
enum MacroId {
  M_YA,  // や
  M_YU,  // ゆ
  M_YE,  // いぇ
  M_YO,  // よ
  M_BLOCK_COMMENT_BEGIN,  // ブロックコメント / **
  M_RIGHT_ARROW,          // 右矢印 ->
  M_RIGHT_DOUBLE_ARROW,   // 右二重矢印 =>
};

// アクション
constexpr action_t AC_M_YA = ACTION_MACRO(M_YA);
constexpr action_t AC_M_YU = ACTION_MACRO(M_YU);
constexpr action_t AC_M_YE = ACTION_MACRO(M_YE);
constexpr action_t AC_M_YO = ACTION_MACRO(M_YO);
constexpr action_t AC_M_BLOCK_COMMENT_BEGIN = ACTION_MACRO(M_BLOCK_COMMENT_BEGIN);
constexpr action_t AC_M_RIGHT_ARROW = ACTION_MACRO(M_RIGHT_ARROW);
constexpr action_t AC_M_RIGHT_DOUBLE_ARROW = ACTION_MACRO(M_RIGHT_DOUBLE_ARROW);

// 関数ID
enum FuncId {
  FN_LCTRL,
  FN_LSHIFT,
  FN_LALT,
  FN_LGUI,
  FN_RCTRL,
  FN_RSHIFT,
  FN_RALT,
  FN_RGUI,
  FN_KATAKANA_HIRAGANA,
  FN_MICROPHONE_MUTE,  // マイクのオン/オフを切り替える
};

// アクション
constexpr action_t AC_FN_LCTRL = ACTION_FUNCTION(FN_LCTRL);
constexpr action_t AC_FN_LSHIFT = ACTION_FUNCTION(FN_LSHIFT);
constexpr action_t AC_FN_LALT = ACTION_FUNCTION(FN_LALT);
constexpr action_t AC_FN_LGUI = ACTION_FUNCTION(FN_LGUI);
constexpr action_t AC_FN_RCTRL = ACTION_FUNCTION(FN_RCTRL);
constexpr action_t AC_FN_RSHIFT = ACTION_FUNCTION(FN_RSHIFT);
constexpr action_t AC_FN_RALT = ACTION_FUNCTION(FN_RALT);
constexpr action_t AC_FN_RGUI = ACTION_FUNCTION(FN_RGUI);
constexpr action_t AC_FN_KATAKANA_HIRAGANA = ACTION_FUNCTION(FN_KATAKANA_HIRAGANA);
constexpr action_t AC_FN_MICROPHONE_MUTE = ACTION_FUNCTION(FN_MICROPHONE_MUTE);

// 関数ID (TAP)
enum FuncTapId {
  FN_TAP_LTHUMB,  // 左親指
  FN_TAP_RTHUMB,  // 右親指
};

// アクション
constexpr action_t AC_FN_TAP_LTHUMB = ACTION_FUNCTION_TAP(FN_TAP_LTHUMB);
constexpr action_t AC_FN_TAP_RTHUMB = ACTION_FUNCTION_TAP(FN_TAP_RTHUMB);

// FN_KEYSのアクション
constexpr std::array FN_ACTIONS{
  /* clang-format off */
  AC_FN_LCTRL, AC_FN_LSHIFT, AC_FN_LALT, AC_FN_LGUI,
  AC_FN_RCTRL, AC_FN_RSHIFT, AC_FN_RALT, AC_FN_RGUI,
  AC_FN_TAP_LTHUMB, AC_FN_TAP_RTHUMB, AC_FN_KATAKANA_HIRAGANA,
  AC_FN_MICROPHONE_MUTE,
  /* clang-format on */
};
static_assert(FN_ACTIONS.size() == FN_KEYS.size());

// 英語入力（調整版Colemak）
constexpr std::array EN_ACTIONS{
  /* clang-format off */
  AC_GRAVE, AC_1, AC_2, AC_3, AC_4, AC_5, AC_6, AC_7, AC_8, AC_9, AC_0, AC_LBRACKET, AC_RBRACKET, AC_YEN,
  AC_Q, AC_W, AC_F, AC_P, AC_G, AC_J, AC_L, AC_U, AC_Y, AC_SCOLON, AC_QUOTE, AC_EQUAL,
  AC_A, AC_R, AC_S, AC_T, AC_D, AC_H, AC_N, AC_E, AC_I, AC_O, AC_UNDERSCORE, AC_ENTER,
  AC_Z, AC_X, AC_C, AC_V, AC_B, AC_K, AC_M, AC_COMMA, AC_DOT, AC_SLASH, AC_FN_RSHIFT,
  /* clang-format on */
};
static_assert(EN_ACTIONS.size() == KEYS.size());

// 左シフト付き英語入力
constexpr std::array LSHIFT_EN_ACTIONS{
  /* clang-format off */
  AC_TILDE, AC_EXCLAIM, AC_AT, AC_HASH, AC_DOLLAR, AC_PERCENT, AC_CIRCUMFLEX, AC_AMPERSAND, AC_ASTERISK, AC_LPAREN, AC_RPAREN, AC_LBRACE, AC_RBRACE, AC_PIPE,
  AC_SHIFT_Q, AC_SHIFT_W, AC_SHIFT_F, AC_SHIFT_P, AC_SHIFT_G, AC_SHIFT_J, AC_SHIFT_L, AC_SHIFT_U, AC_SHIFT_Y, AC_COLON, AC_DQUOTE, AC_PLUS,
  AC_SHIFT_A, AC_SHIFT_R, AC_SHIFT_S, AC_SHIFT_T, AC_SHIFT_D, AC_SHIFT_H, AC_SHIFT_N, AC_SHIFT_E, AC_SHIFT_I, AC_SHIFT_O, AC_EQUAL, AC_SHIFT_ENTER,
  AC_SHIFT_Z, AC_SHIFT_X, AC_SHIFT_C, AC_SHIFT_V, AC_SHIFT_B, AC_SHIFT_K, AC_SHIFT_M, AC_LT, AC_GT, AC_QUESTION, AC_FN_RSHIFT,
  /* clang-format on */
};
static_assert(LSHIFT_EN_ACTIONS.size() == KEYS.size());

// 右シフト付き英語入力
const std::array RSHIFT_EN_ACTIONS = [] {
  auto actions = LSHIFT_EN_ACTIONS;
  for (auto& action : actions) {
    if (action.key.kind == ACT_LMODS) action.key.kind = ACT_RMODS;
  }
  return actions;
}();

// 日本語入力（たちあおい v2.4.1）
constexpr std::array JA_ACTIONS{
  /* clang-format off */
  AC_HANKAKU_ZENKAKU, AC_1, AC_2, AC_3, AC_4, AC_5, AC_6, AC_7, AC_8, AC_9, AC_0, AC_MINUS, AC_CIRCUMFLEX, AC_YEN,
  AC_P, AC_R, AC_D, AC_G, AC_F, AC_L, AC_M_YO, AC_U, AC_O, AC_Q, AC_AT, AC_LBRACKET,
  AC_N, AC_S, AC_T, AC_K, AC_H, AC_M_YU, AC_A, AC_I, AC_E, AC_SCOLON, AC_COLON, AC_RBRACKET,
  AC_W, AC_Z, AC_B, AC_M, AC_V, AC_M_YA, AC_M_YE, AC_COMMA, AC_DOT, AC_SLASH, AC_BSLASH,
  /* clang-format on */
};
static_assert(JA_ACTIONS.size() == KEYS.size());

// 日本語QWERTY
constexpr std::array QWERTY_ACTIONS{
  /* clang-format off */
  AC_HANKAKU_ZENKAKU, AC_1, AC_2, AC_3, AC_4, AC_5, AC_6, AC_7, AC_8, AC_9, AC_0, AC_MINUS, AC_CIRCUMFLEX, AC_YEN,
  AC_Q, AC_W, AC_E, AC_R, AC_T, AC_Y, AC_U, AC_I, AC_O, AC_P, AC_AT, AC_LBRACKET,
  AC_A, AC_S, AC_D, AC_F, AC_G, AC_H, AC_J, AC_K, AC_L, AC_SCOLON, AC_COLON, AC_RBRACKET,
  AC_Z, AC_X, AC_C, AC_V, AC_B, AC_N, AC_M, AC_COMMA, AC_DOT, AC_SLASH, AC_BSLASH,
  /* clang-format on */
};
static_assert(QWERTY_ACTIONS.size() == KEYS.size());

// 親指と同時押しで入力する記号群
constexpr std::array THUMB_ACTIONS{
  /* clang-format off */
  AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO, AC_NO,
  AC_NO, AC_MINUS, AC_PLUS, AC_NO, AC_NO, AC_NO, AC_NO, AC_PIPE, AC_TILDE, AC_NO, AC_GRAVE, AC_NO,
  AC_LBRACKET, AC_ASTERISK, AC_LBRACE, AC_LPAREN, AC_HASH, AC_DOLLAR, AC_RPAREN, AC_RBRACE, AC_AMPERSAND, AC_RBRACKET, AC_M_RIGHT_DOUBLE_ARROW, AC_NO,
  AC_NO, AC_PERCENT, AC_EXCLAIM, AC_BSLASH, AC_NO, AC_NO, AC_AT, AC_CIRCUMFLEX, AC_M_RIGHT_ARROW, AC_M_BLOCK_COMMENT_BEGIN, AC_NO,
  /* clang-format on */
};
static_assert(THUMB_ACTIONS.size() == KEYS.size());

extern "C" {
action_t action_for_key(uint8_t layer, keypos_t pos) {
  const size_t index = pos.row * MATRIX_COLS + pos.col;
  if (index < KEYS.size()) {
    switch (layer) {
      case L_EN:
        return EN_ACTIONS[index];
      case L_LSHIFT_EN:
        set_weak_mods_mask(0x22);  // アクションで定義されるSHIFTを優先する
        return LSHIFT_EN_ACTIONS[index];
      case L_RSHIFT_EN:
        set_weak_mods_mask(0x22);
        return RSHIFT_EN_ACTIONS[index];
      case L_JA:
        return JA_ACTIONS[index];
      case L_LSHIFT_JA:
        set_weak_mods_mask(0x22);
        return LSHIFT_EN_ACTIONS[index];  // SHIFT中は英語入力にする
      case L_RSHIFT_JA:
        set_weak_mods_mask(0x22);
        return RSHIFT_EN_ACTIONS[index];
      case L_QWERTY:
        return QWERTY_ACTIONS[index];
      case L_LTHUMB:
        return THUMB_ACTIONS[index];
      case L_RTHUMB:
        return THUMB_ACTIONS[index];
      default:
        return ACTION_NO;
    }
  } else if (index < KEYS.size() + FN_KEYS.size()) {
    return FN_ACTIONS[index - KEYS.size()];
  } else if (index < KEYS.size() + FN_KEYS.size() + OTHER_KEYS.size()) {
    // それ以外はそのまま
    return {.code = to_keycode(OTHER_KEYS[index - KEYS.size() - FN_KEYS.size()])};
  }
  return ACTION_NO;
}

const macro_t* action_get_macro(keyrecord_t* record, uint8_t id, [[maybe_unused]] uint8_t opt) {
  if (record && record->event.pressed) {
    switch (id) {
      case M_YA:
        return MACRO(U(Y), U(A), T(Y), T(A), END);
      case M_YU:
        return MACRO(U(Y), U(U), T(Y), T(U), END);
      case M_YE:
        return MACRO(U(Y), U(E), T(Y), T(E), END);
      case M_YO:
        return MACRO(U(Y), U(O), T(Y), T(O), END);
      case M_BLOCK_COMMENT_BEGIN:
        return MACRO(T(SLASH), SFT_(T(COLON), T(COLON)), END);
      case M_RIGHT_ARROW:
        return MACRO(T(MINUS), SFT_(T(DOT)), END);
      case M_RIGHT_DOUBLE_ARROW:
        return MACRO(SFT_(T(MINUS), T(DOT)), END);
    }
  }
  return nullptr;
}

void action_function(keyrecord_t* record, uint8_t id, uint8_t opt) {
  if (!record) return;
  const auto& event = record->event;
  const auto& tap = record->tap;

  if (opt & FUNC_TAP) {
    switch (id) {
      case FN_TAP_LTHUMB: {
        if (event.pressed) {
          if (tap.count > 0) {
            // 単打の場合、日本語レイヤーのビットをすべて消して英語入力に切り替える
            layer_state &= ~JA_LAYER_MASK;
            register_code(KC_MUHENKAN);  // IMEをオフにするため、無変換を押したことにする
          } else {
            // 同時押しの場合、親指用レイヤーに切り替える
            layer_on(L_LTHUMB);
          }
        } else {
          if (tap.count > 0) {
            unregister_code(KC_MUHENKAN);
          } else {
            layer_off(L_LTHUMB);
          }
        }
        break;
      }
      case FN_TAP_RTHUMB: {
        if (event.pressed) {
          if (tap.count > 0) {
            // 単打の場合、英語レイヤーのビットを日本語レイヤーにコピーして日本語入力に切り替える
            layer_state |= ((layer_state & EN_LAYER_MASK) | 1) << EJ_LAYER_OFFSET;
            register_code(KC_HENKAN);  // IMEをオンにするため、変換を押したことにする
          } else {
            // 同時押しの場合、親指用レイヤーに切り替える
            layer_on(L_RTHUMB);
          }
        } else {
          if (tap.count > 0) {
            unregister_code(KC_HENKAN);
          } else {
            layer_off(L_RTHUMB);
          }
        }
        break;
      }
    }
  } else {
    switch (id) {
      case FN_LSHIFT: {
        // LSHIFT付きレイヤーをONにする
        // 英語レイヤーは常に行うが、日本語レイヤーはレイヤーが有効なときだけ行う
        if (event.pressed) {
          layer_on(L_LSHIFT_EN);
          if (layer_state & (1 << L_JA)) layer_on(L_LSHIFT_JA);
          register_mods(MOD_BIT(id - FN_LCTRL));
        } else {
          unregister_mods(MOD_BIT(id - FN_LCTRL));
          layer_off(L_LSHIFT_EN);
          if (layer_state & (1 << L_JA)) layer_off(L_LSHIFT_JA);
        }
        break;
      }
      case FN_RSHIFT: {
        // RSHIFT付きレイヤーをONにする
        // 英語レイヤーは常に行うが、日本語レイヤーはレイヤーが有効なときだけ行う
        if (event.pressed) {
          layer_on(L_RSHIFT_EN);
          if (layer_state & (1 << L_JA)) layer_on(L_RSHIFT_JA);
          register_mods(MOD_BIT(id - FN_LCTRL));
        } else {
          unregister_mods(MOD_BIT(id - FN_LCTRL));
          layer_off(L_RSHIFT_EN);
          if (layer_state & (1 << L_JA)) layer_off(L_RSHIFT_JA);
        }
        break;
      }
      case FN_LCTRL:
      case FN_LALT:
      case FN_LGUI:
      case FN_RCTRL:
      case FN_RALT:
      case FN_RGUI: {
        // SHIFT以外のいずれかの修飾キーを押している間だけQWERTYにする
        if (event.pressed) {
          if ((get_mods() & 0xdd) == 0) layer_on(L_QWERTY);
          register_mods(MOD_BIT(id - FN_LCTRL));
        } else {
          unregister_mods(MOD_BIT(id - FN_LCTRL));
          if ((get_mods() & 0xdd) == 0) layer_off(L_QWERTY);
        }
        break;
      }
      case FN_KATAKANA_HIRAGANA: {
        if (event.pressed) {
          // 日本語レイヤーのビットをすべて消して英語入力に切り替える
          layer_state &= ~JA_LAYER_MASK;
          register_code(KC_KATAKANA_HIRAGANA);  // IMEを操作するため、かなを押したことにする
        } else {
          unregister_code(KC_KATAKANA_HIRAGANA);
        }
        break;
      }
      case FN_MICROPHONE_MUTE: {
        if (event.pressed) {
#ifdef WIN32
          // HACK: 自環境のマイクキーの入力を模倣している
          send_to_sink(NativeSinkEvent{0x2, KEYEVENTF_EXTENDEDKEY});
          send_to_sink(NativeSinkEvent{0x2, KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY});
          send_to_sink(NativeSinkEvent{0x58, KEYEVENTF_KEYUP});
#endif
        }
        break;
      }
    }
  }
}
}  // extern "C"
}  // namespace quark

#ifdef _WIN32
#include <quark/win32/settings.hpp>

namespace quark::inline win32 {
using namespace jp109;

const KeyToKeyposTable key_to_keypos_table = [] {
  KeyToKeyposTable t{};
  std::fill(t.begin(), t.end(), keypos_t{0xff, 0xff});
  size_t i = 0;
  for (auto key : KEYS) {
    t[key] = keypos_t{static_cast<uint8_t>(i % MATRIX_COLS), static_cast<uint8_t>(i / MATRIX_COLS)};
    i++;
  }
  for (auto key : FN_KEYS) {
    t[key] = keypos_t{static_cast<uint8_t>(i % MATRIX_COLS), static_cast<uint8_t>(i / MATRIX_COLS)};
    i++;
  }
  for (auto key : OTHER_KEYS) {
    t[key] = keypos_t{static_cast<uint8_t>(i % MATRIX_COLS), static_cast<uint8_t>(i / MATRIX_COLS)};
    i++;
  }
  return t;
}();
}  // namespace quark::inline win32
#endif
