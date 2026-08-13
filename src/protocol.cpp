/**
 * @file protocol.cpp
 * @brief Protocol
 * @copyright Copyright 2026 sgawarat <sgawarat@gmail.com>
 * @license This program is licensed under the GPLv2 or later. For more details, see LICENSE.
 */
#include <common/host.h>

extern "C" {
#ifdef NKRO_ENABLE
uint8_t keyboard_protocol = 1;
#endif
}  // extern "C"
