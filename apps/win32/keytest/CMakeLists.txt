# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(keytest
  main.cpp
  print.cpp
)
target_link_libraries(keytest PRIVATE
    config
)

install(
  TARGETS keytest
  RUNTIME DESTINATION .
  COMPONENT keytest
)
