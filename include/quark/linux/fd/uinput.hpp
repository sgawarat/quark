// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file uinput.hpp
 * @brief Uinput FD
 */
#pragma once

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>

#include <array>
#include <cerrno>

#include "quark/linux/fd/base.hpp"
#include "quark/linux/fd/input.hpp"
#include "quark/linux/key.hpp"

namespace quark::inline evdev {
/**
 * @brief Uinput FDを管理するためのクラス
 */
class UinputFd final : public FdBase {
public:
  /**
   * @brief 仮想キーボード用uinputデバイスを生成する
   */
  static Result<UinputFd> create() noexcept {
    QUARK_EVDEV_RESULT_TRY(UinputFd uifd, open("/dev/uinput", O_WRONLY | O_CLOEXEC));
    QUARK_RESULT_CHECK(uifd.setup());
    return uifd;
  }

  /**
   * @brief inputデバイスのuinputダブルを生成する
   */
  static Result<UinputFd> create(const InputFd& infd) noexcept {
    QUARK_EVDEV_RESULT_TRY(UinputFd uifd, open("/dev/uinput", O_WRONLY | O_CLOEXEC));
    QUARK_RESULT_CHECK(uifd.setup(infd));
    return uifd;
  }

  UinputFd() noexcept = default;

  template <size_t N>
  [[nodiscard]] Result<size_t> get_sysname(std::span<char, N> buffer) const noexcept {
    QUARK_EVDEV_RESULT_TRY(
        auto&& result, ioctl(handle(), UI_GET_SYSNAME(buffer.size()), buffer.data()));

    // 念のため、文字列に終端を付与する
    const auto size = static_cast<size_t>(result);  // 書き込んだバイト数
    if (size <= 0 || size > buffer.size()) return make_error(EFAULT);
    buffer[size - 1] = '\0';  // NOLINT(*-avoid-unchecked-container-access)

    return size;
  }

  [[nodiscard]] Result<std::string> get_sysname() const {
    constexpr size_t NAME_BUFFER_SIZE = 256;
    std::array<char, NAME_BUFFER_SIZE> buffer{};
    QUARK_RESULT_CHECK(get_sysname(std::span{buffer}));
    return std::string{buffer.data()};
  }

  [[nodiscard]] Result<void> write(const input_event& event) const noexcept {
    return write(std::span{&event, 1});
  }

  [[nodiscard]] Result<void> write(std::span<const input_event> events) const noexcept {
    const auto bytes = std::as_bytes(events);
    size_t retry_count = 0;
    size_t offset = 0;
    while (offset < bytes.size()) {
      const int ret = ::write(handle(), bytes.data() + offset, bytes.size() - offset);
      if (ret < 0) [[unlikely]] {
        if (errno == EINTR && retry_count++ < MAX_RETRY_COUNT) continue;
        return make_error(errno);
      }
      if (ret == 0) [[unlikely]] {
        return make_error(ENODEV);
      }
      offset += static_cast<size_t>(ret);
    }
    return {};
  }

private:
  explicit UinputFd(int fd) noexcept : FdBase{fd} {}

  [[nodiscard]] Result<void> setup() const noexcept {
    if (!valid()) return make_error(EBADF);

    // 各種イベントを有効化する
    for (const auto type : std::array{EV_SYN, EV_KEY, EV_REL, EV_REP}) {
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_EVBIT, type));
    }

    // Quantumのキー表現に変換できるキーを有効化する
    constexpr size_t KEY_COUNT = 256;
    for (size_t i = 1; i < KEY_COUNT; ++i) {
      const auto key = to_key(KeyCode{static_cast<uint8_t>(i)});
      if (key == Key::NO) continue;
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_KEYBIT, static_cast<int>(key)));
    }

#ifdef MOUSE_ENABLE
    // マウスボタンを有効化する
    constexpr int BTN_COUNT = 8;
    for (int i = BTN_MOUSE; i < BTN_MOUSE + BTN_COUNT; ++i) {
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_KEYBIT, i));
    }

    // マウス軸を有効化する
    for (const auto type : std::array{REL_X, REL_Y, REL_WHEEL, REL_HWHEEL}) {
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_RELBIT, type));
    }
#endif

    // デバイスをセットアップする
    const uinput_setup ui_setup{
      .id{
        .bustype = BUS_VIRTUAL,
        .vendor = 0x0000,
        .product = 0x0000,
        .version = 0,
      },
      .name = "Quark Virtual Keyboard",
    };
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_DEV_SETUP, &ui_setup));

    // デバイスを生成する
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_DEV_CREATE));

    return {};
  }

  template <int EV, int UI>
  [[nodiscard]] Result<void> set_bits(
      const EventBitArray<0>& evbits, const InputFd& input_fd) const noexcept {
    if (evbits.contains(EV)) {
      QUARK_RESULT_TRY(auto&& bits, input_fd.get_bits<EV>());
      for (const auto i : BitOneView{bits}) {
        QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI, i));

        if constexpr (EV == EV_ABS) {
          uinput_abs_setup setup{.code = static_cast<uint16_t>(i)};
          QUARK_RESULT_TRY(setup.absinfo =, input_fd.get_absinfo(i));
          QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_ABS_SETUP, &setup));
        }
      }
    }
    return {};
  }

  /**
   * @brief Input FDから構成をコピーしてセットアップする
   */
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  [[nodiscard]] Result<void> setup(const InputFd& infd) const noexcept {
    if (!valid()) return make_error(EBADF);
    if (!infd.valid()) return make_error(EINVAL);

    QUARK_RESULT_TRY(auto&& evbits, infd.get_bits<0>());
    for (const auto i : BitOneView{evbits}) {
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_EVBIT, i));
    }

    QUARK_RESULT_CHECK(set_bits<EV_KEY, UI_SET_KEYBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_REL, UI_SET_RELBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_ABS, UI_SET_ABSBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_MSC, UI_SET_MSCBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_LED, UI_SET_LEDBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_SND, UI_SET_SNDBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_FF, UI_SET_FFBIT>(evbits, infd));
    QUARK_RESULT_CHECK(set_bits<EV_SW, UI_SET_SWBIT>(evbits, infd));

    QUARK_RESULT_TRY(auto&& bits, infd.get_props());
    for (const auto i : BitOneView{bits}) {
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_PROPBIT, i));
    }

    constexpr size_t PHYS_BUFFER_SIZE = 256;
    std::array<char, PHYS_BUFFER_SIZE> buffer{};
    if (infd.get_phys(std::span{buffer})) {
      QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_SET_PHYS, buffer.data()));
    }

    // デバイスをセットアップする
    uinput_setup setup{};
    QUARK_RESULT_TRY(setup.id =, infd.get_id());
    QUARK_RESULT_CHECK(infd.get_name(std::span{setup.name}));
    QUARK_RESULT_TRY(setup.ff_effects_max =, infd.get_effects());
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_DEV_SETUP, &setup));

    // デバイスを生成する
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), UI_DEV_CREATE));

    return {};
  }
};
}  // namespace quark::inline evdev
