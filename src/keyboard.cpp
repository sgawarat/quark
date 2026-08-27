// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file keyboard.cpp
 * @brief Keyboard
 */
#include <quark/keyboard.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <exception>
#include <mutex>
#include <thread>

#include <quark/bitset.hpp>
#include <quark/key.hpp>
#include <quark/sink.hpp>
#include <quark/tick.hpp>

extern "C" {
#include <action.h>
#include <action_layer.h>
#include <host.h>
#include <keyboard.h>
#include <matrix.h>
#include <report.h>

extern matrix_row_t raw_matrix[MATRIX_ROWS];
}  // extern "C"

namespace quark {
namespace {
using TickClock = std::chrono::steady_clock;

constexpr auto FAST_TICK_INTERVAL =
    std::chrono::duration_cast<TickClock::duration>(std::chrono::seconds(1)) / QUARK_TICK_FAST_RATE;
constexpr auto SLOW_TICK_INTERVAL =
    std::chrono::duration_cast<TickClock::duration>(std::chrono::seconds(1)) / QUARK_TICK_SLOW_RATE;

std::thread thread_{};                     ///< スレッド
std::atomic<bool> running_{false};         ///< スレッドが動作中かどうか
std::atomic<bool> stop_requested_{false};  ///< スレッドに対する停止要求
std::promise<void> promise_{};             ///< スレッドからの戻り値

std::deque<KeyboardEvent> event_queue_;         ///< イベントキュー
std::mutex event_queue_mtx_;                    ///< イベントキューのためのMutex
std::condition_variable event_queue_cv_;        ///< イベントキューのためのCV
TickClock::time_point next_tick_tp_{};          ///< 次のtickの開始時間
TickClock::time_point fast_tick_timeout_tp_{};  ///< 高速モードの終了時間
Key last_key_ = Key{KEY_COUNT};                 ///< 最後に押したキー

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"

inline void matrix_set(keypos_t keypos) noexcept {
  if (keypos.col >= MATRIX_COLS || keypos.row >= MATRIX_ROWS) return;
  raw_matrix[keypos.row] |= static_cast<matrix_row_t>(1) << keypos.col;
}

inline void matrix_reset(keypos_t keypos) noexcept {
  if (keypos.col >= MATRIX_COLS || keypos.row >= MATRIX_ROWS) return;
  raw_matrix[keypos.row] &= ~(static_cast<matrix_row_t>(1) << keypos.col);
}

inline void matrix_clear() noexcept {
  for (size_t i = 0; i < MATRIX_ROWS; ++i) {
    raw_matrix[i] = 0;
  }
}

#pragma clang diagnostic pop

/**
 * @brief KeyboardEventのvisitor
 */
class KeyboardVisitor final {
public:
  void operator()(std::monostate) noexcept {
    // tick eventを処理する
    keyboard_task();
  }

  void operator()(const KeyEvent& event) noexcept {
    const auto key = event.key();
    const auto keypos = get_key_property<keypos_t>(key);
    if (keypos.row < MATRIX_ROWS && keypos.col < MATRIX_COLS) {
      if (event.is_pressed()) {
        if (key == last_key_) {
          // 直前と同じキーが押されたら、キーリピートとして扱う
          // キーリピートはOSの仕様の範疇なので、ハードウェアレベルのQMKには送らない
          send_to_sink(SinkSignal::KEY_REPEAT);
        } else {
          // 直前と違うキーが押されたら、実際のキー入力として扱う
          matrix_set(keypos);
          keyboard_task();
          last_key_ = key;

          // キーがNonHoldableなら、すぐに離す処理を行う
          if (get_key_property<NonHoldable>(key)) {
            matrix_reset(keypos);
            keyboard_task();
          }
        }
      } else {
        // 直前と同じキーが離されたら、キーリピートを終了する
        if (key == last_key_) last_key_ = Key{KEY_COUNT};
        matrix_reset(keypos);
        keyboard_task();
      }
    }
  }

