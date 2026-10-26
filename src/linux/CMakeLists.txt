# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(PkgConfig REQUIRED)

pkg_check_modules(UDEV REQUIRED IMPORTED_TARGET libudev)

add_library(engine_impl STATIC
  key.cpp
  source.cpp
  sink.cpp
)
target_link_libraries(engine_impl PRIVATE
  config
)
target_link_libraries(engine_impl PUBLIC
  PkgConfig::UDEV
)
