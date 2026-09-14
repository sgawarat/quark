# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(keymap STATIC
  example.cpp
)
target_compile_options(keymap PRIVATE

  # -Wno-switch-enum
)
target_link_libraries(keymap PRIVATE
  config
)
