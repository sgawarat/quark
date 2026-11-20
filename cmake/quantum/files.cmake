# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
include_guard(GLOBAL)

# Quantumのルールに従ってファイルを探す
function(find_quantum_files filename out_var)
  cmake_path(SET dir NORMALIZE "${QUARK_KEYMAP_DIR}")
  cmake_path(SET goal_dir NORMALIZE "${CMAKE_SOURCE_DIR}/keyboards")

  # filenameがパスでないことを確認する
  if(filename MATCHES "[/\\\\]")
    message(FATAL_ERROR "'${filename}' is not filename")
  endif()

  # たどるパスがkeyboardsの中にあることを確認する
  cmake_path(IS_PREFIX goal_dir ${dir} NORMALIZE is_prefix)
  if(NOT is_prefix)
    message(FATAL_ERROR "Keymap '${dir}' is not found in '${goal_dir}'")
  endif()

  # 終点のディレクトリに到達するまで、<filename>とpost_<filename>という名前のファイルを列挙する
  set(out "")
  while(NOT ${dir} STREQUAL ${goal_dir})
    # <filename>はディレクトリの浅い順に並べる
    if(EXISTS "${dir}/${filename}")
      list(PREPEND out "${dir}/${filename}")
    endif()

    # post_<filename>はディレクトリの深い順に並べる
    if(EXISTS "${dir}/post_${filename}")
      list(APPEND out "${dir}/post_${filename}")
    endif()

    # ひとつ上のディレクトリを探す
    cmake_path(GET dir PARENT_PATH parent_dir)

    # 念のため無限ループに陥らないように、ルートディレクトリに到達したか確認する
    if(dir STREQUAL parent_dir)
      message(FATAL_ERROR "Root directory was detected")
    endif()

    # ひとつ上のディレクトリに移動する
    set(dir "${parent_dir}")
  endwhile()

  # 結果を返す
  set(${out_var} ${out} PARENT_SCOPE)
endfunction()

# Quantumのルールに従ってrulesを読み込む
function(load_quantum_rules)
  find_quantum_files(rules.cmake QUANTUM_RULES_FILES)
  foreach(RULES_FILE ${QUANTUM_RULES_FILES})
    # 許可された変数のみを外部に開示する
    block(SCOPE_FOR VARIABLES PROPAGATE
      AUDIO_ENABLE
      AUTO_SHIFT_ENABLE
      AUTOCORRECT_ENABLE
      BACKLIGHT_ENABLE
      BATTERY_ENABLE
      BOOTMAGIC_ENABLE
      CAPS_WORD_ENABLE
      COMBO_ENABLE
      COMMAND_ENABLE
      CONNECTION_ENABLE
      CONSOLE_ENABLE
      DEBOUNCE_TYPE
      DEFERRED_EXEC_ENABLE
      DIGITIZER_ENABLE
      DYNAMIC_KEYMAP_ENABLE
      DYNAMIC_MACRO_ENABLE
      DYNAMIC_TAPPING_TERM_ENABLE
      ENCODER_ENABLE
      EXTRAKEY_ENABLE
      GRAVE_ESC_ENABLE
      HAPTIC_ENABLE
      JOYSTICK_ENABLE
      KEY_LOCK_ENABLE
      KEY_OVERRIDE_ENABLE
      LAYER_LOCK_ENABLE
      LEADER_ENABLE
      LED_MATRIX_ENABLE
      MAGIC_ENABLE
      MIDI_ENABLE
      MUSIC_ENABLE
      MOUSEKEY_ENABLE
      MOUSE_ENABLE
      NKRO_ENABLE
      OS_DETECTION_ENABLE
      PAINTER_ENABLE
      POINTING_DEVICE_ENABLE
      PROGRAMMABLE_BUTTON_ENABLE
      RAW_ENABLE
      REPEAT_KEY_ENABLE
      RGB_MATRIX_ENABLE
      RGBLIGHT_ENABLE
      SECURE_ENABLE
      SEND_STRING_ENABLE
      SEQUENCER_ENABLE
      SPACE_CADET_ENABLE
      SPLIT_KEYBOARD
      STENO_ENABLE
      STENO_PROTOCOL
      TAP_DANCE_ENABLE
      TRI_LAYER_ENABLE
      UCIS_ENABLE
      UNICODEMAP_ENABLE
      UNICODE_ENABLE
      UNICODE_COMMON
      WEAR_LEVELING_ENABLE
      WPM_ENABLE
    )
      include("${RULES_FILE}")
    endblock()
  endforeach()
endfunction()
