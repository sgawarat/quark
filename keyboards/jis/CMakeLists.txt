# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(keyboard STATIC
    jis.cpp
)
target_compile_options(keyboard PRIVATE
    -Wno-switch-enum
)
target_link_libraries(keyboard PRIVATE
    config
)
