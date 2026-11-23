# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
include_guard(GLOBAL)

include("${CMAKE_SOURCE_DIR}/cmake/quantum/files.cmake")

# rulesを読み込む
load_quantum_rules()

# Quantumの設定をまとめたライブラリを作る
add_library(quantum_config INTERFACE)

# Quantumのルールに基づくconfigヘッダのリスト
get_quantum_config_header_list(QUARK_CONFIG_HEADER_LIST)
target_compile_definitions(quantum_config INTERFACE
  QUARK_CONFIG_HEADER_LIST=${QUARK_CONFIG_HEADER_LIST}
)

# ライブラリ側で用意したconfigヘッダ
target_precompile_headers(quantum_config INTERFACE
  "${CMAKE_SOURCE_DIR}/include/quark/quantum/config.h"
)

target_include_directories(quantum_config SYSTEM INTERFACE
  "${QMK_FIRMWARE_DIR}/quantum"
  "${QMK_FIRMWARE_DIR}/quantum/keymap_extras"
  "${QMK_FIRMWARE_DIR}/quantum/logging"
  "${QMK_FIRMWARE_DIR}/quantum/nvm"
  "${QMK_FIRMWARE_DIR}/quantum/nvm/eeprom"
  "${QMK_FIRMWARE_DIR}/quantum/process_keycode"
  "${QMK_FIRMWARE_DIR}/quantum/sequencer"
  "${QMK_FIRMWARE_DIR}/platforms"
  "${QMK_FIRMWARE_DIR}/tmk_core/protocol"

  # include-nextされるヘッダのため、QMKより後ろに列挙する必要がある
  "${CMAKE_SOURCE_DIR}/include/quark/quantum/system"
)

target_link_libraries(quantum_config INTERFACE
  config
)

# Quantumのソースコードをまとめたライブラリを作る
add_library(quantum_sources INTERFACE)
target_sources(quantum_sources INTERFACE
  "${QMK_FIRMWARE_DIR}/quantum/action.c"
  "${QMK_FIRMWARE_DIR}/quantum/action_layer.c"
  "${QMK_FIRMWARE_DIR}/quantum/action_tapping.c"
  "${QMK_FIRMWARE_DIR}/quantum/action_util.c"
  "${QMK_FIRMWARE_DIR}/quantum/bitwise.c"
  "${QMK_FIRMWARE_DIR}/quantum/debounce/none.c"
  "${QMK_FIRMWARE_DIR}/quantum/eeconfig.c"
  "${QMK_FIRMWARE_DIR}/quantum/keyboard.c"
  "${QMK_FIRMWARE_DIR}/quantum/keycode_config.c"
  "${QMK_FIRMWARE_DIR}/quantum/keymap_common.c"
  "${QMK_FIRMWARE_DIR}/quantum/keymap_introspection.c"
  "${QMK_FIRMWARE_DIR}/quantum/led.c"
  "${QMK_FIRMWARE_DIR}/quantum/logging/debug.c"
  "${QMK_FIRMWARE_DIR}/quantum/logging/print.c"
  "${QMK_FIRMWARE_DIR}/quantum/logging/sendchar.c"
  "${QMK_FIRMWARE_DIR}/quantum/matrix_common.c"
  "${QMK_FIRMWARE_DIR}/quantum/nvm/eeprom/nvm_eeconfig.c"
  "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_default_layer.c"
  "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_oneshot.c"
  "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_quantum.c"
  "${QMK_FIRMWARE_DIR}/quantum/quantum.c"
  "${QMK_FIRMWARE_DIR}/drivers/eeprom/eeprom_driver.c"
  "${QMK_FIRMWARE_DIR}/drivers/eeprom/eeprom_transient.c"
  "${QMK_FIRMWARE_DIR}/platforms/suspend.c"
  "${QMK_FIRMWARE_DIR}/platforms/test/bootloaders/none.c"
  "${QMK_FIRMWARE_DIR}/tmk_core/protocol/host.c"
  "${QMK_FIRMWARE_DIR}/tmk_core/protocol/report.c"
  "${QMK_FIRMWARE_DIR}/tmk_core/protocol/usb_device_state.c"
)

