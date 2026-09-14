// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file receiver.hpp
 * @brief Win32のSource処理
 */
#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <stop_token>
#include <variant>

#include <WinUser.h>

#include <quark/utility.hpp>
#include <quark/win32/key.hpp>

namespace quark::inline win32 {
/**
 * @brief イベントの時刻を得るための時計クラス
 */
using SourceClock = std::chrono::steady_clock;

/**
 * @brief Win32のキーイベント
 */
class SourceKeyEvent final {
public:
  SourceKeyEvent() = default;

  SourceKeyEvent(const KBDLLHOOKSTRUCT& event) noexcept
    : vk_{event.vkCode}, sc_{event.scanCode}, flags_{event.flags} {}

  [[nodiscard]] Key key() const noexcept {
    return make_key(vk_, sc_, flags_);
  }

  [[nodiscard]] bool is_pressed() const noexcept {
    return !static_cast<bool>(flags_ & LLKHF_UP);
  }

private:
  DWORD vk_{};
  DWORD sc_{};
  DWORD flags_{};
};

/**
 * @brief Sourceから渡されるシグナルを表す列挙型
 */
enum class SourceSignal {
  CLEAR,  ///< すべてのキーを離す
  RESET,  ///< 初期状態に戻す
};

/**
 * @brief Sourceから渡されるイベントを格納するクラス
 */
using SourceEvent = std::variant<std::monostate, SourceKeyEvent, SourceSignal>;

/**
 * @brief Sourceから渡されるイベントのためのキューを管理するクラス
 */
class SourceQueue final : private Stationary {
public:
  SourceQueue();

  ~SourceQueue() noexcept;

  /**
   * @brief キューにイベントを追加する
   */
  void push(const SourceEvent& event);

  /**
   * @brief キューからイベントを取り出す
   *
   * タイムアウトした場合、空のイベントがeventに格納される。
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   * @param time タイムアウトする時刻
   * @param event 受け取ったイベントの格納先
   * @retval true イベントを受け取った
   * @retval false 停止要求を受け取った
   */
  bool pop(const std::stop_token& stop_token, SourceClock::time_point time, SourceEvent& out);

  /**
   * @brief フックプロシージャで投げられた例外を捕捉する
   */
  void catch_exception_in_hookproc() noexcept {
    exception_in_hookproc_ = std::current_exception();
  }

  /**
   * @brief フックプロシージャで補足した例外をrethrowする
   */
  [[noreturn]] void rethrow_exception_in_hookproc() {
    std::rethrow_exception(exception_in_hookproc_);
  }

private:
  struct Item {
    SourceClock::time_point time;
    SourceEvent event;
  };

  std::deque<Item> queue_;
  std::mutex queue_mtx_;
  std::condition_variable_any queue_cv_;
  std::exception_ptr exception_in_hookproc_;  ///< フックプロシージャで投げられた例外
};

/**
 * @brief Source処理を行うためのクラス
 */
class SourceContext final : private Stationary {
public:
  SourceContext(SourceQueue& queue);

  ~SourceContext() noexcept;

  /**
   * @brief ループ処理を行う
   *
   * 停止要求を受けるか異常停止するまで制御を返さない。
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   */
  void run(const std::stop_token& stop_token);

private:
  SourceQueue& queue_;
  HHOOK hook_{};
};
}  // namespace quark::inline win32
