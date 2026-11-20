# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(quark
  main.cpp
)
target_link_libraries(quark PRIVATE
  config
  engine
)
