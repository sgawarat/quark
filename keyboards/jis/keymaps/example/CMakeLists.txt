# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

# マルチメディアキーを有効化
set(EXTRAKEY_ENABLE yes PARENT_SCOPE)

# マウスキーを有効化
set(MOUSEKEY_ENABLE yes PARENT_SCOPE)

# Nキーロールオーバーを有効化
set(NKRO_ENABLE yes PARENT_SCOPE)

add_library(keymap STATIC
    example.cpp
)
target_compile_options(keymap PRIVATE
    -Wno-switch-enum
)
target_link_libraries(keymap PRIVATE
    config
)
