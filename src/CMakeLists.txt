# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

if(WIN32)
    add_subdirectory(win32)
endif()

add_library(quantum STATIC
    ${QMK_FIRMWARE_DIR}/quantum/action.c
    ${QMK_FIRMWARE_DIR}/quantum/action_layer.c
    ${QMK_FIRMWARE_DIR}/quantum/action_tapping.c
    ${QMK_FIRMWARE_DIR}/quantum/action_util.c
    ${QMK_FIRMWARE_DIR}/quantum/bitwise.c
    ${QMK_FIRMWARE_DIR}/quantum/debounce/none.c
    ${QMK_FIRMWARE_DIR}/quantum/eeconfig.c
    ${QMK_FIRMWARE_DIR}/quantum/keycode_config.c
    ${QMK_FIRMWARE_DIR}/quantum/keyboard.c
    ${QMK_FIRMWARE_DIR}/quantum/keymap_common.c
    ${QMK_FIRMWARE_DIR}/quantum/keymap_introspection.c
    ${QMK_FIRMWARE_DIR}/quantum/led.c
    ${QMK_FIRMWARE_DIR}/quantum/matrix_common.c
    ${QMK_FIRMWARE_DIR}/quantum/nvm/eeprom/nvm_eeconfig.c
    ${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_default_layer.c
    ${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_oneshot.c
    ${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_quantum.c
    ${QMK_FIRMWARE_DIR}/quantum/quantum.c
    ${QMK_FIRMWARE_DIR}/quantum/logging/debug.c
    ${QMK_FIRMWARE_DIR}/quantum/logging/print.c
    ${QMK_FIRMWARE_DIR}/quantum/logging/sendchar.c
    ${QMK_FIRMWARE_DIR}/drivers/eeprom/eeprom_driver.c
    ${QMK_FIRMWARE_DIR}/drivers/eeprom/eeprom_transient.c
    ${QMK_FIRMWARE_DIR}/platforms/suspend.c
    ${QMK_FIRMWARE_DIR}/platforms/test/bootloaders/none.c
    ${QMK_FIRMWARE_DIR}/tmk_core/protocol/host.c
    ${QMK_FIRMWARE_DIR}/tmk_core/protocol/report.c
    ${QMK_FIRMWARE_DIR}/tmk_core/protocol/usb_device_state.c
    timer.cpp
    wait.cpp
)
target_compile_options(quantum PRIVATE
    -Wno-vla
    -Wno-missing-prototypes
    -Wno-sign-conversion
    -Wno-unused-macros
    -Wno-switch-default
    -Wno-unsafe-buffer-usage
    -Wno-declaration-after-statement
    -Wno-implicit-int-conversion
    -Wno-missing-variable-declarations
    -Wno-gnu-union-cast
    -Wno-shadow
    -Wno-documentation
    -Wno-used-but-marked-unused
    -Wno-extra-semi
    -Wno-cast-qual
    -Wno-include-next-absolute-path
)
target_link_libraries(quantum PRIVATE
    config
    keyboard
)

add_library(engine STATIC
    source.cpp
    keyboard.cpp
    sink.cpp
)
target_compile_options(engine PRIVATE
    -Wno-exit-time-destructors
    -Wno-global-constructors
    -Wno-ctad-maybe-unsupported
)
target_link_libraries(engine PRIVATE
    config
    quantum
    engine_impl
)
