# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
include_guard(GLOBAL)

# 共通設定をまとめたライブラリ
add_library(config INTERFACE)

if(MSVC)
  target_compile_options(config INTERFACE
    /WX # 警告をエラーにする
    /GR- # RTTIを無効化する
    /GS # バッファセキュリティチェック
    /sdl # 追加のセキュリティチェック
    /guard:cf # 制御フローガード
  )
  target_link_options(config INTERFACE
    /GUARD:CF # 制御フローガード
    /CETCOMPAT # ハードウェア保護スタック
    /DEPENDENTLOADFLAG:0x800 # DLL検索パスをsystem32のみに制限
  )
  target_compile_definitions(config INTERFACE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
  )
else()
  target_compile_options(config INTERFACE
    -Werror # 警告をエラーにする
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> # RTTIを無効化する

    # TODO: GCC用にセキュリティ系オプションを追加する
  )
endif()

if(QUARK_ENABLE_WARNING_EVERYTHING)
  target_compile_options(config INTERFACE
    -Weverything

    # C++の下位互換性の警告を無効化
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-pre-c++23-compat
    -Wno-pre-c++23-compat-pedantic

    -Wno-switch-default # switchにdefaultを付けなくて良い
    -Wno-padded # アライメント境界でpaddingが行われても警告しない
    -Wno-missing-designated-field-initializers # 指示付初期化にて指示子の省略を許容する
    -Wno-global-constructors # 動的に初期化されるグローバル変数を許容する
    -Wno-exit-time-destructors # グローバル変数がデストラクタを持つことを許容する

    # -Wno-unique-object-duplication # グローバル変数にimport/exort属性が付いていないことを警告しない
    -Wno-unused-member-function # 未使用のメンバー関数が実装されていても良い
    -Wno-c2y-extensions # __COUNTER__の使用を許可
    -Wno-disabled-macro-expansion # 自身と同名のトークンに展開するマクロを許容
    -Wno-lifetime-safety-all # clang v23.1.5では、機能全体をオフにしないとクラッシュする
  )
else()
  target_compile_options(config INTERFACE
    -Wall
    -Wextra
    -Wno-missing-field-initializers
  )
endif()

target_compile_features(config INTERFACE
  c_std_23
  cxx_std_23
)

target_precompile_headers(config INTERFACE
  "${CMAKE_SOURCE_DIR}/include/quark/stable.h"
)

target_include_directories(config INTERFACE
  "${CMAKE_SOURCE_DIR}/include"
)

if(NOT QUARK_DISABLE_DEBUGGING_WORKAROUNDS)
  # デバッガーが正しく機能するように、一部機能を無効化する
  target_compile_definitions(config INTERFACE
    $<$<CONFIG:Debug>:QUARK_ENABLE_DEBUGGING_WORKAROUNDS>
  )
endif()
