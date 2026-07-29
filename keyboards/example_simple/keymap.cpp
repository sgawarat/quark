/**
 * @file keymap.cpp
 * @brief キーマップ
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#include <array>
#include <tmk_desktop/sink.hpp>
#include <tmk_desktop/win32/layouts/jp109.hpp>

extern "C" {
#include <common/action.h>
#include <common/action_layer.h>
#include <common/action_util.h>
}  // extern "C"

namespace tmk_desktop {
using namespace jp109;

// 自環境でのマイクミュートキー
constexpr Key K_MICROPHONE_MUTE{0x102};

// 物理キーのリスト
constexpr std::array KEYS{
    /* clang-format off */
  // 入れ替えるキー
  K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P,
  K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L,
  K_Z, K_X, K_C, K_V, K_B, K_N, K_M,
  K_LCTRL, K_LSHIFT, K_LALT, K_LGUI,
  K_RCTRL, K_RSHIFT, K_RALT, K_RGUI,
  K_MUHENKAN, K_HENKAN, K_KATAKANA_HIRAGANA,
  K_MICROPHONE_MUTE,

  // そのままにするキー
  K_ESCAPE, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
  K_HANKAKU_ZENKAKU, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_CIRCUMFLEX, K_YEN,
  K_AT, K_LBRACKET,
  K_SCOLON, K_COLON, K_RBRACKET,
  K_COMMA, K_DOT, K_SLASH, K_BSLASH,
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
  L_IME_OFF,            // 英語
  L_IME_OFF_WITH_MODS,  // 英語（修飾キーあり）
  L_IME_ON,             // 日本語
  L_IME_ON_WITH_MODS,   // 日本語（修飾キーあり）
};

// マクロ
enum MacroId {
  M_YA,  // や
  M_YU,  // ゆ
  M_YE,  // いぇ
  M_YO,  // よ
};

constexpr action_t AC_YA = ACTION_MACRO(M_YA);
constexpr action_t AC_YU = ACTION_MACRO(M_YU);
constexpr action_t AC_YE = ACTION_MACRO(M_YE);
constexpr action_t AC_YO = ACTION_MACRO(M_YO);

// ファンクション
enum FuncId {
  FN_LCTRL,
  FN_LSHIFT,
  FN_LALT,
  FN_LGUI,
  FN_RCTRL,
  FN_RSHIFT,
  FN_RALT,
  FN_RGUI,
  FN_IME_OFF,
  FN_IME_ON,
  FN_IME_OFF_KANA,
  FN_MICROPHONE_MUTE,  // マイクのオン/オフを切り替える
};

constexpr action_t AC_FN_LCTRL = ACTION_FUNCTION(FN_LCTRL);
constexpr action_t AC_FN_LSHIFT = ACTION_FUNCTION(FN_LSHIFT);
constexpr action_t AC_FN_LALT = ACTION_FUNCTION(FN_LALT);
constexpr action_t AC_FN_LGUI = ACTION_FUNCTION(FN_LGUI);
constexpr action_t AC_FN_RCTRL = ACTION_FUNCTION(FN_RCTRL);
constexpr action_t AC_FN_RSHIFT = ACTION_FUNCTION(FN_RSHIFT);
constexpr action_t AC_FN_RALT = ACTION_FUNCTION(FN_RALT);
constexpr action_t AC_FN_RGUI = ACTION_FUNCTION(FN_RGUI);
constexpr action_t AC_FN_IME_OFF = ACTION_FUNCTION(FN_IME_OFF);
constexpr action_t AC_FN_IME_ON = ACTION_FUNCTION(FN_IME_ON);
constexpr action_t AC_FN_IME_OFF_KANA = ACTION_FUNCTION(FN_IME_OFF_KANA);
constexpr action_t AC_FN_MICROPHONE_MUTE = ACTION_FUNCTION(FN_MICROPHONE_MUTE);

