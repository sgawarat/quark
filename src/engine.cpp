// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file engine.cpp
 * @brief キーボードエンジン
 */
#include "quark/engine.hpp"

#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <stop_token>

#include "quark/keyboard.hpp"
#include "quark/logging.hpp"
#include "quark/utility.hpp"

#ifdef _WIN32
#include "quark/win32/sink.hpp"
#include "quark/win32/source.hpp"
#elifdef __linux__
#include "quark/linux/sink.hpp"
#include "quark/linux/source.hpp"
#endif

namespace quark {
class Engine::State : public std::enable_shared_from_this<Engine::State>, private Pinned {
public:
  explicit State(std::function<void()> on_error) : on_error_{std::move(on_error)} {}

  ~State() noexcept = default;

  void reset() {
    source_queue_.reset();
    keyboard_queue_.reset();
    sink_queue_.reset();
  }

  void push(EngineSignal signal) {
    source_queue_.push(signal);
    keyboard_queue_.push(signal);
    sink_queue_.push(signal);
  }

  [[nodiscard]] SourceContext get_source_context() {
    return SourceContext{
      {shared_from_this(), &keyboard_queue_}, {shared_from_this(), &source_queue_}, {}};
  }

  [[nodiscard]] KeyboardContext get_keyboard_context() {
    return KeyboardContext{
      {shared_from_this(), &keyboard_queue_}, {shared_from_this(), &sink_queue_}};
  }

  [[nodiscard]] SinkContext get_sink_context() {
    return SinkContext{{shared_from_this(), &sink_queue_}, {}};
  }

  void on_error() {
    on_error_();
  }

private:
  std::function<void()> on_error_;
  SinkEventQueue sink_queue_;
  KeyboardEventQueue keyboard_queue_;
  SourceEventQueue source_queue_;
};

Engine::Engine(std::function<void()> on_error)
  : state_{std::make_shared<State>(std::move(on_error))} {}

Engine::~Engine() noexcept {
  source_thread_.request_stop();
  keyboard_thread_.request_stop();
  sink_thread_.request_stop();
}

void Engine::start() {
  if (!state_) throw std::logic_error("Invalid EngineState");

  stop();
  state_->reset();

  const auto state = state_;
  sink_thread_ = Thread{[=](const std::stop_token& stop_token) noexcept {
    try {
      Logger::set_module("sink");
      auto ctx = state->get_sink_context();
      ctx.run(stop_token);
      QUARK_LOG_INFO("Sink thread exited successfully");
    } catch (const std::exception& e) {
      QUARK_LOG_FATAL("Sink thread crashed: {}", e.what());
      state->on_error();
    }
  }};

  keyboard_thread_ = Thread{[=](const std::stop_token& stop_token) noexcept {
    try {
      Logger::set_module("keyboard");
      auto ctx = state->get_keyboard_context();
      ctx.run(stop_token);
      QUARK_LOG_INFO("Keyboard thread exited successfully");
    } catch (const std::exception& e) {
      QUARK_LOG_FATAL("Keyboard thread crashed: {}", e.what());
      state->on_error();
    }
  }};

  source_thread_ = Thread{[=](const std::stop_token& stop_token) noexcept {
    try {
      Logger::set_module("source");
      auto ctx = state->get_source_context();
      ctx.run(stop_token);
      QUARK_LOG_INFO("Source thread exited successfully");
    } catch (const std::exception& e) {
      QUARK_LOG_FATAL("Source thread crashed: {}", e.what());
      state->on_error();
    }
  }};
}

void Engine::stop() noexcept {
  source_thread_.request_stop();
  keyboard_thread_.request_stop();
  sink_thread_.request_stop();
  source_thread_ = {};
  keyboard_thread_ = {};
  sink_thread_ = {};
}

void Engine::send(EngineSignal signal) {
  if (!state_) throw std::logic_error("Invalid EngineState");
  state_->push(signal);
}
}  // namespace quark