# ビルドオプションが他のビルドオプションを定義する
if(NOT DEBOUNCE_TYPE)
  set(DEBOUNCE_TYPE "sym_defer_g")
endif()

if(MOUSEKEY_ENABLE)
  set(MOUSE_ENABLE yes)
endif()

if(UCIS_ENABLE OR UNICODEMAP_ENABLE OR UNICODE_ENABLE)
  set(UNICODE_COMMON yes)
endif()

# ビルドオプションごとの設定を追加する
if(AUDIO_ENABLE)
  message(WARNING "AUDIO_ENABLE is not supported")
endif()

if(AUTO_SHIFT_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    AUTO_SHIFT_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_auto_shift.c"
  )
endif()

if(AUTOCORRECT_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    AUTOCORRECT_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_autocorrect.c"
  )
endif()

if(BACKLIGHT_ENABLE)
  message(WARNING "BACKLIGHT_ENABLE is not supported")
endif()

if(BATTERY_ENABLE)
  message(WARNING "BATTERY_ENABLE is not supported")
endif()

if(BOOTMAGIC_ENABLE)
  message(WARNING "BOOTMAGIC_ENABLE is not supported")
endif()

if(CAPS_WORD_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    CAPS_WORD_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/caps_word.c"
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_caps_word.c"
  )
endif()

if(COMBO_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    COMBO_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_combo.c"
  )
endif()

if(COMMAND_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    COMMAND_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/command.c"
  )
endif()

if(CONNECTION_ENABLE)
  message(WARNING "CONNECTION_ENABLE is not supported")
endif()

if(CONSOLE_ENABLE)
  message(WARNING "CONSOLE_ENABLE is not supported")
endif()

if(NOT DEBOUNCE_TYPE STREQUAL "custom")
  target_compile_definitions(quantum_config INTERFACE
    DEBOUNCE_TYPE="${DEBOUNCE_TYPE}"
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/debounce/${DEBOUNCE_TYPE}.c"
  )
endif()

if(DEFERRED_EXEC_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    DEFERRED_EXEC_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/deferred_exec.c"
  )
endif()

if(DIGITIZER_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    DIGITIZER_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/digitizer.c"
  )
endif()

if(DYNAMIC_KEYMAP_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    DYNAMIC_KEYMAP_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/dynamic_keymap.c"
  )
endif()

if(DYNAMIC_MACRO_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    DYNAMIC_MACRO_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_dynamic_macro.c"
  )
endif()

if(DYNAMIC_TAPPING_TERM_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    DYNAMIC_TAPPING_TERM_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_dynamic_tapping_term.c"
  )
endif()

if(ENCODER_ENABLE)
  message(WARNING "ENCODER_ENABLE is not supported")
endif()

if(EXTRAKEY_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    EXTRAKEY_ENABLE
  )
endif()

if(GRAVE_ESC_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    GRAVE_ESC_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_grave_esc.c"
  )
endif()

if(HAPTIC_ENABLE)
  message(WARNING "HAPTIC_ENABLE is not supported")
endif()

if(JOYSTICK_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    JOYSTICK_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/joystick.c"
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_joystick.c"
  )
endif()

if(KEY_LOCK_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    KEY_LOCK_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_key_lock.c"
  )
endif()

if(KEY_OVERRIDE_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    KEY_OVERRIDE_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_key_override.c"
  )
endif()

if(LAYER_LOCK_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    LAYER_LOCK_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/layer_lock.c"
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_layer_lock.c"
  )
endif()

if(LEADER_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    LEADER_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/leader.c"
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_leader.c"
  )
endif()

if(LED_MATRIX_ENABLE)
  message(WARNING "LED_MATRIX_ENABLE is not supported")
endif()

if(MAGIC_ENABLE)
  message(WARNING "MAGIC_ENABLE is not supported")
endif()

if(MIDI_ENABLE)
  message(WARNING "MIDI_ENABLE is not supported")
endif()

if(MUSIC_ENABLE)
  message(WARNING "MUSIC_ENABLE is not supported")
endif()

if(MOUSEKEY_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    MOUSEKEY_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/mousekey.c"
  )
endif()

