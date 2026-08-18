/**
 * @file sink.hpp
 * @brief Win32のSinkイベント
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
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
