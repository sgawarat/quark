// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file example.cpp
 * @brief JIS配列
 */
#include "../../jis.hpp"

namespace quark::jis {
namespace {
constexpr keypos_t make_extra_keypos(uint8_t col, uint8_t extra_row) noexcept {
  return {col, static_cast<uint8_t>(MATRIX_ROWS - QUARK_KEYBOARD_JIS_EXTRA_ROWS + extra_row)};
}
}  // namespace

// 追加の1行にマルチメディアキーを並べる
std::optional<keypos_t> get_extra_key_property(Key key, KeyPropertyTag<keypos_t>) {
  switch (key) {
    case K_AUDIO_MUTE: return make_extra_keypos(0, 0);
    case K_AUDIO_VOL_DOWN: return make_extra_keypos(1, 0);
    case K_AUDIO_VOL_UP: return make_extra_keypos(2, 0);
    case K_MEDIA_PREV_TRACK: return make_extra_keypos(3, 0);
    case K_MEDIA_PLAY_PAUSE: return make_extra_keypos(4, 0);
    case K_MEDIA_NEXT_TRACK: return make_extra_keypos(5, 0);
    default: break;
  }
  return std::nullopt;
}
}  // namespace quark::jis
