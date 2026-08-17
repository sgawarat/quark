/**
 * @file keymap.c
 * @brief キーマップ
 * @copyright Copyright 2026 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#include <tmk_desktop/layouts/oadg109.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  LAYOUT(JP_LPRN),
};

#ifdef __cplusplus
}
#endif
