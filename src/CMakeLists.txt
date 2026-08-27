# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

if(WIN32)
    add_subdirectory(win32)
endif()

add_subdirectory(quantum)

add_library(engine STATIC
    source.cpp
    keyboard.cpp
    sink.cpp
)
target_compile_options(engine PRIVATE
    -Wno-ctad-maybe-unsupported
)
target_link_libraries(engine PRIVATE
    config
    quantum
    engine_impl
)
