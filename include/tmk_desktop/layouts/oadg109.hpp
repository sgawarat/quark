/**
 * @file oadg109.hpp
 * @brief 日本語配列
 * @copyright Copyright 2021 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#pragma once

extern "C" {
#include <keyboard.h>
}  // extern "C"

#ifdef _WIN32
#include "win32/oadg109.hpp"
#endif

#define LAYOUT(ESC) {{ \
 (ESC), \
}}

namespace tmk_desktop {
keypos_t key_to_keypos(Key key) {
  switch (key) {
    case K_ESCAPE: return {0, 0};
  }
  return {255, 255};
}
}  // namespace tmk_desktop
