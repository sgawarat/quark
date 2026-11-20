# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(keyboard STATIC
  jis.cpp
)
target_link_libraries(keyboard PRIVATE
  quantum_config
)
