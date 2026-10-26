// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file example.cpp
 * @brief JIS配列
 */
#include <cstdint>
#include <optional>

#include "../../jis.hpp"
#include "quark/quantum/key.hpp"

namespace quark::jis {
// 追加の1行にマルチメディアキーを並べる
std::optional<keypos_t> get_extra_key_property(KeyCode keycode, KeyPropertyTag<keypos_t>) {
  const auto row = static_cast<uint8_t>(MATRIX_ROWS - QUARK_KEYBOARD_JIS_EXTRA_ROWS);
  switch (+keycode) {
    case KC_AUDIO_MUTE: return keypos_t{.col = 0, .row = row};
    case KC_AUDIO_VOL_DOWN: return keypos_t{.col = 1, .row = row};
    case KC_AUDIO_VOL_UP: return keypos_t{.col = 2, .row = row};
    case KC_MEDIA_PREV_TRACK: return keypos_t{.col = 3, .row = row};
    case KC_MEDIA_PLAY_PAUSE: return keypos_t{.col = 4, .row = row};
    case KC_MEDIA_NEXT_TRACK: return keypos_t{.col = 5, .row = row};  // NOLINT
  }
  return std::nullopt;
}
}  // namespace quark::jis