if(MOUSE_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    MOUSE_ENABLE
  )
endif()

if(NKRO_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    NKRO_ENABLE
  )
endif()

if(OS_DETECTION_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    OS_DETECTION_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/os_detection.c"
  )
endif()

if(PAINTER_ENABLE)
  message(WARNING "PAINTER_ENABLE is not supported")
endif()

if(POINTING_DEVICE_ENABLE)
  message(WARNING "POINTING_DEVICE_ENABLE is not supported")
endif()

if(PROGRAMMABLE_BUTTON_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    PROGRAMMABLE_BUTTON_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_programmable_button.c"
    "${QMK_FIRMWARE_DIR}/quantum/programmable_button.c"
  )
endif()

if(RAW_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    RAW_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/raw_hid.c"
  )
endif()

if(REPEAT_KEY_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    REPEAT_KEY_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_repeat_key.c"
    "${QMK_FIRMWARE_DIR}/quantum/repeat_key.c"
  )
endif()

if(RGB_MATRIX_ENABLE)
  message(WARNING "RGB_MATRIX_ENABLE is not supported")
endif()

if(RGBLIGHT_ENABLE)
  message(WARNING "RGBLIGHT_ENABLE is not supported")
endif()

if(SECURE_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    SECURE_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_secure.c"
    "${QMK_FIRMWARE_DIR}/quantum/secure.c"
  )
endif()

if(SEND_STRING_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    SEND_STRING_ENABLE
  )
  target_include_directories(quantum_config INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/send_string"
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/send_string/send_string.c"
  )
endif()

if(SEQUENCER_ENABLE)
  message(WARNING "SEQUENCER_ENABLE is not supported")
endif()

if(SPACE_CADET_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    SPACE_CADET_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_space_cadet.c"
  )
endif()

if(SPLIT_KEYBOARD)
  message(WARNING "SPLIT_KEYBOARD is not supported")
endif()

if(STENO_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    STENO_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_steno.c"
  )

  if(NOT DEFINED STENO_PROTOCOL)
    set(STENO_PROTOCOL all)
  endif()

  if(STENO_PROTOCOL STREQUAL "all")
    target_compile_definitions(quantum_config INTERFACE
      STENO_ENABLE_ALL
      STENO_ENABLE_BOLT
      STENO_ENABLE_GEMINI
    )
  elseif(STENO_PROTOCOL STREQUAL "txbolt")
    target_compile_definitions(quantum_config INTERFACE
      STENO_ENABLE_BOLT
    )
  elseif(STENO_PROTOCOL STREQUAL "geminipr")
    target_compile_definitions(quantum_config INTERFACE
      STENO_ENABLE_GEMINI
    )
  endif()
endif()

if(TAP_DANCE_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    TAP_DANCE_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_tap_dance.c"
  )
endif()

if(TRI_LAYER_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    TRI_LAYER_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_tri_layer.c"
    "${QMK_FIRMWARE_DIR}/quantum/tri_layer.c"
  )
endif()

if(UCIS_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    UCIS_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_ucis.c"
    "${QMK_FIRMWARE_DIR}/quantum/unicode/ucis.c"
  )
endif()

if(UNICODEMAP_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    UNICODEMAP_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_unicodemap.c"
    "${QMK_FIRMWARE_DIR}/quantum/unicode/unicodemap.c"
  )
endif()

if(UNICODE_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    UNICODE_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/unicode/unicode.c"
  )
endif()

if(UNICODE_COMMON)
  target_include_directories(quantum_config INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/unicode"
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_unicode_common.c"
    "${QMK_FIRMWARE_DIR}/quantum/process_keycode/process_unicode.c"
    "${QMK_FIRMWARE_DIR}/quantum/unicode/utf8.c"
  )
endif()

if(WEAR_LEVELING_ENABLE)
  message(WARNING "WEAR_LEVELING_ENABLE is not supported")
endif()

if(WPM_ENABLE)
  target_compile_definitions(quantum_config INTERFACE
    WPM_ENABLE
  )
  target_sources(quantum_sources INTERFACE
    "${QMK_FIRMWARE_DIR}/quantum/wpm.c"
  )
endif()
