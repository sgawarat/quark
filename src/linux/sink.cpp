// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
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
template <bool KEYUP>
class KeyInput {
public:
  explicit KeyInput(Key key) noexcept
    : events_{
        {
          {
            .type = EV_KEY,
            .code = static_cast<uint16_t>(key),
            .value = KEYUP ? 0 : 1,
          },
          {
            .type = EV_SYN,
            .code = SYN_REPORT,
          },
        },
      } {}

  void send(const UinputFd& uifd) const {
    uifd.write(events_).or_throw("Failed to send input events to the virtual device");
  }

private:
  std::array<input_event, 2> events_{};
};

using KeyPressInput = KeyInput<false>;
using KeyReleaseInput = KeyInput<true>;

template <bool KEYUP>
class MouseButtonInput {
public:
  explicit MouseButtonInput(MouseButtonSet buttons) {
    buttons.scan([&](uint8_t index) {
      events_.at(size_++) = {
        .type = EV_KEY,
        .code = static_cast<uint16_t>(BTN_MOUSE + index),
        .value = KEYUP ? 0 : 1,
      };
    });
    if (size_ > 0) {
      events_.at(size_++) = {
        .type = EV_SYN,
        .code = SYN_REPORT,
      };
    }
  }

  void send(const UinputFd& uifd) const {
    if (size_ == 0) return;
    // TODO: events_をsize_に切り詰めて渡す
    uifd.write(events_).or_throw("Failed to send input events to the virtual device");
  }

private:
  static constexpr size_t MAX_EVENT_COUNT = MouseButtonSet::EXTENT + 1;

  std::array<input_event, MAX_EVENT_COUNT> events_{};
  size_t size_{};
};

using MousePressInput = MouseButtonInput<false>;
using MouseReleaseInput = MouseButtonInput<true>;

class MouseWheelInput {
public:
  explicit MouseWheelInput(MouseWheelEvent event) {
    // ホイール縦
    if (event.rel_v != 0) {
      events_.at(size_++) = {
        .type = EV_REL,
        .code = REL_WHEEL,
        .value = event.rel_v,
      };
    }

    // ホイール横
    if (event.rel_h != 0) {
      events_.at(size_++) = {
        .type = EV_REL,
        .code = REL_HWHEEL,
        .value = event.rel_h,
      };
    }

    if (size_ > 0) {
      events_.at(size_++) = {
        .type = EV_SYN,
        .code = SYN_REPORT,
      };
    }
  }

  void send(const UinputFd& uifd) const {
    if (size_ == 0) return;
    uifd.write(events_).or_throw("Failed to send input events to the virtual device");
  }

private:
  std::array<input_event, 3> events_{};
  size_t size_{};
};

class MouseCursorInput {
public:
  explicit MouseCursorInput(MouseCursorEvent event) {
    // カーソル横
    if (event.rel_x != 0) {
      events_.at(size_++) = {
        .type = EV_REL,
        .code = REL_X,
        .value = event.rel_x,
      };
    }

    // カーソル縦
    if (event.rel_y != 0) {
      events_.at(size_++) = {
        .type = EV_REL,
        .code = REL_Y,
        .value = event.rel_y,
      };
    }

    if (size_ > 0) {
      events_.at(size_++) = {
        .type = EV_SYN,
        .code = SYN_REPORT,
      };
    }
  }

  void send(const UinputFd& uifd) const {
    if (size_ == 0) return;
    uifd.write(events_).or_throw("Failed to send input events to the virtual device");
  }

private:
  std::array<input_event, 3> events_{};
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
  if (key == Key::NO) return;
  KeyPressInput{key}.send(uinput_fd_);
}

void SinkContext::update(EventTime, KeyReleaseEvent event) {
  const auto key = to_key(event.keycode);
  if (key == Key::NO) return;
  KeyReleaseInput{key}.send(uinput_fd_);
}

void SinkContext::update(EventTime, MousePressEvent event) {
  MousePressInput{event.buttons}.send(uinput_fd_);
}

void SinkContext::update(EventTime, MouseReleaseEvent event) {
  MouseReleaseInput{event.buttons}.send(uinput_fd_);
}

void SinkContext::update(EventTime, MouseWheelEvent event) {
  MouseWheelInput{event}.send(uinput_fd_);
}

void SinkContext::update(EventTime, MouseCursorEvent event) {
  MouseCursorInput{event}.send(uinput_fd_);
}

void SinkContext::update(EventTime, EngineSignal) {}
}  // namespace quark::inline evdev
