// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file handle.hpp
 * @brief ハンドル
 */
#pragma once

#include <memory>
#include <type_traits>

/**
 * @brief ハンドルを破棄するための関数オブジェクト型
 *
 * @tparam T ハンドルを表すポインタ型
 * @tparam D ハンドルの破棄を定義する関数
 */
template <typename T, auto D>
  requires(std::is_pointer_v<T> && std::is_invocable_v<decltype(D), T>)
struct HandleDeleter {
  constexpr void operator()(T handle) noexcept {
    D(handle);
  }
};

/**
 * @brief ハンドルを管理するためのunique_ptrラッパークラス
 *
 * @tparam T ハンドルを表すポインタ型
 * @tparam D ハンドルの破棄を定義する関数
 */
template <typename T, auto D>
  requires(std::is_pointer_v<T> && std::is_invocable_v<decltype(D), T>)
using UniqueHandle = std::unique_ptr<std::remove_pointer_t<T>, HandleDeleter<T, D>>;
