# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")
endif()

project(quark VERSION 0.6.0)

# モジュールのスキャンでエラーになることがあるので無効化しておく
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

# Required
set(QMK_FIRMWARE_DIR "$ENV{QMK_FIRMWARE_DIR}" CACHE PATH "Path to the QMK Firmware Directory")
set(QUARK_KEYBOARD "$ENV{QUARK_KEYBOARD}" CACHE STRING "Keyboard Name")
set(QUARK_KEYMAP "$ENV{QUARK_KEYMAP}" CACHE STRING "Keymap Name")

# Optional
set(QUARK_DISABLE_DEBUGGING_WORKAROUNDS "$ENV{QUARK_DISABLE_DEBUGGING_WORKAROUNDS}" CACHE BOOL
  "Disables debugging workarounds")
set(QUARK_ENABLE_TESTING "$ENV{QUARK_ENABLE_TESTING}" CACHE BOOL "Compiles unit tests")
set(QUARK_ENABLE_WARNING_EVERYTHING "${QUARK_ENABLE_WARNING_EVERYTHING}" CACHE BOOL "Enables -Weverything flag")

# QMK Firmware
if(NOT IS_ABSOLUTE "${QMK_FIRMWARE_DIR}")
  cmake_path(ABSOLUTE_PATH QMK_FIRMWARE_DIR NORMALIZE)
endif()

if(NOT(EXISTS "${QMK_FIRMWARE_DIR}" AND IS_DIRECTORY "${QMK_FIRMWARE_DIR}"))
  message(FATAL_ERROR "QMK Firmware directory '${QMK_FIRMWARE_DIR}' is not found")
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

# 共通設定を作る
include("${CMAKE_SOURCE_DIR}/cmake/quark.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/quantum.cmake")

# subdirectories
if(EXISTS "${QUARK_KEYBOARD_DIR}/CMakeLists.txt")
  add_subdirectory("${QUARK_KEYBOARD_DIR}")
endif()

if(EXISTS "${QUARK_KEYMAP_DIR}/CMakeLists.txt")
  add_subdirectory("${QUARK_KEYMAP_DIR}")
endif()

add_subdirectory(src)
add_subdirectory(apps)

if(QUARK_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
