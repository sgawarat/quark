// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyboard.cpp
 * @brief Keyboard処理
 */
#include "quark/keyboard.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <stdexcept>
#include <stop_token>
#include <utility>

#include "quark/event.hpp"
#include "quark/keyset.hpp"
#include "quark/logging.hpp"
#include "quark/quantum/key.hpp"
#include "quark/quantum/report.hpp"
#include "quark/queue.hpp"

namespace quark {
namespace {
/**
 * @brief エクストラキーに対応するキーコードを取得する
 */
constexpr KeyCode to_keycode(const report_extra_t& report) noexcept {
  switch (report.report_id) {
    case REPORT_ID_SYSTEM: {
      switch (report.usage) {
        case SYSTEM_POWER_DOWN: return KC_SYSTEM_POWER;
        case SYSTEM_SLEEP: return KC_SYSTEM_SLEEP;
        case SYSTEM_WAKE_UP: return KC_SYSTEM_WAKE;
      }
      return KC_NO;
    }
    case REPORT_ID_CONSUMER: {
      switch (report.usage) {
        // case SNAPSHOT: return KC_PRINT;
        case BRIGHTNESS_UP: return KC_BRIGHTNESS_UP;
        case BRIGHTNESS_DOWN: return KC_BRIGHTNESS_DOWN;
        // case TRANSPORT_RECORD: return KC_RECORD;
        case TRANSPORT_FAST_FORWARD: return KC_MEDIA_FAST_FORWARD;
        case TRANSPORT_REWIND: return KC_MEDIA_REWIND;
        case TRANSPORT_NEXT_TRACK: return KC_MEDIA_NEXT_TRACK;
        case TRANSPORT_PREV_TRACK: return KC_MEDIA_PREV_TRACK;
        case TRANSPORT_STOP: return KC_MEDIA_STOP;
        case TRANSPORT_EJECT: return KC_MEDIA_EJECT;
        // case TRANSPORT_RANDOM_PLAY: return KC_;
        // case TRANSPORT_STOP_EJECT: return KC_;
        case TRANSPORT_PLAY_PAUSE: return KC_MEDIA_PLAY_PAUSE;
        case AUDIO_MUTE: return KC_AUDIO_MUTE;
        case AUDIO_VOL_UP: return KC_AUDIO_VOL_UP;
        case AUDIO_VOL_DOWN: return KC_AUDIO_VOL_DOWN;
        case AL_CC_CONFIG: return KC_MEDIA_SELECT;
        case AL_EMAIL: return KC_MAIL;
        case AL_CALCULATOR: return KC_CALCULATOR;
        case AL_LOCAL_BROWSER: return KC_MY_COMPUTER;
        // case AL_LOCK: return KC_COFFEE;
        case AL_CONTROL_PANEL: return KC_CONTROL_PANEL;
        case AL_ASSISTANT: return KC_ASSISTANT;
        // case AL_KEYBOARD_LAYOUT: return KC_;
        // case AC_NEW: return KC_NEW;
        // case AC_OPEN: return KC_OPEN;
        // case AC_CLOSE: return KC_CLOSE;
        // case AC_EXIT: return KC_EXIT;
        // case AC_MAXIMIZE: return KC_;
        // case AC_MINIMIZE: return KC_;
        // case AC_SAVE: return KC_SAVE;
        // case AC_PRINT: return KC_PRINT;
        // case AC_PROPERTIES: return KC_PROPS;
        case AC_UNDO: return KC_UNDO;
        case AC_COPY: return KC_COPY;
        case AC_CUT: return KC_CUT;
        case AC_PASTE: return KC_PASTE;
        // case AC_SELECT_ALL: return KC_;
        case AC_FIND: return KC_FIND;
        case AC_SEARCH: return KC_WWW_SEARCH;
        case AC_HOME: return KC_WWW_HOME;
        case AC_BACK: return KC_WWW_BACK;
        case AC_FORWARD: return KC_WWW_FORWARD;
        case AC_STOP: return KC_WWW_STOP;
        case AC_REFRESH: return KC_WWW_REFRESH;
        case AC_BOOKMARKS: return KC_WWW_FAVORITES;
        // case AC_NEXT_KEYBOARD_LAYOUT_SELECT: return KC_;
        case AC_DESKTOP_SHOW_ALL_WINDOWS: return KC_MISSION_CONTROL;
        case AC_SOFT_KEY_LEFT: return KC_LAUNCHPAD;
      }
      return KC_NO;
    }
  }
  return KC_NO;
}
}  // namespace

KeyboardContext::KeyboardContext(
    std::shared_ptr<KeyboardEventQueue> consumer, std::shared_ptr<SinkEventQueue> producer)
  : consumer_{std::move(consumer)}, producer_{std::move(producer)} {
  if (!consumer_) throw std::invalid_argument("Invalid KeyboardEventQueue");
  if (!producer_) throw std::invalid_argument("Invalid SinkEventQueue");
}

KeyboardContext::~KeyboardContext() noexcept = default;

void KeyboardContext::run(const std::stop_token& stop_token) {
  std::optional<KeyboardEventPacket> packet_opt;
  while (!stop_token.stop_requested()) {
    if (tick_period_tp_ < tick_timeout_tp_) {
      // 次のtickがアクティブ期間内なので、指定の時間で制御を戻させる
      packet_opt = consumer_->pop(stop_token, tick_period_tp_);
    } else {
      // 次のtickがアクティブ期間外なので、イベントが来るまで待機させる
      packet_opt = consumer_->pop(stop_token);
    }
    if (packet_opt) {
      packet_opt->visit([this](const auto& time, const auto& event) { update(time, event); });
    }
  }
}

void KeyboardContext::update(EventTime, TickEvent) {
  keyboard_task();

  // 次のtickの時刻を計算する
  tick_period_tp_ = EventClock::now() + TICK_INTERVAL;
}

void KeyboardContext::update(EventTime, KeyPressEvent event) {
  matrix_set(event.keycode);

  // OSのキー表現経由ではBACKSLASHとNONUS_HASHを区別するのが難しいので、BACKSLASHはNONUS_HASHとしても扱う
  // （BACKSLASHとNONUS_HASHを両方使いたいケースは相当特殊なはずなので、実用上はこれで問題ないと思います）
  // TODO: BACKSLASHとNONUS_HASHを個別に扱うためのオプションを作る？
  if (event.keycode == KC_BACKSLASH) matrix_set(KC_NONUS_HASH);

  // 新しいキーイベントが来たので、アクティブ時間をリセットする
  tick_timeout_tp_ = EventClock::now() + TICK_ACTIVE_TIME;
}

void KeyboardContext::update(EventTime, KeyReleaseEvent event) {
  matrix_reset(event.keycode);

  // OSのキー表現経由ではBACKSLASHとNONUS_HASHを区別するのが難しいので、BACKSLASHはNONUS_HASHとしても扱う
  if (event.keycode == KC_BACKSLASH) matrix_reset(KC_NONUS_HASH);

  // 新しいキーイベントが来たので、アクティブ時間をリセットする
  tick_timeout_tp_ = EventClock::now() + TICK_ACTIVE_TIME;
}

void KeyboardContext::update(EventTime, EngineSignal signal) {
  switch (signal) {
    case EngineSignal::SCREEN_LOCKED: {
      QUARK_LOG_DEBUG("SCREEN_LOCKED");
      break;
    }
    case EngineSignal::SCREEN_UNLOCKED: {
      QUARK_LOG_DEBUG("SCREEN_UNLOCKED");
      matrix_clear();

      // 新しいキーイベントが来たと同じことなので、アクティブ時間をリセットする
      tick_timeout_tp_ = EventClock::now() + TICK_ACTIVE_TIME;
      break;
    }
  }
}

void KeyboardContext::send_report(const report_keyboard_t& report) {
  send_report(KeySet{report});
}

void KeyboardContext::send_report(const report_nkro_t& report) {
  send_report(KeySet{report});
}

void KeyboardContext::send_report(const report_mouse_t& report) {
  const MouseButtonSet buttons{report};

  const MouseButtonSet released_buttons{releasing(prev_mouse_buttons_, buttons)};
  if (released_buttons.any()) producer_->emplace<MouseReleaseEvent>(released_buttons);

  const MouseButtonSet pressed_buttons{pressing(prev_mouse_buttons_, buttons)};
  if (pressed_buttons.any()) producer_->emplace<MousePressEvent>(pressed_buttons);

  if (report.v != 0 || report.h != 0) producer_->emplace<MouseWheelEvent>(report.v, report.h);
  if (report.x != 0 || report.y != 0) producer_->emplace<MouseCursorEvent>(report.x, report.y);

  prev_mouse_buttons_ = buttons;
}

void KeyboardContext::send_report(const report_extra_t& report) {
  // TODO: EXTRAKEYが同時押しできないのか調査する
  if (last_extra_key_ != KC_NO) {
    producer_->emplace<KeyReleaseEvent>(last_extra_key_);
  }
  last_extra_key_ = to_keycode(report);
  if (last_extra_key_ != KC_NO) {
    producer_->emplace<KeyPressEvent>(last_extra_key_);
  }
}

void KeyboardContext::send_report(const KeySet& keys) {
  const KeySet released_keys = releasing(prev_keys_, keys);
  const KeySet pressed_keys = pressing(prev_keys_, keys);

  released_keys.scan_keys([this](KeyCode keycode) {  // 非修飾キーを離す
    QUARK_LOG_TRACE("release {}", +keycode);
    producer_->emplace<KeyReleaseEvent>(keycode);
  });
  released_keys.scan_mods([this](KeyCode keycode) {  // 修飾キーを離す
    QUARK_LOG_TRACE("release {}", +keycode);
    producer_->emplace<KeyReleaseEvent>(keycode);
  });
  pressed_keys.scan_mods([this](KeyCode keycode) {  // 修飾キーを押す
    QUARK_LOG_TRACE("press {}", +keycode);
    producer_->emplace<KeyPressEvent>(keycode);
  });
  pressed_keys.scan_keys([this](KeyCode keycode) {  // 非修飾キーを押す
    QUARK_LOG_TRACE("press {}", +keycode);
    producer_->emplace<KeyPressEvent>(keycode);
  });

  prev_keys_ = keys;
}
}  // namespace quark
