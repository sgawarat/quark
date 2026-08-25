// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.cpp
 * @brief Sink
 */
#include <atomic>
#include <condition_variable>
#include <deque>
#include <exception>
#include <mutex>
#include <thread>

#include <quark/bitset.hpp>
#include <quark/sink.hpp>

extern "C" {
#include <action.h>
}  // extern "C"

#ifdef _WIN32
#include "win32/sender.hpp"
#endif

namespace quark {
namespace {
std::thread thread_{};                     ///< スレッド
std::atomic<bool> running_{false};         ///< スレッドが動作中かどうか
std::atomic<bool> stop_requested_{false};  ///< スレッドに対する停止要求

std::deque<SinkEvent> event_queue_;       ///< イベントキュー
std::mutex event_queue_mtx_;              ///< イベントキューのためのMutex
std::condition_variable event_queue_cv_;  ///< イベントキューのためのCV
EventSender sender_;                      ///< OSに入力イベントを送るためのクラス

constexpr uint8_t to_keycode(const report_extra_t& report) noexcept {
  switch (report.report_id) {
    case REPORT_ID_SYSTEM:
      switch (report.usage) {
        case SYSTEM_POWER_DOWN: return KC_SYSTEM_POWER;
        case SYSTEM_SLEEP: return KC_SYSTEM_SLEEP;
        case SYSTEM_WAKE_UP: return KC_SYSTEM_WAKE;
        default: break;
      }
      break;
    case REPORT_ID_CONSUMER:
      switch (report.usage) {
        case AUDIO_MUTE: return KC_AUDIO_MUTE;
        case AUDIO_VOL_UP: return KC_AUDIO_VOL_UP;
        case AUDIO_VOL_DOWN: return KC_AUDIO_VOL_DOWN;
        case TRANSPORT_NEXT_TRACK: return KC_MEDIA_NEXT_TRACK;
        case TRANSPORT_PREV_TRACK: return KC_MEDIA_PREV_TRACK;
        case TRANSPORT_STOP: return KC_MEDIA_STOP;
        case TRANSPORT_STOP_EJECT: return KC_MEDIA_EJECT;
        case TRANSPORT_PLAY_PAUSE: return KC_MEDIA_PLAY_PAUSE;
        case AL_CC_CONFIG: return KC_MEDIA_SELECT;
        case AL_EMAIL: return KC_MAIL;
        case AL_CALCULATOR: return KC_CALCULATOR;
        case AL_LOCAL_BROWSER: return KC_MY_COMPUTER;
        case AC_SEARCH: return KC_WWW_SEARCH;
        case AC_HOME: return KC_WWW_HOME;
        case AC_BACK: return KC_WWW_BACK;
        case AC_FORWARD: return KC_WWW_FORWARD;
        case AC_STOP: return KC_WWW_STOP;
        case AC_REFRESH: return KC_WWW_REFRESH;
        case AC_BOOKMARKS: return KC_WWW_FAVORITES;
        case TRANSPORT_FAST_FORWARD: return KC_MEDIA_FAST_FORWARD;
        case TRANSPORT_REWIND: return KC_MEDIA_REWIND;
        case BRIGHTNESS_UP: return KC_BRIGHTNESS_UP;
        case BRIGHTNESS_DOWN: return KC_BRIGHTNESS_DOWN;
        case AL_CONTROL_PANEL: return KC_CONTROL_PANEL;
        case AL_ASSISTANT: return KC_ASSISTANT;
        default: break;
      }
      break;
    default: break;
  }
  return KC_NO;
}

/**
 * @brief SinkEventのvisitor
 */
class SinkEventVisitor final {
public:
  using Keyset = Bitset<256, uint8_t>;
  using ModKeyset = Bitset<8, uint8_t>;

  void operator()(const report_keyboard_t& report) noexcept {
    // 前回のキー状態を保存する
    const auto prev_keyset = keyset_;          // 前回のキー状態
    const auto prev_mod_keyset = mod_keyset_;  // 前回の修飾キー状態

    // 今回のキー状態を記録する
    keyset_.clear();
    for (auto key : std::span(report.keys)) {
      if (key >= KC_LEFT_CTRL && key <= KC_RIGHT_GUI) continue;
      if (key != KC_NO) keyset_.set(key);
    }
    mod_keyset_ = ModKeyset{report.mods};

    // 今回の更新で変化するキーを抽出する
    scan(prev_keyset, prev_mod_keyset);
  }

  void operator()(const report_nkro_t& report) noexcept {
    // 前回のキー状態を保存する
    const auto prev_keyset = keyset_;          // 前回のキー状態
    const auto prev_mod_keyset = mod_keyset_;  // 前回の修飾キー状態

    // 今回のキー状態を記録する
    keyset_ = Keyset{report.bits};         // 今回のキー状態
    mod_keyset_ = ModKeyset{report.mods};  // 今回の修飾キー状態

    // 今回の更新で変化するキーを抽出する
    scan(prev_keyset, prev_mod_keyset);
  }

