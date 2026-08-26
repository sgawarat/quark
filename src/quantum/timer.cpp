// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file timer.cpp
 * @brief タイマー関数
 */
#include <chrono>

#include <cstdint>

extern "C" {
#include <timer.h>
}  // extern "C"

namespace {
using Clock = std::chrono::steady_clock;

template <typename T>
inline auto get_elapsed_time() noexcept {
  return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(Clock::now().time_since_epoch());
}
}  // namespace

extern "C" {
void timer_init() {}

void timer_clear() {}

uint16_t timer_read() {
  return get_elapsed_time<uint16_t>().count();
}

uint32_t timer_read32() {
  return get_elapsed_time<uint32_t>().count();
}

uint16_t timer_elapsed(uint16_t last) {
  const auto now = get_elapsed_time<uint16_t>().count();
  if (now < last) {
    // タイマーがオーバーフローしたら、それ込みで差分を取る
    return uint16_t{0xffff} - last + now + 1;
  }
  return now - last;
}

uint32_t timer_elapsed32(uint32_t last) {
  const auto now = get_elapsed_time<uint32_t>().count();
  if (now < last) {
    // タイマーがオーバーフローしたら、それ込みで差分を取る
    return uint32_t{0xffffffff} - last + now + 1;
  }
  return now - last;
}
}  // extern "C"