// IMEオフのときのアクションリスト
constexpr std::array IME_OFF_ACTIONS{
    /* clang-format off */
  AC_Q,AC_W,AC_E,AC_R,AC_T,AC_Y,AC_U,AC_I,AC_O,AC_P,
  AC_A,AC_S,AC_D,AC_F,AC_G,AC_H,AC_J,AC_K,AC_L,
  AC_Z,AC_X,AC_C,AC_V,AC_B,AC_N,AC_M,
  AC_FN_LCTRL, AC_FN_LSHIFT, AC_FN_LALT, AC_FN_LGUI,
  AC_FN_RCTRL, AC_FN_RSHIFT, AC_FN_RALT, AC_FN_RGUI,
  AC_FN_IME_OFF, AC_FN_IME_ON, AC_FN_IME_OFF_KANA,
  AC_FN_MICROPHONE_MUTE,
    /* clang-format on */
};

// IMEオンのときのアクションリスト
constexpr std::array IME_ON_ACTIONS{
    /* clang-format off */
  // v2.1
  // AC_P,AC_D,AC_R,AC_G,AC_F,AC_YE,AC_YO,AC_U,AC_O,AC_Q,
  // AC_N,AC_S,AC_T,AC_K,AC_H,AC_YU,AC_A,AC_I,AC_E,
  // AC_Z,AC_B,AC_W,AC_M,AC_V,AC_YA,AC_L,

  // v2.2
  // AC_P,AC_D,AC_R,AC_G,AC_F,AC_YE,AC_YO,AC_U,AC_O,AC_Q,
  // AC_N,AC_T,AC_S,AC_K,AC_H,AC_YU,AC_A,AC_I,AC_E,
  // AC_B,AC_W,AC_Z,AC_M,AC_V,AC_YA,AC_L,

  // v2.3
  // AC_P,AC_H,AC_D,AC_M,AC_F,AC_YE,AC_YO,AC_U,AC_O,AC_Q,
  // AC_N,AC_S,AC_T,AC_K,AC_R,AC_YU,AC_A,AC_I,AC_E,
  // AC_W,AC_Z,AC_B,AC_G,AC_V,AC_YA,AC_L,

  // v2.3.1
  // AC_P,AC_M,AC_D,AC_H,AC_F,AC_YE,AC_YO,AC_U,AC_O,AC_Q,
  // AC_N,AC_K,AC_T,AC_S,AC_R,AC_YU,AC_A,AC_I,AC_E,
  // AC_W,AC_G,AC_B,AC_Z,AC_V,AC_YA,AC_L,

  // v2.4
  // AC_P,AC_R,AC_D,AC_G,AC_F,AC_YE,AC_YO,AC_U,AC_O,AC_Q,
  // AC_N,AC_S,AC_T,AC_K,AC_H,AC_YU,AC_A,AC_I,AC_E,
  // AC_W,AC_Z,AC_B,AC_M,AC_V,AC_YA,AC_L,

  // v2.4.1
  AC_P,AC_R,AC_D,AC_G,AC_F,AC_L,AC_YO,AC_U,AC_O,AC_Q,
  AC_N,AC_S,AC_T,AC_K,AC_H,AC_YU,AC_A,AC_I,AC_E,
  AC_W,AC_Z,AC_B,AC_M,AC_V,AC_YA,AC_YE,

  AC_FN_LCTRL, AC_FN_LSHIFT, AC_FN_LALT, AC_FN_LGUI,
  AC_FN_RCTRL, AC_FN_RSHIFT, AC_FN_RALT, AC_FN_RGUI,
  AC_FN_IME_OFF, AC_FN_IME_ON, AC_FN_IME_OFF_KANA,
  AC_FN_MICROPHONE_MUTE,
    /* clang-format on */
};

static_assert(IME_OFF_ACTIONS.size() == IME_ON_ACTIONS.size());

