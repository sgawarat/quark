// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file engine.cpp
 * @brief キーボードエンジン
 */
#include <quark/engine.hpp>

#include <atomic>
#include <chrono>
#include <stdexcept>

#include <quark/key.hpp>
#include <quark/logging.hpp>
#include <quark/utility.hpp>

#ifdef _WIN32
#include <quark/win32/sink.hpp>
#include <quark/win32/source.hpp>
#endif

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
/**
 * @brief キーボード処理を行うためのクラス
 */
class KeyboardContext final : private Stationary {
public:
  explicit KeyboardContext(SinkQueue& sink_queue, SourceQueue& source_queue)
    : sink_queue_{sink_queue},
      source_queue_{source_queue},
      driver_{
        .keyboard_leds = []() -> uint8_t { return 0; },
        .send_keyboard =
            [](report_keyboard_t* report_ptr) noexcept {
              auto* owner = owner_.load(std::memory_order_acquire);
              if (owner && report_ptr) owner->send_to_sink(*report_ptr);
            },
        .send_nkro =
            [](report_nkro_t* report_ptr) noexcept {
              auto* owner = owner_.load(std::memory_order_acquire);
              if (owner && report_ptr) owner->send_to_sink(*report_ptr);
            },
        .send_mouse =
            [](report_mouse_t* report_ptr) noexcept {
              auto* owner = owner_.load(std::memory_order_acquire);
              if (owner && report_ptr) owner->send_to_sink(*report_ptr);
            },
        .send_extra =
            [](report_extra_t* report_ptr) noexcept {
              auto* owner = owner_.load(std::memory_order_acquire);
              if (owner && report_ptr) owner->send_to_sink(*report_ptr);
            },
      } {
    // QMKの使用権を借用する
    KeyboardContext* expected{nullptr};
    if (!owner_.compare_exchange_strong(expected, this)) {
      throw std::logic_error("QMK is already in use");
    }

    // QMKを初期化する
    host_set_driver(&driver_);
    keyboard_setup();
    keyboard_init();
    try_rethrow_exception();
  }

  ~KeyboardContext() noexcept {
    // QMKの使用権を持っていなければ何もしない
    if (owner_.load(std::memory_order_relaxed) != this) return;

    // QMKをクリアする
    matrix_clear();
    clear_keyboard();
    try {
      try_rethrow_exception();
    } catch (const std::exception& e) {
      warn_log("Failed to clean the QMK up ({})", e.what());
    }

    // QMKの使用権を返却する
    host_set_driver(nullptr);
    owner_.store(nullptr, std::memory_order_release);
  }

  /**
   * @brief ループ処理を行う
   */
  void run(const std::stop_token& stop_token) {
    SourceEvent event;
    while (source_queue_.pop(stop_token, tick_period_tp_, event)) {
      std::visit([this](const auto& event) { update(event); }, event);
      std::this_thread::yield();
    }
  }

  /**
   * @brief イベントをSinkへ送る
   *
   * QMKのホストドライバ関数で呼び出される。
   */
  void send_to_sink(const SinkEvent& event) noexcept {
    try {
      sink_queue_.push(event);
    } catch (...) {
      exception_in_qmk_ = std::current_exception();
    }
  }

  /**
   * @brief QMK処理中に投げられた例外があればrethrowする
   */
  void try_rethrow_exception() {
    if (exception_in_qmk_) std::rethrow_exception(exception_in_qmk_);
  }

private:
  static inline std::atomic<KeyboardContext*> owner_;  ///< QMKの占有者

  /**
   * @brief 次のtickまでの時間間隔
   */
  static constexpr auto TICK_INTERVAL =
      std::chrono::duration_cast<SourceClock::duration>(std::chrono::seconds(1)) /
      QUARK_TICK_ACTIVE_RATE;

  /**
   + tick eventを処理する
   */
  void update(std::monostate) {
    keyboard_task();
    try_rethrow_exception();

    // 次のtickの時刻を計算する
    const auto now = SourceClock::now();
    if (now < tick_timeout_tp_) {
      tick_period_tp_ = now + TICK_INTERVAL;
    } else {
      // ある程度の期間にキーイベントがなかったら、tickをさせないようにする
      tick_period_tp_ = SourceClock::time_point::max();
    }
  }

  void update(const SourceKeyEvent& event) {
    const auto key = event.key();
    const auto keypos = get_key_property<keypos_t>(key);
    if (keypos.row < MATRIX_ROWS && keypos.col < MATRIX_COLS) {
      if (event.is_pressed()) {
        if (key == last_key_) {
          // 直前と同じキーが押されたら、キーリピートとして扱う
          // TODO: DebounceなどのQMKの内部状態を考慮したキーリピート実装を行う
          sink_queue_.push(SinkSignal::KEY_REPEAT);
        } else {
          // 直前と違うキーが押されたら、実際のキー入力として扱う
          matrix_set(keypos);
          keyboard_task();
          try_rethrow_exception();
          last_key_ = key;

          // キーがNonHoldableなら、すぐに離す処理を行う
          if (get_key_property<NonHoldable>(key) == NonHoldable::YES) {
            last_key_ = Key{KEY_COUNT};
            matrix_reset(keypos);
            keyboard_task();
            try_rethrow_exception();
          }

          // 新しいキーイベントが来たので、tickをアクティブにする
          const auto now = SourceClock::now();
          tick_period_tp_ = now + TICK_INTERVAL;
          tick_timeout_tp_ = now + QUARK_TICK_ACTIVE_TIME;
        }
      } else {
        // 直前と同じキーが離されたら、キーリピートを終了する
        if (key == last_key_) last_key_ = Key{KEY_COUNT};
        matrix_reset(keypos);
        keyboard_task();
        try_rethrow_exception();

        // 新しいキーイベントが来たので、tickをアクティブにする
        const auto now = SourceClock::now();
        tick_period_tp_ = now + TICK_INTERVAL;
        tick_timeout_tp_ = now + QUARK_TICK_ACTIVE_TIME;
      }
    }
  }

