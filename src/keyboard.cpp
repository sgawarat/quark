// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyboard.cpp
 * @brief Keyboard
 */
#include <tmk_desktop/keyboard.hpp>
#include <atomic>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <thread>
#include <tmk_desktop/bitset.hpp>
#include <tmk_desktop/sink.hpp>

extern "C" {
#include <keyboard.h>
#include <action.h>
#include <action_layer.h>
#include <matrix.h>
#include <host.h>
#include <report.h>

extern matrix_row_t raw_matrix[MATRIX_ROWS];
}  // extern "C"

namespace tmk_desktop {
// 変換表にアクセスする関数
extern keypos_t key_to_keypos(Key key) noexcept;
extern bool is_tapping_key(Key key) noexcept;

namespace {
std::thread thread_{};                     ///< スレッド
std::atomic<bool> running_{false};         ///< スレッドが動作中かどうか
std::atomic<bool> stop_requested_{false};  ///< スレッドに対する停止要求

std::deque<KeyboardEvent> event_queue_;   ///< イベントキュー
std::mutex event_queue_mtx_;              ///< イベントキューのためのMutex
std::condition_variable event_queue_cv_;  ///< イベントキューのためのCV

static constexpr Key NO_REPEAT = Key{KEY_COUNT};  ///< キーリピートしていないことを示す値

Key repeat_key_ = NO_REPEAT;  ///< リピートしているキー

inline void matrix_set(keypos_t keypos) noexcept {
  raw_matrix[keypos.row] |= static_cast<matrix_row_t>(1) << keypos.col;
}

inline void matrix_reset(keypos_t keypos) noexcept {
  raw_matrix[keypos.row] &= ~(static_cast<matrix_row_t>(1) << keypos.col);
}

inline void matrix_clear() noexcept {
  for (size_t i = 0; i < MATRIX_ROWS; ++i) {
    raw_matrix[i] = 0;
  }
}

/**
 * @brief KeyboardEventのvisitor
 */
class KeyboardVisitor final {
public:
  void operator()(const KeyEvent& event) noexcept {
    const auto key = event.key();
    const auto keypos = key_to_keypos(key);
    if (keypos.row < MATRIX_ROWS && keypos.col < MATRIX_COLS) {
      if (event.is_pressed()) {
        if (key == repeat_key_) {
          send_to_sink(SinkSignal::KEY_REPEAT);
        } else {
          send_to_sink(SinkSignal::KEY_REPEAT_END);
          repeat_key_ = key;
          matrix_set(keypos);
          keyboard_task();

          // 指定のキーはすぐに離す処理を行う
          if (is_tapping_key(key)) {
            send_to_sink(SinkSignal::KEY_REPEAT_END);
            repeat_key_ = NO_REPEAT;
            matrix_reset(keypos);
            keyboard_task();
          }
        }
      } else {
        if (key == repeat_key_) {
          send_to_sink(SinkSignal::KEY_REPEAT_END);
          repeat_key_ = NO_REPEAT;
        }
        matrix_reset(keypos);
        keyboard_task();
      }
    }
  }

  void operator()(KeyboardSignal signal) noexcept {
    switch (signal) {
      case KeyboardSignal::CLEAR: {
        if (repeat_key_ != NO_REPEAT) {
          send_to_sink(SinkSignal::KEY_REPEAT_END);
          repeat_key_ = NO_REPEAT;
        }
        matrix_clear();
        clear_keyboard();
        break;
      }
      case KeyboardSignal::RESET: {
        if (repeat_key_ != NO_REPEAT) {
          send_to_sink(SinkSignal::KEY_REPEAT_END);
          repeat_key_ = NO_REPEAT;
        }
        matrix_clear();
        clear_keyboard();
        layer_clear();
        send_to_sink(SinkSignal::RESET);
        break;
      }
    }
  }

} visitor_;

// Sinkにイベントを送信するホストドライバ関数たち
uint8_t keyboard_leds() noexcept {
  return 0;
}
void send_keyboard(report_keyboard_t* report_ptr) noexcept {
  if (report_ptr) send_to_sink(*report_ptr);
}
void send_nkro(report_nkro_t* report_ptr) noexcept {
  if (report_ptr) send_to_sink(*report_ptr);
}
void send_mouse(report_mouse_t* report_ptr) noexcept {
  if (report_ptr) send_to_sink(*report_ptr);
}
void send_extra(report_extra_t* report_ptr) noexcept {
  if (report_ptr) send_to_sink(*report_ptr);
}
}  // namespace

bool start_keyboard() {
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
          static host_driver_t driver{
              keyboard_leds, send_keyboard, send_nkro, send_mouse, send_extra,
          };
          host_set_driver(&driver);
          keyboard_setup();
          keyboard_init();
        }
        ~ScopedInit() {
          clear_keyboard();
          host_set_driver(nullptr);
        }
      } _init{};

      while (!stop_requested_.load(std::memory_order_acquire)) {
        KeyboardEvent event;
        {
          std::unique_lock lock{event_queue_mtx_};
          if (event_queue_.empty()) {
            event_queue_cv_.wait(lock, [] { return !event_queue_.empty() || stop_requested_.load(std::memory_order_acquire); });
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
    } catch (std::exception& e) {
      on_keyboard_error(e);
    }
  });

  return true;
}

bool stop_keyboard() {
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

void send_to_keyboard(const KeyboardEvent& event) {
  {
    std::lock_guard lock{event_queue_mtx_};
    event_queue_.emplace_back(event);
  }
  event_queue_cv_.notify_one();
}

KeyboardStatus get_keyboard_status() noexcept {
  if (running_.load(std::memory_order_acquire)) {
    if (stop_requested_.load(std::memory_order_acquire)) return KeyboardStatus::STOPPING;
    return KeyboardStatus::RUNNING;
  } else {
    if (thread_.joinable()) return KeyboardStatus::STOPPED;
    return KeyboardStatus::RESET;
  }
}
}  // namespace tmk_desktop
