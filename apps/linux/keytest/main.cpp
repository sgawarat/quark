// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief キーの値を確認するためのアプリケーション
 */
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <format>
#include <print>
#include <string>
#include <utility>
#include <vector>

#include "print.hpp"
#include "quark/linux/fd/epoll.hpp"
#include "quark/linux/fd/input.hpp"
#include "quark/linux/key.hpp"
#include "quark/logging.hpp"
#include "quark/quantum/key.hpp"

int main() {  // NOLINT(readability-function-cognitive-complexity)
  using namespace quark;
  using namespace quark::key_test;
  try {
    auto epfd_res = EpollFd::create();
    if (!epfd_res) QUARK_LOG_WARN("Failed to epoll_create1");

    constexpr size_t MAX_DEVICE_COUNT = 64;
    std::vector<InputFd> infds;
    for (size_t i = 0; i < MAX_DEVICE_COUNT; ++i) {
      const auto path = std::format("/dev/input/event{}", i);
      auto infd_res = InputFd::create(path.c_str());
      if (!infd_res) continue;
      auto&& infd = *infd_res;

      const auto name_res = infd.get_name();
      if (!name_res) {
        QUARK_LOG_WARN("Failed to get_name (index:{})", i);
        continue;
      }
      auto&& name = *name_res;

      // デバイスがキーボードらしい機能を持っているか調べる
      const auto evbits_res = infd.get_bits<0>();
      if (!evbits_res) {
        QUARK_LOG_WARN("Failed to get_bits(0) (index:{})", i);
        continue;
      }
      auto&& evbits = *evbits_res;

      const EventBitArray<0> required_evbits{EV_SYN, EV_KEY, EV_MSC, EV_REP};
      if (!evbits.is_superset_of(required_evbits)) {
        QUARK_LOG_INFO("\"{}\" is not a keyboard (EV:{::#016x})", path, evbits.values());
        continue;
      }
      if (evbits.contains(EV_REL) || evbits.contains(EV_ABS)) {
        QUARK_LOG_INFO("\"{}\" is a pointing device (EV:{::#016x})", path, evbits.values());
        continue;
      }

      const auto keybits_res = infd.get_bits<EV_KEY>();
      if (!keybits_res) {
        QUARK_LOG_WARN("Failed to get keybits (index:{})", i);
        continue;
      }
      auto&& keybits = *keybits_res;

      std::println("-----");
      std::println("\"{}\":", path);
      std::println("NAME = {}", name);
      std::println("BIT = {::#0x}", evbits.values());
      std::println("KEY = {::#0x}", keybits.values());

      std::print("Unsupported Keys: ");
      {
        for (uint16_t i = 0; i < KEY_CNT; ++i) {
          if (!keybits.contains(i)) continue;
          if (to_keycode(Key{i}) != KeyCode{KC_NO}) continue;
          std::print("{} ({}), ", get_key_name(i), i);
        }
      }
      std::println("");

      if (epfd_res) {
        epoll_event eev{.events = EPOLLIN, .data{.u64 = infds.size()}};
        if (epoll_ctl(epfd_res->handle(), EPOLL_CTL_ADD, infd.handle(), &eev) < 0) {
          QUARK_LOG_WARN("Failed to EPOLL_CTL_ADD (path:\"{}\")", path);
        }
        infds.push_back(std::move(infd));
      }
    }
    std::println("-----");

    if (epfd_res) {
      while (true) {
        epoll_event eev{};
        if (epoll_wait(epfd_res->handle(), &eev, 1, -1) == 1) {
          input_event iev{};
          if (read(infds.at(eev.data.u64).handle(), &iev, sizeof(iev)) == sizeof(iev)) {
            print_key_event(eev.data.u64, iev);
          }
        }
      }
    }

    return EXIT_SUCCESS;
  } catch (...) {
    return EXIT_FAILURE;
  }
}