  void operator()(const report_mouse_t& report) noexcept {
    // TODO: send_mouseを実装する
  }

  void operator()(const report_extra_t& report) noexcept {
    const auto keycode = to_keycode(report);
    if (keycode != KC_NO) sender_.send_key_tap(keycode);
  }

  void operator()(const NativeSinkEvent& event) noexcept {
    sender_.send_native_event(event);
  }

  void operator()(SinkSignal signal) noexcept {
    switch (signal) {
      case SinkSignal::KEY_REPEAT: sender_.send_key_repeat(); break;
      case SinkSignal::RESET:
        keyset_.clear();
        mod_keyset_.clear();
        sender_.reset();
        break;
      default: break;
    }
  }

private:
  void scan(const Keyset& prev_keyset, const ModKeyset& prev_mod_keyset) noexcept {
    // 今回の更新で変化するキーを抽出する
    const auto pressed_keyset = reset_to_set(prev_keyset, keyset_);               // 押した
    const auto released_keyset = set_to_reset(prev_keyset, keyset_);              // 離した
    const auto pressed_mod_keyset = reset_to_set(prev_mod_keyset, mod_keyset_);   // 押した
    const auto released_mod_keyset = set_to_reset(prev_mod_keyset, mod_keyset_);  // 離した

    // キーイベントを送信する
    released_keyset.scan([](auto pos) {  // 非修飾キーを離す
      sender_.send_key_release(static_cast<uint8_t>(pos.index()));
    });
    released_mod_keyset.scan([](auto pos) {  // 修飾キーを離す
      sender_.send_key_release(KC_LEFT_CTRL + static_cast<uint8_t>(pos.index()));
    });
    pressed_mod_keyset.scan([](auto pos) {  // 修飾キーを押す
      sender_.send_key_press(KC_LEFT_CTRL + static_cast<uint8_t>(pos.index()));
    });
    pressed_keyset.scan([](auto pos) {  // 非修飾キーを押す
      sender_.send_key_press(static_cast<uint8_t>(pos.index()));
    });
  }

  Keyset keyset_{};         ///< 最新のキー状態
  ModKeyset mod_keyset_{};  ///< 最新の修飾キー状態
} visitor_;
}  // namespace

bool start_sink() {
  if (thread_.joinable()) return false;

  // 状態を初期化する
  stop_requested_.store(false, std::memory_order_release);

  thread_ = std::thread([] {
    const struct ScopedRunning {
      ScopedRunning() {
        running_.store(true, std::memory_order_release);
      }
      ~ScopedRunning() {
        running_.store(false, std::memory_order_release);
      }
    } _running{};

    try {
      const struct ScopedInit {
        ScopedInit() {
          sender_.enable();
        }
        ~ScopedInit() {
          sender_.disable();
        }
      } _init{};

      while (!stop_requested_.load(std::memory_order_acquire)) {
        SinkEvent event;
        {
          std::unique_lock lock{event_queue_mtx_};
          if (event_queue_.empty()) {
            event_queue_cv_.wait(
                lock, [] { return !event_queue_.empty() || stop_requested_.load(std::memory_order_acquire); });
            if (stop_requested_.load(std::memory_order_acquire)) break;
            if (event_queue_.empty()) continue;
          }
          event = event_queue_.front();
          event_queue_.pop_front();
        }

        // イベントの中身に応じて処理を行う
        std::visit(visitor_, event);

        // CPUを明け渡す
        std::this_thread::yield();
      }
    } catch (const std::exception& e) {
      on_sink_error(e);
    }
  });

  return true;
}

bool stop_sink() {
  // すでにスレッドが停止しているかを確認する
  if (!thread_.joinable()) return false;

  // すでにスレッドが実行終了しているかを確認する
  if (!running_.load(std::memory_order_acquire)) return false;

  // スレッドに停止要求を出す
  stop_requested_.store(true, std::memory_order_release);
  event_queue_cv_.notify_one();

  // スレッドが停止するのを待つ
  thread_.join();

  return true;
}

void send_to_sink(const SinkEvent& event) {
  {
    std::lock_guard lock{event_queue_mtx_};
    event_queue_.push_back(event);
  }
  event_queue_cv_.notify_one();
}

SinkStatus get_sink_status() noexcept {
  if (running_.load(std::memory_order_acquire)) {
    if (stop_requested_.load(std::memory_order_acquire)) return SinkStatus::STOPPING;
    return SinkStatus::RUNNING;
  } else {
    if (thread_.joinable()) return SinkStatus::STOPPED;
    return SinkStatus::RESET;
  }
}
}  // namespace quark
