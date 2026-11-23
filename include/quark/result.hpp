// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file result.hpp
 * @brief Result
 */
#pragma once

#include <expected>
#include <format>
#include <system_error>

#include "quark/logging.hpp"

/**
 * @brief TRYで戻り値を受け取る変数の名前を作るマクロ
 */
#define QUARK_RESULT_VARIABLE_NAME QUARK_RESULT_VARIABLE_NAME_1(tmp_result_, __COUNTER__)
#define QUARK_RESULT_VARIABLE_NAME_1(a, b) QUARK_RESULT_VARIABLE_NAME_2(a, b)
#define QUARK_RESULT_VARIABLE_NAME_2(a, b) a##b

/**
 * @brief expectedをアンラップするマクロ
 */
#define QUARK_RESULT_TRY(decl, ...) \
  QUARK_RESULT_TRY_1(decl, QUARK_RESULT_VARIABLE_NAME, __VA_ARGS__)
#define QUARK_RESULT_TRY_1(decl, result, ...) \
  auto&& result = (__VA_ARGS__); \
  if (!result) { \
    return ::std::unexpected{::std::forward<decltype(result)>(result).error()}; \
  } \
  decl(*::std::forward<decltype(result)>(result))

/**
 * @brief expectedをconstでアンラップするマクロ
 */
#define QUARK_RESULT_TRY_CONST(decl, ...) \
  QUARK_RESULT_TRY_CONST_1(decl, QUARK_RESULT_VARIABLE_NAME, __VA_ARGS__)
#define QUARK_RESULT_TRY_CONST_1(decl, result, ...) \
  const auto&& result = (__VA_ARGS__); \
  if (!result) { \
    return ::std::unexpected{std::forward<decltype(result)>(result).error()}; \
  } \
  decl(*::std::forward<decltype(result)>(result))

/**
 * @brief expectedがエラーのときにリターンするマクロ
 */
#define QUARK_RESULT_CHECK(...) \
  do { \
    if (auto&& result = (__VA_ARGS__); !result.has_value()) { \
      return ::std::unexpected{::std::forward<decltype(result)>(result).error()}; \
    } \
  } while (false)

namespace quark {
/**
 * @brief 値かエラーを表現するためのクラス
 */
template <typename T>
class Result : public std::expected<T, std::error_code> {
public:
  using std::expected<T, std::error_code>::expected;
  using std::expected<T, std::error_code>::operator=;

  /**
   * @brief エラー時にログを出力する
   */
  template <typename Self, typename... Args>
  constexpr Self&& or_log(this Self&& self [[clang::lifetimebound]],
      const std::format_string<Args...> what, Args&&... args) {
    if (!self.has_value()) {
      QUARK_LOG_ERROR(
          "{}: {}", std::format(what, std::forward<Args>(args)...), self.error().message());
    }
    return std::forward<Self>(self);
  }

  /**
   * @brief エラー時にsystem_error例外を投げる
   */
  template <typename Self, typename... Args>
  constexpr Self&& or_throw(this Self&& self [[clang::lifetimebound]],
      const std::format_string<Args...> what, Args&&... args) {
    if (!self.has_value()) {
      throw std::system_error{
        std::forward<Self>(self).error(),
        std::format(what, std::forward<Args>(args)...),
      };
    }
    return std::forward<Self>(self);
  }
};
}  // namespace quark
