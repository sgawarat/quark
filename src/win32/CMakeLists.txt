# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(engine_impl STATIC
    scancode.cpp
    injected.cpp
    receiver.cpp
)
target_link_libraries(engine_impl PRIVATE
    config
)
