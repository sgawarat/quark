// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.cpp
 * @brief LinuxのSink処理
 */
#include "quark/linux/sink.hpp"

#include <linux/input-event-codes.h>
#include <linux/input.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <stop_token>
#include <utility>

#include "quark/event.hpp"
#include "quark/linux/fd/uinput.hpp"
#include "quark/linux/key.hpp"
#include "quark/queue.hpp"

namespace quark::inline evdev {
namespace {
template <size_t N>
struct InputEventBuilder {
  constexpr auto& add(const input_event& event) {
    events_.at(size_++) = event;
    return *this;
  }

  constexpr auto& key_press(Key key) {
    return add({
      .type = EV_KEY,
      .code = static_cast<uint16_t>(key),
      .value = 1,
    });
  }

  constexpr auto& key_release(Key key) {
    return add({
      .type = EV_KEY,
      .code = static_cast<uint16_t>(key),
      .value = 0,
    });
  }

  constexpr auto& button_press(uint16_t index) {
    return add({
      .type = EV_KEY,
      .code = static_cast<uint16_t>(BTN_MOUSE + index),
      .value = 1,
    });
  }

  constexpr auto& button_release(uint16_t index) {
    return add({
      .type = EV_KEY,
      .code = static_cast<uint16_t>(BTN_MOUSE + index),
      .value = 0,
    });
  }

  constexpr auto& rel(uint16_t code, int32_t value) {
    return add({
      .type = EV_REL,
      .code = code,
      .value = value,
    });
  }

  constexpr auto& syn_report() {
    return add({
      .type = EV_SYN,
      .code = SYN_REPORT,
    });
  }

  void send(const UinputFd& uifd) const {
    uifd.write(std::span{events_.data(), size_})
        .or_throw("Failed to send input events to the virtual device");
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    return size_;
  }

private:
  std::array<input_event, N> events_{};
  size_t size_{};
};
}  // namespace

SinkContext::SinkContext(std::shared_ptr<SinkEventQueue> consumer, const SinkContextDesc&)
  : consumer_{std::move(consumer)},
    uinput_fd_{UinputFd::create().or_throw("Failed to create uinput device").value()} {
  if (!consumer_) throw std::invalid_argument("Invalid SinkEventQueue");
}

SinkContext::~SinkContext() noexcept = default;

void SinkContext::run(const std::stop_token& stop_token) {
  while (!stop_token.stop_requested()) {
    const auto packet_opt = consumer_->pop(stop_token);
    if (packet_opt) {
      packet_opt->visit([this](const auto& time, const auto& event) { update(time, event); });
    }
  }
}

void SinkContext::update(EventTime, TickEvent) {}

void SinkContext::update(EventTime, KeyPressEvent event) {
  const auto key = to_key(event.keycode);
  if (key == Key{}) return;
  InputEventBuilder<2>{}.key_press(key).syn_report().send(uinput_fd_);
}

void SinkContext::update(EventTime, KeyReleaseEvent event) {
  const auto key = to_key(event.keycode);
  if (key == Key{}) return;
  InputEventBuilder<2>{}.key_release(key).syn_report().send(uinput_fd_);
}

void SinkContext::update(EventTime, MousePressEvent event) {
  InputEventBuilder<9> events{};
  event.buttons.scan([&](uint8_t i) { events.button_press(i); });
  if (events.size() > 0) events.syn_report().send(uinput_fd_);
}

void SinkContext::update(EventTime, MouseReleaseEvent event) {
  InputEventBuilder<9> events{};
  event.buttons.scan([&](uint8_t i) { events.button_release(i); });
  if (events.size() > 0) events.syn_report().send(uinput_fd_);
}

void SinkContext::update(EventTime, MouseWheelEvent event) {
  InputEventBuilder<3> events{};
  if (event.rel_v != 0) events.rel(REL_WHEEL, event.rel_v);
  if (event.rel_h != 0) events.rel(REL_HWHEEL, event.rel_h);
  if (events.size() > 0) events.syn_report().send(uinput_fd_);
}

void SinkContext::update(EventTime, MouseCursorEvent event) {
  InputEventBuilder<3> events{};
  if (event.rel_x != 0) events.rel(REL_X, event.rel_x);
  if (event.rel_y != 0) events.rel(REL_Y, event.rel_y);
  if (events.size() > 0) events.syn_report().send(uinput_fd_);
}

void SinkContext::update(EventTime, SinkCommand) {}
}  // namespace quark::inline evdev
