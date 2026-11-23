// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.hpp
 * @brief LinuxにおけるSource処理
 */
#pragma once

#include <libudev.h>

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <stop_token>
#include <unordered_set>
#include <vector>

#include "quark/engine.hpp"
#include "quark/handle.hpp"
#include "quark/keyboard.hpp"
#include "quark/linux/fd/epoll.hpp"
#include "quark/linux/fd/event.hpp"
#include "quark/linux/fd/input.hpp"
#include "quark/linux/fd/uinput.hpp"
#include "quark/utility.hpp"

namespace quark::inline evdev {
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

  SourceEventQueue()
    : event_fd_{EventFd::create_semaphore().or_throw("Failed to create eventfd").value()} {}

  /**
   * @brief イベントを追加する
   */
  template <typename T>
  void push(T&& value) {
    {
      const std::scoped_lock<std::mutex> lock{mutex_};
      queue_.emplace_back(std::in_place, std::forward<T>(value));
    }
    event_fd_.write(1).or_throw("Failed to eventfd_write");
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
    event_fd_.write(1).or_throw("Failed to eventfd_write");
  }

  /**
   * @brief キューからイベントを取り出す
   */
  std::optional<Packet> pop() {
    const std::unique_lock<std::mutex> lock{mutex_};
    if (queue_.empty()) return std::nullopt;
    auto out = queue_.front();
    event_fd_.read().or_throw("Failed to eventfd_read");
    queue_.pop_front();
    return out;
  }

  /**
   * @brief キューを初期化する
   */
  void reset() {
    const std::unique_lock<std::mutex> lock{mutex_};
    queue_ = {};  // 内部で確保されたメモリを解放する
    event_fd_ = EventFd::create_semaphore().or_throw("Failed to create eventfd").value();
  }

  /**
   * @brief キューに値が追加されたことを通知するハンドルを取得する
   */
  [[nodiscard]] int handle() const noexcept {
    return event_fd_.handle();
  }

private:
  std::deque<Packet> queue_;
  std::mutex mutex_;
  EventFd event_fd_;  ///< キューにイベントが追加されたことを通知するためのEvent
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
  using Udev = UniqueHandle<udev*, udev_unref>;
  using UdevMonitor = UniqueHandle<udev_monitor*, udev_monitor_unref>;
  using UdevEnumerate = UniqueHandle<udev_enumerate*, udev_enumerate_unref>;
  using UdevDevice = UniqueHandle<udev_device*, udev_device_unref>;

  struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const {
      return std::hash<std::string_view>{}(sv);
    }
  };
  using StringHashSet = std::unordered_set<std::string, StringHash, std::equal_to<>>;

  static constexpr uint64_t EPOLL_DATA_STOP_REQUESTED = UINT64_MAX - 1;
  static constexpr uint64_t EPOLL_DATA_UDEV_MONITOR = UINT64_MAX - 2;
  static constexpr uint64_t EPOLL_DATA_EVENT_QUEUE = UINT64_MAX - 3;

  struct InputDevice {
    InputFd input_fd;
    UinputFd uinput_fd;
    std::string uinput_sysname;
  };

  void update(SourceEventTime time, TickEvent event);
  void update(SourceEventTime time, EngineSignal signal);

  void update_udev_monitor(const epoll_event& event);
  void update_input_device(const epoll_event& event, size_t device_index);
  bool receive_input_event(size_t device_index, const InputDevice& device);

  bool add_device(const char* syspath, udev_device* device);
  void remove_device(size_t device_index);

  std::shared_ptr<KeyboardEventQueue> producer_;
  std::shared_ptr<SourceEventQueue> consumer_;
  EpollFd epoll_fd_;                           ///< イベントの受信待ちを行うためのFD
  EventFd event_fd_;                           ///< 停止要求を通知するためのFD
  std::vector<InputDevice> devices_;           ///< 入力デバイスのリスト
  std::vector<size_t> unused_device_indices_;  // デバイスリストの空きスロット番号のリスト
  Udev udev_;                                  ///< UDEVのコンテキスト
  UdevMonitor udev_monitor_;                   ///< UDEV-Monitorのコンテキスト
  StringHashSet uinput_sysname_set_;           ///< uinputが生成したデバイスのsysnameのセット
};
}  // namespace quark::inline evdev
