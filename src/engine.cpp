// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file engine.cpp
 * @brief キーボードエンジン
 */
#include "quark/engine.hpp"

#include <exception>
#include <memory>
#include <stdexcept>
#include <stop_token>

#include "quark/event.hpp"
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
  State() = default;

  ~State() noexcept = default;

  void reset() {
    source_queue_.reset();
    sink_queue_.reset();
  }

  void push(SourceCommand command) {
    source_queue_.push(command);
  }

  void push(SinkCommand command) {
    sink_queue_.push(command);
  }

  [[nodiscard]] SourceContext get_source_context() {
    return SourceContext{{shared_from_this(), &source_queue_}, {}};
  }

  [[nodiscard]] KeyboardContext get_keyboard_context() {
    return KeyboardContext{
      {shared_from_this(), &source_queue_}, {shared_from_this(), &sink_queue_}};
  }

  [[nodiscard]] SinkContext get_sink_context() {
    return SinkContext{{shared_from_this(), &sink_queue_}, {}};
  }

private:
  SinkEventQueue sink_queue_;
  SourceEventQueue source_queue_;
};

Engine::Engine() : state_{std::make_shared<State>()} {}

Engine::~Engine() noexcept {
  source_thread_.request_stop();
  keyboard_thread_.request_stop();
  sink_thread_.request_stop();
}

void Engine::start(void (*on_error)(void*) noexcept, void* data) {
  if (!state_) throw std::logic_error("Invalid EngineState");

  stop();
  state_->reset();

  const auto state = state_;
  sink_thread_ = std::jthread{[=](const std::stop_token& stop_token) noexcept {
    try {
      auto ctx = state->get_sink_context();
      ctx.run(stop_token);
      info_log("Sink thread exited successfully");
    } catch (const std::exception& e) {
      critical_log("Sink thread crashed: {}", e.what());
      if (on_error) on_error(data);
    }
  }};

  keyboard_thread_ = std::jthread{[=](const std::stop_token& stop_token) noexcept {
    try {
      auto ctx = state->get_keyboard_context();
      ctx.run(stop_token);
      info_log("Keyboard thread exited successfully");
    } catch (const std::exception& e) {
      critical_log("Keyboard thread crashed: {}", e.what());
      if (on_error) on_error(data);
    }
  }};

  source_thread_ = std::jthread{[=](const std::stop_token& stop_token) noexcept {
    try {
      auto ctx = state->get_source_context();
      ctx.run(stop_token);
      info_log("Source thread exited successfully");
    } catch (const std::exception& e) {
      critical_log("Source thread crashed: {}", e.what());
      if (on_error) on_error(data);
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
  if (signal == EngineSignal::CLEAR) {
    state_->push(SourceCommand::CLEAR);
  }
}
}  // namespace quark
