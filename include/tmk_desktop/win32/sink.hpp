// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.hpp
 * @brief Win32のSinkイベント
 */
#pragma once

#include <Windows.h>

namespace tmk_desktop::inline win32 {
/**
 * @brief Sinkで使うOSネイティブなイベント
 */
struct NativeSinkEvent : INPUT {
  constexpr explicit NativeSinkEvent(WORD sc, DWORD flags) noexcept
      : INPUT{
            .type = INPUT_KEYBOARD,
            .ki =
                {
                    .wScan = sc,
                    .dwFlags = KEYEVENTF_SCANCODE | flags,
                },
        } {}
};
}  // namespace tmk_desktop::inline win32