  void update(SourceSignal signal) noexcept {
    switch (signal) {
      // すべてのキーを離した扱いにする
      case SourceSignal::CLEAR: {
        matrix_clear();
        keyboard_task();
        try_rethrow_exception();
        break;
      }

      // 内部状態を初期値に戻す
      case SourceSignal::RESET: {
        matrix_clear();
        clear_keyboard();
        try_rethrow_exception();
        layer_clear();
        last_key_ = Key{KEY_COUNT};
        sink_queue_.push(SinkSignal::RESET);
        break;
      }
    }
  }

  void matrix_set(keypos_t keypos) const {
    if (keypos.col >= MATRIX_COLS || keypos.row >= MATRIX_ROWS) return;
    if (keypos.row < raw_matrix_.size()) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
      raw_matrix_[keypos.row] |= static_cast<matrix_row_t>(1) << keypos.col;
    }
  }

  void matrix_reset(keypos_t keypos) const noexcept {
    if (keypos.col >= MATRIX_COLS || keypos.row >= MATRIX_ROWS) return;
    if (keypos.row < raw_matrix_.size()) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
      raw_matrix_[keypos.row] &= ~(static_cast<matrix_row_t>(1) << keypos.col);
    }
  }

  void matrix_clear() const noexcept {
    for (auto& row : raw_matrix_) {
      row = 0;
    }
  }

  SinkQueue& sink_queue_;
  SourceQueue& source_queue_;
  host_driver_t driver_{};
  Key last_key_ = Key{KEY_COUNT};
  SourceClock::time_point tick_period_tp_{SourceClock::now()};   ///< 次のtickの開始時間
  SourceClock::time_point tick_timeout_tp_{SourceClock::now()};  ///< アクティブモードの終了時間
  std::exception_ptr exception_in_qmk_;
  const std::span<matrix_row_t, MATRIX_ROWS> raw_matrix_{raw_matrix};
};
}  // namespace

class Engine::State : private Stationary {
public:
  State() = default;

  ~State() noexcept = default;

  void push(SourceSignal signal) {
    source_queue_.push(signal);
  }

  [[nodiscard]] SinkContext get_sink_context() {
    return SinkContext{sink_queue_};
  }

  [[nodiscard]] KeyboardContext get_keyboard_context() {
    return KeyboardContext{sink_queue_, source_queue_};
  }

  [[nodiscard]] SourceContext get_source_context() {
    return SourceContext{source_queue_};
  }

private:
  SinkQueue sink_queue_;
  SourceQueue source_queue_;
};

Engine::Engine() = default;

Engine::~Engine() noexcept = default;

void Engine::init() {
  state_ = std::make_shared<State>();
}

void Engine::start(void (*on_error)(void*) noexcept, void* data) {
  if (!state_) throw std::logic_error("Engine is invalid");

  auto state = state_;
  sink_thread_ = std::jthread{[=](const std::stop_token& stop_token) noexcept {
    try {
      auto ctx = state->get_sink_context();
      ctx.run(stop_token);
      info_log("Sink thread exited successfully");
    } catch (const std::exception& e) {
      critical_log("Sink thread crashed ({})", e.what());
      if (on_error) on_error(data);
    }
  }};

  keyboard_thread_ = std::jthread{[=](const std::stop_token& stop_token) noexcept {
    try {
      auto ctx = state->get_keyboard_context();
      ctx.run(stop_token);
      info_log("Keyboard thread exited successfully");
    } catch (const std::exception& e) {
      critical_log("Keyboard thread crashed ({})", e.what());
      if (on_error) on_error(data);
    }
  }};

  source_thread_ = std::jthread{[=](const std::stop_token& stop_token) noexcept {
    try {
      auto ctx = state->get_source_context();
      ctx.run(stop_token);
      info_log("Source thread exited successfully");
    } catch (const std::exception& e) {
      critical_log("Source thread crashed ({})", e.what());
      if (on_error) on_error(data);
    }
  }};
}

void Engine::stop() noexcept {
  source_thread_ = {};
  keyboard_thread_ = {};
  sink_thread_ = {};
}

void Engine::send(EngineSignal signal) {
  if (!state_) throw std::logic_error("Engine is invalid");
  switch (signal) {
    case EngineSignal::CLEAR: {
      state_->push(SourceSignal::CLEAR);
      break;
    }
    case EngineSignal::RESET: {
      state_->push(SourceSignal::RESET);
      break;
    }
  }
}
}  // namespace quark
