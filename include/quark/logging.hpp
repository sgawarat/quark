// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file logging.hpp
 * @brief ログ
 */
#pragma once

#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>

#include "quark/utility.hpp"

namespace quark {
/**
 * @brief ログの深刻さのレベル
 */
enum class LogSeverity {
  FATAL,  ///< 継続不能かつ復旧困難
  ERROR,  ///< 継続不能だが復旧可能
  WARN,   ///< 継続可能だが異常な状態
  INFO,   ///< 継続可能かつ正常な状態
  DEBUG,  ///< デバッグに有用な情報
  TRACE,  ///< デバッグに有用な情報の詳細
};

/**
 * @brief LogSeverityを文字列に変換する
 *
 * @return その値の定数名の文字列。不明の場合は空を返す。
 */
constexpr std::string_view to_string(LogSeverity severity) noexcept {
  switch (severity) {
    using enum LogSeverity;
    case FATAL: return "FATAL";
    case ERROR: return "ERROR";
    case WARN: return "WARN";
    case INFO: return "INFO";
    case DEBUG: return "DEBUG";
    case TRACE: return "TRACE";
  }
  return {};
}

#ifdef NDEBUG
/**
 * @brief ログレベルの既定のしきい値
 */
constexpr auto DEFAULT_LOG_LEVEL = LogSeverity::WARN;
#else
/**
 * @brief ログレベルの既定のしきい値
 */
constexpr auto DEFAULT_LOG_LEVEL = LogSeverity::DEBUG;
#endif

/**
 * @brief ロガー
 */
class Logger final : Pinned {
public:
  using Clock = std::chrono::system_clock;

  /**
   * @brief 初期化する
   *
   * @param log_level ログレベルのしきい値。これより低いseverityを持つログは出力されない。
   * @param output_path 出力先のファイルパス。空であれば、代わりにstderrに出力される。
   */
  explicit Logger(LogSeverity log_level, const std::filesystem::path& output_path = {}) {
    s_log_level = log_level;
    if (!output_path.empty()) {
      s_log_file = std::make_unique<std::ofstream>(output_path, std::ios::app);
    }
  }

  ~Logger() noexcept {
    s_log_file = {};
    s_log_level = DEFAULT_LOG_LEVEL;
  }

  /**
   * @brief ログを出力する
   *
   * 出力される文字列はstd::formatの書式で整形される。
   *
   * @param severity ログの深刻さ
   * @param fmt フォーマット文字列
   * @param args 文字列に挿入される値
   */
  template <typename... Args>
  static void log(LogSeverity severity, std::format_string<Args...> fmt, Args&&... args) noexcept {
    if (severity > s_log_level) return;
    try {
      auto& os = s_log_file ? *s_log_file : std::clog;
      os << std::format(
          "{{\"timestamp\":\"{:%FT%TZ}\",\"severity\":{:?},\"module\":{:?},"
          "\"message\":{:?}}}\n",
          Clock::now(), to_string(severity), t_module,
          std::format(fmt, std::forward<Args>(args)...));
      if (severity <= LogSeverity::ERROR) os.flush();  // severityの高いログは即座に出力する
    } catch (const std::exception& e) {
      try {
        std::cerr << "Failed to log: " << e.what() << '\n';
      } catch (...) {
        // できることがない
        (void)0;
      }
    } catch (...) {
      try {
        std::cerr << "Failed to log: Unknown error\n";
      } catch (...) {
        // できることがない
        (void)0;
      }
    }
  }

  /**
   * @brief スレッドごとのログの発信元を示す文字列をセットする
   */
  static void set_module(std::string_view str) noexcept {
    t_module = str;
  }

  /**
   * @brief 指定の条件でログを出力できるか調べる
   */
  [[nodiscard]] static bool is_enabled_for(LogSeverity severity) noexcept {
    return severity <= s_log_level;
  }

private:
  static inline std::unique_ptr<std::ofstream> s_log_file;    ///< 出力先のファイル
  static inline LogSeverity s_log_level = DEFAULT_LOG_LEVEL;  ///< ログレベルのしきい値
  static thread_local inline std::string_view t_module;       ///< ログの発信元を示す文字列
};

/**
 * @brief ログを出力する
 *
 * 設定によりログを出力しない場合、関数の呼び出し自体が行われない。
 */
#define QUARK_LOG(severity, fmt, ...) \
  do { \
    if (::quark::Logger::is_enabled_for(severity)) { \
      ::quark::Logger::log((severity), fmt __VA_OPT__(, ) __VA_ARGS__); \
    } \
  } while (false)

#define QUARK_LOG_FATAL(fmt, ...) QUARK_LOG(::quark::LogSeverity::FATAL, fmt, __VA_ARGS__)
#define QUARK_LOG_ERROR(fmt, ...) QUARK_LOG(::quark::LogSeverity::ERROR, fmt, __VA_ARGS__)
#define QUARK_LOG_WARN(fmt, ...) QUARK_LOG(::quark::LogSeverity::WARN, fmt, __VA_ARGS__)
#define QUARK_LOG_INFO(fmt, ...) QUARK_LOG(::quark::LogSeverity::INFO, fmt, __VA_ARGS__)

// DEBUGとTRACEはデバッグビルドのみで有効
#ifndef NDEBUG
#define QUARK_LOG_DEBUG(fmt, ...) QUARK_LOG(::quark::LogSeverity::DEBUG, fmt, __VA_ARGS__)
#define QUARK_LOG_TRACE(fmt, ...) QUARK_LOG(::quark::LogSeverity::TRACE, fmt, __VA_ARGS__)
#else
#define QUARK_LOG_DEBUG(...) (void)0
#define QUARK_LOG_TRACE(...) (void)0
#endif
}  // namespace quark