  void operator()(KeyboardSignal signal) noexcept {
    switch (signal) {
      // すべてのキーを離した扱いにする
      case KeyboardSignal::CLEAR: {
        matrix_clear();
        keyboard_task();
        break;
      }

      // 内部状態を初期値に戻す
      case KeyboardSignal::RESET: {
        matrix_clear();
        clear_keyboard();
        layer_clear();
        last_key_ = Key{KEY_COUNT};
        send_to_sink(SinkSignal::RESET);
        break;
      }
      default: break;
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

std::future<void> start_keyboard() {
  // スレッドがすでに動作中であれば何もしない
  if (running_.load(std::memory_order_acquire)) return {};

  // スレッドを完全に停止させる
  if (thread_.joinable()) thread_.join();

  // 状態を初期化する
  stop_requested_.store(false, std::memory_order_release);
  promise_ = {};

  // 新しくスレッドを生成する
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
              keyboard_leds,
              send_keyboard,
              send_nkro,
              send_mouse,
              send_extra,
          };
          host_set_driver(&driver);
          keyboard_setup();
          keyboard_init();
        }
        ~ScopedInit() {
          matrix_clear();
          clear_keyboard();
          host_set_driver(nullptr);
        }
      } _init{};

      while (true) {
        KeyboardEvent event;
        {
          std::unique_lock lock{event_queue_mtx_};

          // 確認
          if (event_queue_.empty()) {
            // 既存分を処理し終えていたら、停止要求に答える
            if (stop_requested_.load(std::memory_order_acquire)) break;

            // 次のtickまでイベントを待つ
            event_queue_cv_.wait_until(lock, next_tick_tp_, [] {
              return !event_queue_.empty() || stop_requested_.load(std::memory_order_acquire);
            });
          }

          // 本番
          if (event_queue_.empty()) {
            // イベントが来ぬまま停止要求が来たら、それに答える
            if (stop_requested_.load(std::memory_order_acquire)) break;

            // イベントがないので、次のtickに備える
            const auto now = TickClock::now();
            if (now < fast_tick_timeout_tp_) {
              next_tick_tp_ = now + FAST_TICK_INTERVAL;
            } else {
              next_tick_tp_ = now + SLOW_TICK_INTERVAL;
            }
          } else {
            // イベントが来たので、それを取得する
            event = event_queue_.front();
            event_queue_.pop_front();

            // イベントが来たので、高速モードに戻す
            fast_tick_timeout_tp_ = TickClock::now() + QUARK_TICK_FAST_TIME;
          }
        }

        // イベントの中身に応じて処理を行う
        std::visit(visitor_, event);

        // CPUを明け渡す
        std::this_thread::yield();
      }

      promise_.set_value();
    } catch (...) {
      promise_.set_exception(std::current_exception());
      on_keyboard_error();
    }
  });

  return promise_.get_future();
}

void stop_keyboard() {
  // すでにスレッドが完全に停止していれば何もしない
  if (!thread_.joinable()) return;

  // スレッドが動作中であれば、停止を要求する
  if (running_.load(std::memory_order_acquire)) {
    stop_requested_.store(true, std::memory_order_release);
    event_queue_cv_.notify_one();
  }

  // スレッドが完全に停止するのを待つ
  thread_.join();
}

void send_to_keyboard(const KeyboardEvent& event) {
  {
    std::lock_guard lock{event_queue_mtx_};
    event_queue_.emplace_back(event);
  }
  event_queue_cv_.notify_one();
}

KeyboardStatus get_keyboard_status() noexcept {
  if (!running_.load(std::memory_order_acquire)) {
    return KeyboardStatus::STOPPED;
  }
  if (stop_requested_.load(std::memory_order_acquire)) {
    return KeyboardStatus::STOP_REQUESTED;
  }
  return KeyboardStatus::RUNNING;
}
}  // namespace quark
