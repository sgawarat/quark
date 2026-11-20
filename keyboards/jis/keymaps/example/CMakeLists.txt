# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(keymap STATIC
  example.cpp
)
target_link_libraries(keymap PRIVATE
  quantum_config
)
