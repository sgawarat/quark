// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.cpp
 * @brief LinuxのSource処理
 */
#include "quark/linux/source.hpp"

#include <libudev.h>
#include <linux/input-event-codes.h>
#include <sys/epoll.h>

#include <chrono>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <string_view>
#include <system_error>
#include <thread>
#include <utility>

#include "quark/event.hpp"
#include "quark/linux/key.hpp"
#include "quark/logging.hpp"
#include "quark/quantum/key.hpp"

namespace quark::inline evdev {
SourceContext::SourceContext(std::shared_ptr<SourceEventQueue> producer, const SourceContextDesc&)
  : producer_{std::move(producer)} {
  if (!producer_) throw std::invalid_argument("Invalid SourceEventQueue");

  // 受信待機用FDを作る
  epoll_fd_ = EpollFd::create().or_throw("Failed to create EpollFd").value();

  // 終了通知用FDを作る
  event_fd_ = EventFd::create().or_throw("Failed to create EventFd").value();
  epoll_fd_.add(event_fd_.handle(), {.events = EPOLLIN, .data{.u64 = EPOLL_DATA_EVENT}})
      .or_throw("Failed to add EventFd to Epoll");

  // udev経由で入力デバイスの接続を監視する
  udev_ = Udev{udev_new()};
  if (!udev_) throw std::runtime_error("Failed to udev_new");
  udev_monitor_ = UdevMonitor{udev_monitor_new_from_netlink(udev_.get(), "udev")};
  if (!udev_monitor_) throw std::runtime_error("Failed to create udev_monitor");
  if (udev_monitor_filter_add_match_subsystem_devtype(udev_monitor_.get(), "input", nullptr) < 0 ||
      udev_monitor_enable_receiving(udev_monitor_.get()) < 0) {
    throw std::runtime_error("Failed to udev_monitor_enable_receiving");
  }
  epoll_fd_
      .add(udev_monitor_get_fd(udev_monitor_.get()),
          {.events = EPOLLIN, .data{.u64 = EPOLL_DATA_UDEV_MONITOR}})
      .or_throw("Failed to add UdevMonitorFd to Epoll");

  // すでに接続されているキーボードデバイスを列挙する
  const UdevEnumerate udev_enumerate{udev_enumerate_new(udev_.get())};
  if (!udev_enumerate) throw std::runtime_error("Failed to udev_enumerate_new");
  if (udev_enumerate_add_match_subsystem(udev_enumerate.get(), "input") < 0 ||
      udev_enumerate_add_match_sysname(udev_enumerate.get(), "event*") < 0 ||
      udev_enumerate_add_match_property(udev_enumerate.get(), "ID_INPUT_KEYBOARD", "1") < 0 ||
      udev_enumerate_scan_devices(udev_enumerate.get()) < 0) {
    throw std::runtime_error("Failed to udev_enumerate_scan_devices");
  }

  for (udev_list_entry* entry = udev_enumerate_get_list_entry(udev_enumerate.get()); entry;
      entry = udev_list_entry_get_next(entry)) {
    const char* syspath = udev_list_entry_get_name(entry);
    if (!syspath) continue;

    const UdevDevice device{udev_device_new_from_syspath(udev_.get(), syspath)};
    if (!device) {
      warn_log("{} was skipped: Failed to udev_device_new_from_syspath", syspath);
      continue;
    }

    add_device(syspath, device.get());
  }
}

SourceContext::~SourceContext() noexcept = default;

void SourceContext::run(const std::stop_token& stop_token) {
  const std::stop_callback _{
    stop_token, [this] { event_fd_.write(1).or_log("Failed to request_stop to Source"); }};

  while (!stop_token.stop_requested()) {
    // イベントが来るのを待つ
    // ただし、通知が正常に送られなかったときのために、定期的に制御を戻して停止要求を確認する
    const auto timeout = std::chrono::seconds(1);
    const auto event_opt =
        epoll_fd_.wait_for(timeout).or_throw("Failed to wait for input events").value();
    if (!event_opt) continue;
    auto&& event = *event_opt;

    // Event FD経由で終了要求が来たらループを抜ける
    if (event.data.u64 == EPOLL_DATA_EVENT) break;

    // デバイスの接続状況に変化があれば、それを反映させる
    if (event.data.u64 == EPOLL_DATA_UDEV_MONITOR) {
      update_udev_monitor(event);
      continue;
    }

    // デバイスの入力イベントが来たら読み取りを行う
    const size_t device_index = event.data.u64;
    if (device_index < devices_.size()) {
      update_input_device(event, device_index);
      continue;
    }
  }
}

void SourceContext::update_udev_monitor(const epoll_event& event) {
  if ((event.events & (EPOLLERR | EPOLLHUP)) != 0) {
    // TODO: udevを見失ったときに再接続を行うべきか検討する
    throw std::runtime_error("udev was disconnected");
  }

  if ((event.events & EPOLLIN) != 0) {
    const UdevDevice device{udev_monitor_receive_device(udev_monitor_.get())};
    if (!device) return;
    const char* syspath = udev_device_get_syspath(device.get());
    if (!syspath) return;
    const char* action = udev_device_get_action(device.get());
    if (!action) return;
    const std::string_view action_sv{action};

    if (action_sv == "add") {
      add_device(syspath, device.get());
    }
    // TODO: udevのchangeアクションはどうやってハンドリングする？

    // デバイスの取り外しはepoll側で処理するので、action == "remove"は何もしない
  }
}

void SourceContext::update_input_device(const epoll_event& event, size_t device_index) {
  // デバイスを見失ったら、それをリストから除外する
  if ((event.events & (EPOLLERR | EPOLLHUP)) != 0) {
    remove_device(device_index);
    return;
  }

  // イベントが届いていれば、それを読み出す
  if ((event.events & EPOLLIN) != 0) {
    const auto& device = devices_.at(device_index);

    // SYN_REPORTまで処理する
    // ただし、最大回数で打ち切って長時間占有しないようにする
    constexpr size_t MAX_EVENT_COUNT = 32;
    for (size_t event_count = 0; event_count < MAX_EVENT_COUNT; ++event_count) {
      if (!receive_input_event(device_index, device)) break;
    }
  }
}

bool SourceContext::receive_input_event(size_t device_index, const InputDevice& device) {
  const auto event_res = device.input_fd.read();
  if (!event_res) {
    // 異常が発生したら、そのデバイスを監視リストから除外する
    remove_device(device_index);
    return false;  // 中断
  }
  const auto& event_opt = *event_res;

  // イベントが来なかったら、次のループでもう一度試みる
  if (!event_opt) return true;  // 継続
  const auto& event = *event_opt;

  // イベントが来たら、それを振り分ける
  if (event.type == EV_KEY) {
    const auto keycode = to_keycode(Key{event.code});
    if (keycode != KC_NO) {
      // QMKが認識するキーの入力イベントは仮想キーボードへ横流しする
      if (event.value == 0) {
        producer_->emplace<KeyReleaseEvent>(keycode);
      } else if (event.value == 1) {
        producer_->emplace<KeyPressEvent>(keycode);
      }
      return true;  // 継続
    }
  }

  // その他のイベントはそのuinputダブルへ受け流す
  if (!device.uinput_fd.write(event)) {
    // 異常が発生したら、デバイスを監視リストから除外する
    warn_log("Failed to write a input event back to the device #{}", device_index);
    remove_device(device_index);
    return false;
  }

  // SYN_REPORTが来たら、そこでひと区切り
  if (event.type == EV_SYN && event.code == SYN_REPORT) return false;  // 中断

  // SYN_DROPPEDが来たら、処理が追いついていないという証なので、安全のためにデバイスを監視リストから除外する
  if (event.type == EV_SYN && event.code == SYN_DROPPED) {
    // TODO: SYN_DROPPEDを正しくハンドリングする
    warn_log("Input events was dropped in the device #{}", device_index);
    remove_device(device_index);
    return false;  // 中断
  }

  return true;  // 継続
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
bool SourceContext::add_device(const char* syspath, udev_device* device) {
  if (!syspath || !device) throw std::invalid_argument("Invalid input device");

  // そのデバイスが入力イベントを送るものであることを確認する
  const char* const sysname = udev_device_get_sysname(device);
  if (!sysname || !std::string_view{sysname}.starts_with("event")) {
    info_log("{} was skipped: This is not an input event", syspath);
    return false;
  }

  // そのデバイスがキーボードであることを確認する
  const char* const keyboard = udev_device_get_property_value(device, "ID_INPUT_KEYBOARD");
  if (!keyboard || std::string_view{keyboard} != "1") {
    info_log("{} was skipped: This is not a keyboard", syspath);
    return false;
  }

  // そのデバイスがuinputダブルでないことを確認する
  udev_device* const parent_device = udev_device_get_parent(device);
  if (!parent_device) {
    info_log("{} was skipped: This is a virtual device", syspath);
    return false;
  }
  const char* const parent_sysname = udev_device_get_sysname(parent_device);
  if (!parent_sysname || uinput_sysname_set_.contains(parent_sysname)) {
    info_log("{} was skipped: This is a virtual device", syspath);
    return false;
  }

  // そのデバイスを開く
  const char* const devnode = udev_device_get_devnode(device);
  if (!devnode) {
    info_log("{} was skipped: Invalid devnode", syspath);
    return false;
  }
  auto input_fd_res = InputFd::create(devnode);
  if (!input_fd_res) {
    info_log("{} was skipped: Failed to open input: {}", syspath, input_fd_res.error().message());
    return false;
  }
  auto&& input_fd = *std::move(input_fd_res);

  // そのデバイスがSinkの仮想デバイスでないことを確認する
  const auto input_name_res = input_fd.get_name();
  if (input_name_res && *input_name_res == "Quark Virtual Keyboard") {
    info_log("{} was skipped: This is my virtual device", syspath);
    return false;
  }

  // そのデバイスと対になる仮想デバイスを開く
  auto uinput_fd_res = UinputFd::create(input_fd);
  if (!uinput_fd_res) {
    info_log("{} was skipped: Failed to open uinput: {}", syspath, uinput_fd_res.error().message());
    return false;
  }
  auto&& uinput_fd = *std::move(uinput_fd_res);

  // 作られた仮想デバイスのsysnameを取得する
  auto uinput_sysname_res = uinput_fd.get_sysname();
  if (!uinput_sysname_res) {
    info_log("{} was skipped: Invalid uinput sysname: {}", syspath,
        uinput_sysname_res.error().message());
    return false;
  }
  auto&& uinput_sysname = *std::move(uinput_sysname_res);

  // キーが押されたままgrabするとungrabするまで押されたままになるので、ユーザーがすべてのキーを離すまで少し待つ
  // TODO: 押しっぱなしキー問題のきちんとした解決策を考える
  if (![&] {
        auto keys_res = input_fd.get_keys();
        if (!keys_res) return false;
        if (!keys_res->any()) return true;
        info_log("Please release all keys on the {}", syspath);

        constexpr size_t COUNT = 10;
        const auto DELTA = std::chrono::milliseconds(100);
        for (size_t i = 0; i < COUNT; i++) {
          auto keys_res = input_fd.get_keys();
          if (!keys_res) return false;
          if (!keys_res->any()) return true;
          std::this_thread::sleep_for(DELTA);
        }
        return false;
      }()) {
    warn_log("{} was skipped: Stuck key was detected", syspath);
    return false;
  }

  // grabしたままデバッガで停止するとリピートが止まらなかったり何も入力できなくなったりするので無効化できるようにしてある
#ifndef QUARK_ENABLE_DEBUGGING_WORKAROUNDS
  // そのデバイスをgrabする
  const auto input_grab_res = input_fd.grab();
  if (!input_grab_res) {
    warn_log("{} was skipped: Failed to grab: {}", syspath, input_grab_res.error().message());
    return false;
  }
#endif

  // そのデバイスをイベント監視対象に含める
  const size_t device_index =
      unused_device_indices_.empty() ? devices_.size() : unused_device_indices_.back();
  const auto epoll_add_res =
      epoll_fd_.add(input_fd.handle(), {.events = EPOLLIN, .data{.u64 = device_index}});
  if (!epoll_add_res) {
    warn_log(
        "{} was skipped: Failed to add to epoll: {}", syspath, epoll_add_res.error().message());
    return false;
  }

  // デバイスリストに追加する
  if (unused_device_indices_.empty()) {
    // 空きがないので、末尾に挿入する
    auto& slot =
        devices_.emplace_back(std::move(input_fd), std::move(uinput_fd), std::move(uinput_sysname));
    uinput_sysname_set_.emplace(slot.uinput_sysname);
  } else {
    // 空きがあるので、そこに挿入する
    auto& slot = devices_.at(device_index);
    if (slot.input_fd.valid()) {
      throw std::logic_error("An unused device slot should be empty");
    }
    slot.input_fd = std::move(input_fd);
    slot.uinput_fd = std::move(uinput_fd);
    slot.uinput_sysname = std::move(uinput_sysname);
    uinput_sysname_set_.emplace(slot.uinput_sysname);
    unused_device_indices_.pop_back();
  }

  info_log("{} was added to the device list as #{}", syspath, device_index);
  return true;
}

void SourceContext::remove_device(size_t device_index) {
  auto& device = devices_.at(device_index);

  // 解放済みスロットを再び解放してはいけない
  if (!device.input_fd.valid()) {
    throw std::logic_error("A used device slot should be not empty");
  }

  epoll_fd_.del(device.input_fd.handle())
      .or_log("Failed to remove the device #{} from epoll", device_index);

  uinput_sysname_set_.erase(device.uinput_sysname);
  device.uinput_sysname = {};
  device.uinput_fd = {};
  device.input_fd = {};
  unused_device_indices_.push_back(device_index);
  info_log("Device #{} was removed from the device list", device_index);
}
}  // namespace quark::inline evdev