extern "C" {
action_t action_for_key(uint8_t layer, keypos_t pos) {
  const size_t i = pos.row * MATRIX_COLS + pos.col;
  // 修飾キーを押していない日本語入力のときだけ配列を切り替える
  if (i < IME_OFF_ACTIONS.size()) {
    switch (layer) {
      case L_IME_OFF:
      case L_IME_OFF_WITH_MODS:
      case L_IME_ON_WITH_MODS:
        return IME_OFF_ACTIONS[i];
      case L_IME_ON:
        return IME_ON_ACTIONS[i];
    }
    return ACTION_NO;
  }

  // その他はそのままのキーコードを返す
  if (i < KEYS.size()) return {.code = to_keycode(KEYS[i])};
  return ACTION_NO;
}

const macro_t* action_get_macro(keyrecord_t* record, uint8_t id, [[maybe_unused]] uint8_t opt) {
  if (record && record->event.pressed) {
    switch (id) {
      case M_YA:
        return MACRO(T(Y), T(A), END);
      case M_YU:
        return MACRO(T(Y), T(U), END);
      case M_YE:
        return MACRO(T(Y), T(E), END);
      case M_YO:
        return MACRO(T(Y), T(O), END);
    }
  }
  return nullptr;
}

void action_function(keyrecord_t* record, uint8_t id, uint8_t opt) {
  if (!record) return;
  const auto& event = record->event;
  const auto& tap = record->tap;

  if (opt & FUNC_TAP) {
    return;
  } else {
    switch (id) {
      case FN_LCTRL:
      case FN_LSHIFT:
      case FN_LALT:
      case FN_LGUI:
      case FN_RCTRL:
      case FN_RSHIFT:
      case FN_RALT:
      case FN_RGUI: {
        // いずれかの修飾キーを押している間だけレイヤーをずらす
        if (event.pressed) {
          if (get_mods() == 0) {
            if (layer_state == 0) layer_state = 1;
            layer_state <<= 1;
          }
          register_mods(MOD_BIT(id));
        } else {
          unregister_mods(MOD_BIT(id));
          if (get_mods() == 0) {
            layer_state >>= 1;
          }
        }
        send_keyboard_report();
        break;
      }
      case FN_IME_OFF: {
        // IMEをオフにしたときにレイヤーを右にずらす
        if (event.pressed) {
          if (layer_state & (0b11 << L_IME_ON)) {
            layer_state >>= L_IME_ON;
          }
          register_code(KC_MUHENKAN);
        } else {
          unregister_code(KC_MUHENKAN);
        }
        send_keyboard_report();
        break;
      }
      case FN_IME_ON: {
        // IMEをオンにしたときにレイヤーを左にずらす
        if (event.pressed) {
          if (layer_state == 0) layer_state = 1;
          if (layer_state & (0b11 << L_IME_OFF)) {
            layer_state <<= L_IME_ON;
          }
          register_code(KC_HENKAN);
        } else {
          unregister_code(KC_HENKAN);
        }
        send_keyboard_report();
        break;
      }
      case FN_IME_OFF_KANA: {
        // IMEをオフにしたときにレイヤーを右にずらす
        if (event.pressed) {
          if (layer_state & (0b11 << L_IME_ON)) {
            layer_state >>= L_IME_ON;
          }
          register_code(KC_KATAKANA_HIRAGANA);
        } else {
          unregister_code(KC_KATAKANA_HIRAGANA);
        }
        send_keyboard_report();
        break;
      }
      case FN_MICROPHONE_MUTE:
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
}  // extern "C"
}  // namespace tmk_desktop

#ifdef _WIN32
#include <tmk_desktop/win32/settings.hpp>

namespace tmk_desktop::inline win32 {
using namespace jp109;

// 物理キーボードから仮想キーボードへの対応表を定義します。
const KeyToKeyposTable key_to_keypos_table = [] {
  KeyToKeyposTable t{};
  std::fill(t.begin(), t.end(), keypos_t{0xff, 0xff});
  for (size_t i = 0; auto key : KEYS) {
    t[key] = keypos_t{static_cast<uint8_t>(i % MATRIX_COLS), static_cast<uint8_t>(i / MATRIX_COLS)};
    i++;
  }
  return t;
}();
}  // namespace tmk_desktop::inline win32
#endif
