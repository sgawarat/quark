// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.hpp
 * @brief LinuxにおけるSource処理
 */
#pragma once

#include <libudev.h>

#include <cstdint>
#include <memory>
#include <stop_token>
#include <unordered_set>
#include <vector>

#include "quark/event.hpp"
#include "quark/linux/fd/epoll.hpp"
#include "quark/linux/fd/event.hpp"
#include "quark/linux/fd/input.hpp"
#include "quark/linux/fd/uinput.hpp"
#include "quark/utility.hpp"

namespace quark::inline evdev {
/**
 * @brief Source処理に必要な初期データを格納するためのクラス
 */
struct SourceContextDesc {};

/**
 * @brief Source処理を行うためのクラス
 */
class SourceContext final : private Pinned {
public:
  SourceContext(std::shared_ptr<SourceEventQueue> producer, const SourceContextDesc& desc);

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
  template <typename T, auto D>
  using Scoped = std::unique_ptr<T, decltype([](T* ptr) { D(ptr); })>;

  using Udev = Scoped<udev, udev_unref>;
  using UdevMonitor = Scoped<udev_monitor, udev_monitor_unref>;
  using UdevEnumerate = Scoped<udev_enumerate, udev_enumerate_unref>;
  using UdevDevice = Scoped<udev_device, udev_device_unref>;

  struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const {
      return std::hash<std::string_view>{}(sv);
    }
  };
  using StringHashSet = std::unordered_set<std::string, StringHash, std::equal_to<>>;

  static constexpr uint64_t EPOLL_DATA_EVENT = UINT64_MAX - 1;
  static constexpr uint64_t EPOLL_DATA_UDEV_MONITOR = UINT64_MAX - 2;

  struct InputDevice {
    InputFd input_fd;
    UinputFd uinput_fd;
    std::string uinput_sysname;
  };

  void update_udev_monitor(const epoll_event& event);
  void update_input_device(const epoll_event& event, size_t device_index);
  bool receive_input_event(size_t device_index, const InputDevice& device);

  bool add_device(const char* syspath, udev_device* device);
  void remove_device(size_t device_index);

  std::shared_ptr<SourceEventQueue> producer_;
  EpollFd epoll_fd_;                           ///< イベントの受信待ちを行うためのFD
  EventFd event_fd_;                           ///< 停止要求を通知するためのFD
  std::vector<InputDevice> devices_;           ///< 入力デバイスのリスト
  std::vector<size_t> unused_device_indices_;  // デバイスリストの空きスロット番号のリスト
  Udev udev_;                                  ///< UDEVのコンテキスト
  UdevMonitor udev_monitor_;                   ///< UDEV-Monitorのコンテキスト
  StringHashSet uinput_sysname_set_;           ///< uinputが生成したデバイスのsysnameのセット
};
}  // namespace quark::inline evdev
