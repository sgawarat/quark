# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(layout STATIC
    oadg109.cpp
)
target_include_directories(layout PUBLIC
    .
)
target_compile_definitions(layout PUBLIC
    QMK_KEYBOARD_H="oadg109.h"
)
target_link_libraries(layout PRIVATE
    config
)
