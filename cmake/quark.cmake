# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
include_guard(GLOBAL)

# 共通設定をまとめたライブラリ
add_library(config INTERFACE)

add_compile_options(-Weverything -Werror)

if(MSVC)
  # add_compile_options(/W4 /WX)
  target_compile_options(config INTERFACE
    # RTTIを無効化する
    /GR-
  )
else()
  # add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  target_compile_options(config INTERFACE
    # RTTIを無効化する
    -fno-rtti
  )
endif()

target_compile_options(config INTERFACE
  # C++98の下位互換性を放棄
  -Wno-c++98-compat
  -Wno-c++98-compat-pedantic

  # switchにdefaultを付けない
  -Wno-switch-default

  # 指示付初期化にて指示子の省略を許可
  -Wno-missing-designated-field-initializers

  # アライメント境界でpaddingが行われても警告しない
  -Wno-padded

  # グローバル変数がコンストラクタやデストラクタを持つことを許容する
  # TODO: できればグローバル変数を使わないようにしたい
  -Wno-global-constructors
  -Wno-exit-time-destructors

  # グローバル変数にimport/exortアノテーションが付いていないために共有ライブラリでは単一のオブジェクトにまとめられないことを警告するフラグ
  # TODO: そもそもグローバル変数を使わないようにしたい
  -Wno-unique-object-duplication

  # 未使用のメンバー関数が実装されていても良い
  -Wno-unused-member-function
)

target_compile_features(config INTERFACE
  cxx_std_23
)

target_precompile_headers(config INTERFACE
  "${CMAKE_SOURCE_DIR}/include/quark/stable.h"
)

target_include_directories(config INTERFACE
  "${CMAKE_SOURCE_DIR}/include"
)

target_compile_definitions(config INTERFACE
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  UNICODE
  _UNICODE
)

if(NOT QUARK_DISABLE_DEBUGGING_WORKAROUNDS)
  # デバッガーが正しく機能するように、一部機能を無効化する
  target_compile_definitions(config INTERFACE
    $<$<CONFIG:Debug>:QUARK_ENABLE_DEBUGGING_WORKAROUNDS>
  )
endif()
