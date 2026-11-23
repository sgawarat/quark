# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

# cmakeの情報を使ってバージョン情報のリソースファイルを作る
configure_file(version.rc.in version.rc @ONLY)

add_executable(quark
  main.cpp
  "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)
target_link_libraries(quark PRIVATE
  config
  engine
  winmm.lib
)

install(
  TARGETS quark
  RUNTIME DESTINATION .
  COMPONENT quark
)
