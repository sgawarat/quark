# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Qt6 COMPONENTS Widgets Svg)

if(Qt6_FOUND)
  set(CMAKE_AUTORCC ON)

  add_executable(quark-ui WIN32
    main.cpp
    resources.qrc
  )
  target_link_libraries(quark-ui PRIVATE
    config
    Qt6::Widgets
    Qt6::Svg
  )

  install(
    TARGETS quark quark-ui
    RUNTIME DESTINATION .
    COMPONENT quark-ui
  )
endif()
