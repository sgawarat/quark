# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(quark WIN32
    main.cpp
    main.rc
)
target_compile_options(quark PRIVATE
    -Wno-exit-time-destructors
    -Wno-global-constructors
)
target_link_libraries(quark PRIVATE
    config
    engine
    wtsapi32.lib
)
