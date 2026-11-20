# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

set(CMAKE_AUTORCC ON)

add_executable(quark-qt WIN32
  main.cpp
  resources.qrc
)
target_link_libraries(quark-qt PRIVATE
  config
  Qt6::Widgets
  Qt6::Svg
)
