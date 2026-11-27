// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.hpp
 * @brief Win32におけるSource処理
 */
#pragma once

#include <Windows.h>

#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <stop_token>

#include "quark/engine.hpp"
#include "quark/keyboard.hpp"
#include "quark/utility.hpp"
#include "quark/win32/utility.hpp"

namespace quark::inline win32 {
/**
 * @brief Sourceに送られるイベントの時刻をはかる時計
 */
using SourceEventClock = std::chrono::steady_clock;

/**
 * @brief Sourceに送られるイベントの時刻を格納するための型
 */
using SourceEventTime = SourceEventClock::time_point;

/**
 * @brief Sourceに送られるイベントを管理するキュー
 */
class SourceEventQueue {
public:
  /**
   * @brief イベントキューを流れるパケット
   */
  class Packet final {
  public:
    Packet() = default;

    explicit Packet(SourceEventTime time) : time_{time} {}

    template <typename T>
    explicit Packet(std::in_place_t, T&& value)
      : time_{SourceEventClock::now()}, value_{std::forward<T>(value)} {}

    template <typename T, typename... Args>
    explicit Packet(std::in_place_type_t<T>, Args&&... args)
      : time_{SourceEventClock::now()},
        value_{std::in_place_type<T>, std::forward<Args>(args)...} {}

    template <EventVisitor Fn>
    void visit(Fn&& fn) const {
      std::visit([&](const auto& event) { std::forward<Fn>(fn)(time_, event); }, value_);
    }

    [[nodiscard]] auto time() const noexcept {
      return time_;
    }

  private:
    SourceEventTime time_;
    std::variant<TickEvent, EngineSignal> value_;
  };

  SourceEventQueue() : handle_{CreateSemaphoreW(nullptr, 0, LONG_MAX, nullptr)} {
    if (!handle_) throw std::runtime_error("Failed to CreateSemaphore");
  }

  /**
   * @brief イベントを追加する
   */
  template <typename T>
  void push(T&& value) {
    {
      const std::scoped_lock<std::mutex> lock{mutex_};
      queue_.emplace_back(std::in_place, std::forward<T>(value));
    }
    if (ReleaseSemaphore(handle_.get(), 1, nullptr) == 0) {
      throw Win32Error("Failed to ReleaseSemaphore");
    }
  }

  /**
   * @brief イベントを構築して追加する
   */
  template <typename T, typename... Args>
  void emplace(Args&&... args) {
    {
      const std::scoped_lock<std::mutex> lock{mutex_};
      queue_.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
    }
    if (ReleaseSemaphore(handle_.get(), 1, nullptr) == 0) {
      throw Win32Error("Failed to ReleaseSemaphore");
    }
  }

  /**
   * @brief キューからイベントを取り出す
   *
   * @note 通知ハンドルから通知を受けるたびに一度だけ呼び出すことができる。
   */
  std::optional<Packet> pop() {
    const std::unique_lock<std::mutex> lock{mutex_};
    if (queue_.empty()) return std::nullopt;
    auto out = queue_.front();
    queue_.pop_front();
    return out;
  }

  /**
   * @brief キューを空にする
   *
   * @note 以前の通知ハンドルは無効化される。
   */
  void reset() {
    const std::unique_lock<std::mutex> lock{mutex_};
    queue_ = {};  // 内部で確保されたメモリを解放する
    handle_.reset(CreateSemaphoreW(nullptr, 0, LONG_MAX, nullptr));
  }

  /**
   * @brief キューに値が追加されたことを通知するハンドルを取得する
   */
  [[nodiscard]] HANDLE handle() const noexcept {
    return handle_.get();
  }

private:
  std::deque<Packet> queue_;
  std::mutex mutex_;
  Handle handle_;  ///< キューにイベントが追加されたことを通知するためのSemaphore
};

/**
 * @brief Source処理に必要な初期データを格納するためのクラス
 */
struct SourceContextDesc {};

/**
 * @brief Source処理を行うためのクラス
 */
class SourceContext final : private Pinned {
public:
  SourceContext(std::shared_ptr<KeyboardEventQueue> producer,
      std::shared_ptr<SourceEventQueue> consumer, const SourceContextDesc& desc);

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
  LRESULT hookproc(int code, WPARAM wparam, LPARAM lparam);

  void update(SourceEventTime time, TickEvent event);
  void update(SourceEventTime time, EngineSignal signal);

  std::shared_ptr<KeyboardEventQueue> producer_;
  std::shared_ptr<SourceEventQueue> consumer_;
  Hook hook_;
  KeyCode last_key_;  ///< 最後に押されたキー
};
}  // namespace quark::inline win32
