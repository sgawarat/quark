# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(keytest)
target_link_libraries(keytest PRIVATE
    config
    engine_impl
)

if(WIN32)
  target_sources(keytest PRIVATE
    win32/main.cpp
    win32/print.cpp
  )
elseif(LINUX)
  target_sources(keytest PRIVATE
    linux/main.cpp
    linux/print.cpp
  )
elseif(QUARK_X11)
  find_package(X11 REQUIRED)

  target_sources(keytest PRIVATE
    x11/main.cpp
  )
  target_link_libraries(keytest PRIVATE
      config
      X11::Xkb
  )
endif()
