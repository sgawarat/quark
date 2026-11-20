# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

# cmakeの情報を使ってバージョン情報のリソースファイルを作る
configure_file(version.rc.in version.rc @ONLY)

add_executable(quark-ui WIN32
  main.cpp
  main.rc
  "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)
target_link_options(quark-ui PRIVATE
  LINKER:/ENTRY:mainCRTStartup
)
target_link_libraries(quark-ui PRIVATE
  config
)

install(
  TARGETS quark quark-ui
  RUNTIME DESTINATION .
  COMPONENT quark-ui
)
