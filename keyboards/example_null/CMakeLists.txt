# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(keyboard STATIC
    keymap.c
)
target_link_libraries(keyboard PRIVATE
    config
    layout
)
