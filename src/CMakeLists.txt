# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

if(WIN32)
    add_subdirectory(win32)
endif()

add_library(engine STATIC
    ${QMK_SOURCE_DIR}/quantum/action.c
    ${QMK_SOURCE_DIR}/quantum/action_layer.c
    ${QMK_SOURCE_DIR}/quantum/action_tapping.c
    ${QMK_SOURCE_DIR}/quantum/action_util.c
    ${QMK_SOURCE_DIR}/quantum/bitwise.c
    ${QMK_SOURCE_DIR}/quantum/debounce/none.c
    ${QMK_SOURCE_DIR}/quantum/eeconfig.c
    ${QMK_SOURCE_DIR}/quantum/keycode_config.c
    ${QMK_SOURCE_DIR}/quantum/keyboard.c
    ${QMK_SOURCE_DIR}/quantum/keymap_common.c
    ${QMK_SOURCE_DIR}/quantum/keymap_introspection.c
    ${QMK_SOURCE_DIR}/quantum/led.c
    ${QMK_SOURCE_DIR}/quantum/matrix_common.c
    ${QMK_SOURCE_DIR}/quantum/nvm/eeprom/nvm_eeconfig.c
    ${QMK_SOURCE_DIR}/quantum/process_keycode/process_default_layer.c
    ${QMK_SOURCE_DIR}/quantum/process_keycode/process_oneshot.c
    ${QMK_SOURCE_DIR}/quantum/process_keycode/process_quantum.c
    ${QMK_SOURCE_DIR}/quantum/quantum.c
    ${QMK_SOURCE_DIR}/quantum/logging/debug.c
    ${QMK_SOURCE_DIR}/quantum/logging/print.c
    ${QMK_SOURCE_DIR}/quantum/logging/sendchar.c
    ${QMK_SOURCE_DIR}/drivers/eeprom/eeprom_driver.c
    ${QMK_SOURCE_DIR}/drivers/eeprom/eeprom_transient.c
    ${QMK_SOURCE_DIR}/platforms/suspend.c
    ${QMK_SOURCE_DIR}/platforms/test/bootloaders/none.c
    ${QMK_SOURCE_DIR}/tmk_core/protocol/host.c
    ${QMK_SOURCE_DIR}/tmk_core/protocol/report.c
    ${QMK_SOURCE_DIR}/tmk_core/protocol/usb_device_state.c

    source.cpp
    keyboard.cpp
    sink.cpp
    timer.cpp
    wait.cpp
)
target_link_libraries(engine PRIVATE
    config
    keyboard
    engine_impl
)
