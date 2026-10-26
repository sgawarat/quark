// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file queue.hpp
 * @brief イベントキュー
 */
#pragma once

#include <chrono>
#include <concepts>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <stop_token>
#include <utility>
#include <variant>

#include "quark/utility.hpp"

namespace quark {
/**
 * @brief イベント時刻を取得するのに使われる時計クラス
 */
using EventClock = std::chrono::steady_clock;

/**
 * @brief イベント時刻
 */
using EventTime = EventClock::time_point;

/**
 * @brief 周期的な処理を行うためのイベント
 */
struct TickEvent {};

/**
 * @brief イベントのvisitor型のコンセプト
 */
template <typename T, typename... Events>
concept EventVisitor = (std::invocable<T, const EventTime&, const Events&> && ...);

/**
 * @brief イベントキュー
 */
template <typename... Events>
class EventQueue final : private Pinned {
public:
  /**
   * @brief イベントキューを流れるパケット
   */
  class Packet final {
  public:
    Packet() = default;

    explicit Packet(EventTime time) : time_{time}, value_{} {}

    template <typename T>
    explicit Packet(std::in_place_t, T&& value)
      : time_{EventClock::now()}, value_{std::forward<T>(value)} {}

    template <typename T, typename... Args>
    explicit Packet(std::in_place_type_t<T>, Args&&... args)
      : time_{EventClock::now()}, value_{std::in_place_type<T>, std::forward<Args>(args)...} {}

    template <EventVisitor Fn>
    void visit(Fn&& fn) const {
      std::visit([&](const auto& event) { std::forward<Fn>(fn)(time_, event); }, value_);
    }

    [[nodiscard]] const auto& time() const noexcept {
      return time_;
    }

  private:
    EventTime time_;
    std::variant<TickEvent, Events...> value_;
  };

  EventQueue() = default;

  ~EventQueue() noexcept = default;

  /**
   * @brief イベントを追加する
   */
  template <typename T>
  void push(T&& value) {
    {
      const std::scoped_lock<std::mutex> lock{mtx_};
      q_.emplace_back(std::in_place, std::forward<T>(value));
    }
    cv_.notify_one();
  }

  /**
   * @brief イベントを構築して追加する
   */
  template <typename T, typename... Args>
  void emplace(Args&&... args) {
    {
      const std::scoped_lock<std::mutex> lock{mtx_};
      q_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
    }
    cv_.notify_one();
  }

  /**
   * @brief イベントを取り出す
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   * @param time タイムアウトする時刻
   * @return 取り出したイベントを返す。指定の時刻までにイベントが来なければ、TickEventを返す。キューが空かつ停止要求が来れば、nulloptを返す。
   */
  std::optional<Packet> pop(const std::stop_token& stop_token, EventTime time) {
    std::unique_lock<std::mutex> lock{mtx_};

    // イベントがなければ、イベントか停止要求か時間経過を待つ
    if (q_.empty()) {
      if (!cv_.wait_until(lock, stop_token, time, [&] { return !q_.empty(); })) {
        // 停止要求が来た場合、それに答える
        if (stop_token.stop_requested()) return std::nullopt;

        // 時間が来た場合、デフォルト値を返す
        return Packet{time};
      }
    }

    // イベントが時間経過後に来た場合、代わりにデフォルト値を返す
    auto& packet = q_.front();
    if (packet.time() >= time) return Packet{time};

    // イベントが時間経過前に来た場合、それを返す
    auto out = std::move(packet);
    q_.pop_front();
    return out;
  }

  /**
   * @brief イベントを取り出す
   *
   * @param stop_token 呼び出し元スレッドのstop_token
   * @return 取り出したイベントを返す。キューが空かつ停止要求が来れば、nulloptを返す。
   */
  std::optional<Packet> pop(const std::stop_token& stop_token) {
    std::unique_lock<std::mutex> lock{mtx_};

    // イベントがなければ、イベントか停止要求を待つ
    if (q_.empty()) {
      if (!cv_.wait(lock, stop_token, [&] { return !q_.empty(); })) {
        // 停止要求が来たので、それに答える
        return std::nullopt;
      }
    }

    // イベントがあるので、それを返す
    auto out = std::move(q_.front());
    q_.pop_front();
    return out;
  }

  /**
   * @brief 内部状態をリセットする
   */
  void reset() {
    const std::unique_lock<std::mutex> lock{mtx_};
    q_ = {};  // 内部で確保されたメモリを解放する
  }

private:
  std::deque<Packet> q_;
  std::mutex mtx_;
  std::condition_variable_any cv_;
};
}  // namespace quark
