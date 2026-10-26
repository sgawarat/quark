# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.28)
project(quark VERSION 0.5)

# Options
set(QMK_FIRMWARE_DIR "" CACHE PATH "Path to the QMK Firmware Directory")
set(QUARK_KEYBOARD "" CACHE STRING "Keyboard Name")
set(QUARK_KEYMAP "" CACHE STRING "Keymap Name")
set(QUARK_ENABLE_KEYMAP_LIBRARY FALSE CACHE BOOL "Compiles Your Keymap as a Static Library")

set(QUARK_ENABLE_DEBUGGING_WORKAROUNDS FALSE CACHE BOOL "Enables Workarounds for Debugging")

# QMK Firmware
if(NOT(EXISTS "${QMK_FIRMWARE_DIR}" AND IS_DIRECTORY "${QMK_FIRMWARE_DIR}"))
  message(FATAL_ERROR "QMK Firmware directory '${QMK_FIRMWARE_DIR}' is not found")
endif()

# # clang-tidyにQMKを解析させないようにする
if(CMAKE_C_CLANG_TIDY)
  list(APPEND CMAKE_C_CLANG_TIDY "--exclude-header-filter=${QMK_FIRMWARE_DIR}/.*")
endif()

if(CMAKE_CXX_CLANG_TIDY)
  list(APPEND CMAKE_CXX_CLANG_TIDY "--exclude-header-filter=${QMK_FIRMWARE_DIR}/.*")
endif()

# Keyboard
set(QUARK_KEYBOARD_DIR "${CMAKE_SOURCE_DIR}/keyboards/${QUARK_KEYBOARD}")
cmake_path(ABSOLUTE_PATH QUARK_KEYBOARD_DIR NORMALIZE)
cmake_path(GET QUARK_KEYBOARD_DIR FILENAME QUARK_KEYBOARD_NAME)

if(NOT EXISTS "${QUARK_KEYBOARD_DIR}" OR NOT IS_DIRECTORY "${QUARK_KEYBOARD_DIR}")
  message(FATAL_ERROR "Keyboard '${QUARK_KEYBOARD}' is not found")
endif()

# Keymap
set(QUARK_KEYMAP_DIR "${QUARK_KEYBOARD_DIR}/keymaps/${QUARK_KEYMAP}")
cmake_path(ABSOLUTE_PATH QUARK_KEYMAP_DIR NORMALIZE)
cmake_path(GET QUARK_KEYMAP_DIR FILENAME QUARK_KEYMAP_NAME)

if(NOT EXISTS "${QUARK_KEYMAP_DIR}" OR NOT IS_DIRECTORY "${QUARK_KEYMAP_DIR}")
  message(FATAL_ERROR "Keymap '${QUARK_KEYMAP}' is not found")
endif()

# rulesを読み込む
include("cmake/quantum_rules.cmake")

# 共通設定ライブラリ
add_library(config INTERFACE)
target_precompile_headers(config INTERFACE
  ./include/quark/stable.h
)

# # 個々のライブラリの-Wno-*より前に配置するため、警告フラグセットを全体オプションに直接追加する
add_compile_options(-Wall -Wextra)

if(MSVC)
  target_compile_options(config INTERFACE
    /source-charset:utf-8
  )
endif()

target_compile_options(config INTERFACE
  -fno-rtti

  # C++98の下位互換性を放棄
  -Wno-c++98-compat
  -Wno-c++98-compat-pedantic

  # QMKの_____マクロを許可
  -Wno-reserved-identifier

  # GNU拡張を許可
  -Wno-gnu-include-next
  -Wno-gnu-anonymous-struct

  # switchにdefaultを付けない
  -Wno-switch-default

  # 指示付初期化にて指示子の省略を許可
  -Wno-missing-designated-field-initializers

  # アライメント境界でpaddingが行われても警告しない
  -Wno-padded
)
target_compile_features(config INTERFACE
  cxx_std_23
)
target_compile_definitions(config INTERFACE
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  UNICODE
  _UNICODE
)

if(QUARK_ENABLE_DEBUGGING_WORKAROUNDS)
  target_compile_definitions(config INTERFACE
    # デバッグ用のワークアラウンド（デバッガで停止するときのためにデバイスをgrabしない）
    QUARK_ENABLE_DEBUGGING_WORKAROUNDS
  )
endif()

target_include_directories(config INTERFACE
  ./include
)
target_link_libraries(config INTERFACE
  quantum_config
)

if(EXISTS "${QUARK_KEYBOARD_DIR}/CMakeLists.txt")
  add_subdirectory("${QUARK_KEYBOARD_DIR}")
endif()

if(EXISTS "${QUARK_KEYMAP_DIR}/CMakeLists.txt")
  add_subdirectory("${QUARK_KEYMAP_DIR}")
endif()

add_subdirectory(src)
add_subdirectory(platforms)
add_subdirectory(tools)

enable_testing()

add_subdirectory(tests)
