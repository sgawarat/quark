# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_subdirectory(quantum)

if(WIN32)
  add_subdirectory(win32)
endif()

if(LINUX)
  add_subdirectory(linux)
endif()

add_library(engine STATIC
  keyboard.cpp
  engine.cpp
)
target_link_libraries(engine PUBLIC
  engine_impl
  quantum
)
target_link_libraries(engine PRIVATE
  $<$<TARGET_EXISTS:keyboard>:keyboard>
  $<$<TARGET_EXISTS:keymap>:keymap>
)
