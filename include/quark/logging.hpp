// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file logging.hpp
 * @brief ログ
 */
#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <print>
#include <string_view>

#ifdef _WIN32
#include <debugapi.h>
#elifdef __linux__
#include <unistd.h>
#endif

namespace quark {
/**
 * @brief ログを出力する
 *
 * std::formatの書式を用いる。
 *
 * @tparam ArgsT 文字列に挿入する値の型
 * @param severity ログレベル
 * @param fmt フォーマット文字列
 * @param args 文字列に挿入する値
 */
template <typename... ArgsT>
inline void log(
    const std::string_view severity, std::format_string<ArgsT...> fmt, ArgsT&&... args) noexcept {
  try {
    const auto message = std::format(fmt, std::forward<ArgsT>(args)...);

    // QMKのprintlnマクロが定義されていれば一時的に無効化しておく
#pragma push_macro("println")
#ifdef println
#undef println
#endif
    std::println(std::cerr, R"({{"timestamp":"{:%FT%TZ}","severity":{:?},"message":{:?}}})",
        std::chrono::system_clock::now(), severity, message);
#pragma pop_macro("println")

  } catch (const std::exception& e) {
    // エラーの起こりにくい出力先に対して、最小限のログを出力する
#ifdef _WIN32
    OutputDebugStringA("[Quark] Failed to log: ");
    OutputDebugStringA(e.what());
#else
    const std::string_view head = "[Quark] Failed to log: ";
    const std::string_view what = e.what();
    write(STDERR_FILENO, head.data(), head.size());
    write(STDERR_FILENO, what.data(), what.size());
#endif
  }
}

/**
 * @brief formatの書式で致命的エラーログを出力する
 *
 * 処理に失敗して、もう他にできることがないときに用いる。
 *
 * @tparam ArgsT 文字列に挿入する値の型
 * @param fmt フォーマット文字列
 * @param args 文字列に挿入する値
 */
template <typename... ArgsT>
inline void critical_log(std::format_string<ArgsT...> fmt, ArgsT&&... args) noexcept {
  log("critical", fmt, std::forward<ArgsT>(args)...);
}

/**
 * @brief formatの書式を用いてエラーログを出力する
 *
 * 処理に失敗したが、まだ他にできることがあるときに用いる。
 *
 * @tparam ArgsT 文字列に挿入する値の型
 * @param fmt フォーマット文字列
 * @param args 文字列に挿入する値
 */
template <typename... ArgsT>
inline void error_log(std::format_string<ArgsT...> fmt, ArgsT&&... args) noexcept {
  log("error", fmt, std::forward<ArgsT>(args)...);
}

/**
 * @brief formatの書式を用いて警告ログを出力する
 *
 * 通常では起こり得ないことが起こったときに用いる。
 *
 * @tparam ArgsT 文字列に挿入する値の型
 * @param fmt フォーマット文字列
 * @param args 文字列に挿入する値
 */
template <typename... ArgsT>
inline void warn_log(std::format_string<ArgsT...> fmt, ArgsT&&... args) noexcept {
  log("warn", fmt, std::forward<ArgsT>(args)...);
}

/**
 * @brief formatの書式を用いて情報ログを出力する
 *
 * 通常でも起こり得ることが起こったときに用いる。
 *
 * @tparam ArgsT 文字列に挿入する値の型
 * @param fmt フォーマット文字列
 * @param args 文字列に挿入する値
 */
template <typename... ArgsT>
inline void info_log(std::format_string<ArgsT...> fmt, ArgsT&&... args) noexcept {
  log("info", fmt, std::forward<ArgsT>(args)...);
}

/**
 * @brief formatの書式を用いてデバッグログを出力する
 *
 * デバッグに必要な情報を得るために用いる。
 * デバッグビルドでのみ有効。
 *
 * @tparam ArgsT 文字列に挿入する値の型
 * @param fmt フォーマット文字列
 * @param args 文字列に挿入する値
 */
template <typename... ArgsT>
inline void debug_log(
    [[maybe_unused]] std::format_string<ArgsT...> fmt, [[maybe_unused]] ArgsT&&... args) noexcept {
#ifndef NDEBUG
  log("debug", fmt, std::forward<ArgsT>(args)...);
#endif
}
}  // namespace quark
