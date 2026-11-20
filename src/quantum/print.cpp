// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file print.cpp
 * @brief QMKで使われるprint関数
 */
#include <array>
#include <cstdarg>
#include <cstdio>

#include "quark/logging.hpp"

extern "C" {
#include "quark/quantum/system/_print.h"  // IWYU pragma: keep

[[gnu::format(printf, 1, 2)]]
void quark_log_printf(const char* fmt, ...) {
  constexpr auto BUFFER_SIZE = 512;
  std::array<char, BUFFER_SIZE> buffer{};
  va_list args{};
  va_start(args, fmt);
  const auto ret = vsnprintf(buffer.data(), buffer.size(), fmt, args);
  va_end(args);
  if (ret > 0) QUARK_LOG_TRACE("{}", std::string_view{buffer.data()});
}
}  // extern "C"
