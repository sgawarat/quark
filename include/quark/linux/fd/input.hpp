// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file input.hpp
 * @brief Input FD
 */
#pragma once

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <optional>
#include <span>
#include <string>
#include <utility>

#include "quark/bitset.hpp"
#include "quark/linux/fd/base.hpp"
#include "quark/linux/result.hpp"

namespace quark::inline evdev {
template <int TYPE>
struct EventType;

template <>
struct EventType<0> {
  static constexpr int TYPE = 0;
  static constexpr size_t COUNT = EV_CNT;
};

#define QUARK_EVDEV_DEFINE_EVENT_TYPE(type) \
  template <> \
  struct EventType<EV_##type> { \
    static constexpr int TYPE = EV_##type; \
    static constexpr size_t COUNT = type##_CNT; \
  };

QUARK_EVDEV_DEFINE_EVENT_TYPE(KEY)
QUARK_EVDEV_DEFINE_EVENT_TYPE(REL)
QUARK_EVDEV_DEFINE_EVENT_TYPE(ABS)
QUARK_EVDEV_DEFINE_EVENT_TYPE(MSC)
QUARK_EVDEV_DEFINE_EVENT_TYPE(SW)
QUARK_EVDEV_DEFINE_EVENT_TYPE(LED)
QUARK_EVDEV_DEFINE_EVENT_TYPE(REP)
QUARK_EVDEV_DEFINE_EVENT_TYPE(SND)
QUARK_EVDEV_DEFINE_EVENT_TYPE(FF)

template <int TYPE>
using EventBitArray = BitArray<EventType<TYPE>::COUNT>;

/**
 * @brief Input FDを管理するためのクラス
 */
class InputFd final : public FdBase {
public:
  static Result<InputFd> create(const char* path) {
    QUARK_EVDEV_RESULT_TRY(InputFd infd, open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC));
    return std::move(infd);
  }

  InputFd() noexcept = default;

  [[nodiscard]] Result<void> grab() const noexcept {
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), EVIOCGRAB, 1));
    return {};
  }

  [[nodiscard]] Result<void> ungrab() const noexcept {
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), EVIOCGRAB, 0));
    return {};
  }

  [[nodiscard]] Result<std::optional<input_event>> read() const noexcept {
    input_event event{};
    const ssize_t ret = ::read(handle(), &event, sizeof(event));
    if (ret < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) return std::nullopt;
      return make_error(errno);
    }
    if (ret == 0) return make_error(ENODEV);
    if (ret != sizeof(event)) return make_error(EIO);
    return event;
  }

  template <int TYPE>
  [[nodiscard]] Result<EventBitArray<TYPE>> get_bits() const noexcept {
    EventBitArray<TYPE> bits;
    QUARK_EVDEV_RESULT_CHECK(
        ioctl(handle(), EVIOCGBIT(TYPE, bits.values().size_bytes()), bits.values().data()));
    return bits;
  }

  [[nodiscard]] Result<input_absinfo> get_absinfo(size_t index) const noexcept {
    input_absinfo info{};
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), EVIOCGABS(index), &info));
    return info;
  }

  [[nodiscard]] Result<BitArray<INPUT_PROP_CNT>> get_props() const noexcept {
    BitArray<INPUT_PROP_CNT> bits;
    QUARK_EVDEV_RESULT_CHECK(
        ioctl(handle(), EVIOCGPROP(bits.values().size_bytes()), bits.values().data()));
    return bits;
  }

  template <size_t N>
  [[nodiscard]] Result<size_t> get_phys(std::span<char, N> buffer) const noexcept {
    QUARK_EVDEV_RESULT_TRY(
        auto&& result, ioctl(handle(), EVIOCGPHYS(buffer.size()), buffer.data()));

    // 念のため、文字列に終端を付与する
    const auto size = static_cast<size_t>(result);  // 書き込んだバイト数
    if (size <= 0 || size > buffer.size()) return make_error(EFAULT);
    buffer[size - 1] = '\0';  // NOLINT(*-avoid-unchecked-container-access)

    return size;
  }

  [[nodiscard]] Result<std::string> get_phys() const {
    constexpr size_t PHYS_BUFFER_SIZE = 256;
    std::array<char, PHYS_BUFFER_SIZE> buffer{};
    QUARK_RESULT_CHECK(get_phys(std::span{buffer}));
    return std::string{buffer.data()};
  }

  [[nodiscard]] Result<input_id> get_id() const noexcept {
    input_id id{};
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), EVIOCGID, &id));
    return id;
  }

  template <size_t N>
  [[nodiscard]] Result<size_t> get_name(std::span<char, N> buffer) const noexcept {
    QUARK_EVDEV_RESULT_TRY(
        auto&& result, ioctl(handle(), EVIOCGNAME(buffer.size()), buffer.data()));

    // 念のため、文字列に終端を付与する
    const auto size = static_cast<size_t>(result);  // 書き込んだバイト数
    if (size <= 0 || size > buffer.size()) return make_error(EFAULT);
    buffer[size - 1] = '\0';  // NOLINT(*-avoid-unchecked-container-access)

    return size;
  }

  [[nodiscard]] Result<std::string> get_name() const {
    constexpr size_t NAME_BUFFER_SIZE = 256;
    std::array<char, NAME_BUFFER_SIZE> buffer{};
    QUARK_RESULT_CHECK(get_name(std::span{buffer}));
    return std::string{buffer.data()};
  }

  [[nodiscard]] Result<size_t> get_effects() const noexcept {
    int effects{};
    QUARK_EVDEV_RESULT_CHECK(ioctl(handle(), EVIOCGEFFECTS, &effects));
    return static_cast<size_t>(effects);
  }

  [[nodiscard]] Result<BitArray<KEY_CNT>> get_keys() const noexcept {
    BitArray<KEY_CNT> bits;
    QUARK_EVDEV_RESULT_CHECK(
        ioctl(handle(), EVIOCGKEY(bits.values().size_bytes()), bits.values().data()));
    return bits;
  }

private:
  explicit InputFd(int fd) noexcept : FdBase{fd} {}
};
}  // namespace quark::inline evdev
