// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file receiver.cpp
 * @brief Win32の入力イベントを受け取るやつ
 */
#include "receiver.hpp"

namespace quark::inline win32 {
thread_local std::exception_ptr EventReceiver::ep_{};
}  // namespace quark::inline win32
