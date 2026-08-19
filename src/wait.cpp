// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file wait.cpp
 * @brief 待機関数
 */
#include <thread>

extern "C" {
void wait_ms(uintptr_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void wait_us(uintptr_t us) {
  std::this_thread::sleep_for(std::chrono::microseconds(us));
}
}  // extern "C"
