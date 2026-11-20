// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file wait.cpp
 * @brief 待機関数
 */
#include <chrono>
#include <cstdint>
#include <thread>

extern "C" {
void wait_ms(uintptr_t msec) {
  if (msec > 0) std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}
}  // extern "C"
