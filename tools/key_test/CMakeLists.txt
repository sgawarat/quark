# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(key_test WIN32
    main.cpp
)
target_link_libraries(key_test PRIVATE
    config
)
