// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file result.hpp
 * @brief Result
 */
#pragma once

#include <cerrno>
#include <expected>
#include <system_error>

#include "quark/result.hpp"

/**
 * @brief システムコールをResult型でハンドリングするマクロ
 */
#define QUARK_EVDEV_RESULT_TRY(decl, ...) \
  QUARK_RESULT_TRY(decl, ::quark::evdev::make_result(__VA_ARGS__))

/**
 * @brief システムコールをResult型でハンドリングするマクロ
 */
#define QUARK_EVDEV_RESULT_TRY_CONST(decl, ...) \
  QUARK_RESULT_TRY_CONST(decl, ::quark::evdev::make_result(__VA_ARGS__))

/**
 * @brief システムコールをResult型でハンドリングするマクロ
 */
#define QUARK_EVDEV_RESULT_CHECK(...) QUARK_RESULT_CHECK(::quark::evdev::make_result(__VA_ARGS__))

namespace quark::inline evdev {
/**
 * @brief POSIXエラーコードをstd::error_codeにラップする
 */
constexpr std::error_code make_error_code(int code) noexcept {
  return std::error_code{code, std::generic_category()};
}

/**
 * @brief POSIXエラーコードをstd::unexpectedにラップする
 */
constexpr std::unexpected<std::error_code> make_error(int code) noexcept {
  return std::unexpected{make_error_code(code)};
}

/**
 * @brief システムコールの戻り値をResultにラップする
 */
template <typename T>
constexpr Result<T> make_result(T ret) noexcept {
  if (ret < 0) return make_error(errno);
  return ret;
}
}  // namespace quark::inline evdev
