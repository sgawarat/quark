# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_subdirectory(quantum)

if(WIN32)
  add_subdirectory(win32)
endif()

add_library(engine STATIC
  engine.cpp
)
target_link_libraries(engine PRIVATE
  config
  $<$<TARGET_EXISTS:keymap>:keymap>
  $<$<TARGET_EXISTS:keyboard>:keyboard>
  engine_impl
  quantum
)
