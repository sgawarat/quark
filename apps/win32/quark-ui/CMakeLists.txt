# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

set(QUARK_WIN32_SUBSYSTEM "WINDOWS" CACHE STRING "Defines Win32 Subsystem (CONSOLE or WINDOWS)")

if(QUARK_WIN32_SUBSYSTEM STREQUAL CONSOLE)
  add_executable(quark
    main.cpp
    main.rc
  )
  target_compile_definitions(quark PRIVATE
    _CONSOLE
  )
else()
  add_executable(quark WIN32
    main.cpp
    main.rc
  )
  target_compile_definitions(quark PRIVATE
    _WINDOWS
  )
endif()

target_link_libraries(quark PRIVATE
  config
  engine
  wtsapi32.lib
  winmm.lib
)
