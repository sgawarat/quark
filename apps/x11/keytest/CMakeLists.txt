# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(keytest-x11
  main.cpp
)
target_link_libraries(keytest-x11 PRIVATE
  config
  X11::Xkb
)
