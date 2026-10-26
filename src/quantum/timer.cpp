// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file timer.cpp
 * @brief タイマー関数
 */
#include <chrono>
#include <cstdint>
#include <ratio>

extern "C" {
#include <timer.h>
}  // extern "C"

namespace {
using Clock = std::chrono::steady_clock;

template <typename T>
inline auto get_elapsed_time() noexcept {
  using namespace std::chrono;
  return duration_cast<duration<T, std::milli>>(Clock::now().time_since_epoch());
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
  return now - last;
}

uint32_t timer_elapsed32(uint32_t last) {
  const auto now = get_elapsed_time<uint32_t>().count();
  return now - last;
}
}  // extern "C"
