# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

# clang-tidyにQMKのソースファイルを解析させない
set(CMAKE_C_CLANG_TIDY "")

add_library(quantum_core STATIC
  timer.cpp
  wait.cpp
  print.cpp
  quantum.cpp
)

if(QUARK_ENABLE_WARNING_EVERYTHING)
  target_compile_options(quantum_core PRIVATE
    -Weverything

    -Wno-c2y-extensions
    -Wno-cast-qual
    -Wno-declaration-after-statement
    -Wno-documentation
    -Wno-extra-semi
    -Wno-extra-semi-stmt
    -Wno-gnu-folding-constant
    -Wno-gnu-include-next
    -Wno-gnu-union-cast
    -Wno-implicit-int-conversion
    -Wno-include-next-absolute-path
    -Wno-missing-prototypes
    -Wno-missing-variable-declarations
    -Wno-packed
    -Wno-padded
    -Wno-pre-c11-compat
    -Wno-pre-c23-compat
    -Wno-reserved-identifier
    -Wno-shadow
    -Wno-sign-conversion
    -Wno-switch-default
    -Wno-unaligned-access
    -Wno-unsafe-buffer-usage
    -Wno-unused-macros
    -Wno-unused-parameter
    -Wno-used-but-marked-unused
    -Wno-vla

    # C++
    -Wno-global-constructors
    -Wno-exit-time-destructors
  )
else()
  target_compile_options(quantum_core PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-include-next-absolute-path
  )
endif()

target_link_libraries(quantum_core PRIVATE
  quantum_config
  quantum_sources
)

add_library(quantum INTERFACE)

if(QUARK_ENABLE_KEYMAP_LIBRARY)
  # キーマップをライブラリ化するので、QMKには宣言のみを渡す
  target_compile_definitions(quantum_core PRIVATE
    KEYMAP_C="${CMAKE_SOURCE_DIR}/src/quantum/keymap.h"
  )

  if(EXISTS "${QUARK_KEYMAP_DIR}/keymap.h")
    target_compile_definitions(quantum_core PRIVATE
      QUARK_QUANTUM_KEYMAP_HEADER="${QUARK_KEYMAP_DIR}/keymap.h"
    )
  endif()

  # keymap.cがあれば、それをライブラリ化する
  if(EXISTS "${QUARK_KEYMAP_DIR}/keymap.c")
    add_library(quantum_keymap STATIC
      "${QUARK_KEYMAP_DIR}/keymap.c"
    )
    target_compile_definitions(quantum_keymap PRIVATE
      QMK_KEYBOARD_H="${CMAKE_SOURCE_DIR}/src/quantum/keyboard.h"
    )
    target_link_libraries(quantum_keymap PRIVATE
      quantum_config
    )

    if(EXISTS "${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.h")
      target_compile_definitions(quantum_keymap PRIVATE
        QUARK_QUANTUM_KEYBOARD_HEADER="${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.h"
      )
    endif()

    target_link_libraries(quantum INTERFACE
      quantum_keymap
    )
  endif()

  # <keyboard>.cがあれば、それをライブラリ化する
  if(EXISTS "${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.c")
    add_library(quantum_keyboard STATIC
      "${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.c"
    )
    target_link_libraries(quantum_keyboard PRIVATE
      quantum_config
    )

    target_link_libraries(quantum INTERFACE
      quantum_keyboard
    )
  endif()
else()
  # キーマップをライブラリ化しないので、QMKには実装を渡す
  if(EXISTS "${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.c")
    target_sources(quantum_core PRIVATE
      "${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.c"
    )
  endif()

  if(EXISTS "${QUARK_KEYMAP_DIR}/keymap.c")
    if(EXISTS "${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.h")
      target_compile_definitions(quantum_core PRIVATE
        QUARK_QUANTUM_KEYBOARD_HEADER="${QUARK_KEYBOARD_DIR}/${QUARK_KEYBOARD_NAME}.h"
      )
    endif()

    target_compile_definitions(quantum_core PRIVATE
      QMK_KEYBOARD_H="${CMAKE_SOURCE_DIR}/src/quantum/keyboard.h"
      KEYMAP_C="${QUARK_KEYMAP_DIR}/keymap.c"
    )
  endif()
endif()

target_link_libraries(quantum INTERFACE
  quantum_config
  quantum_core
)
