// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file thread.hpp
 * @brief スレッド
 */
#pragma once

#include <Windows.h>
#include <sddl.h>

#include <concepts>
#include <stop_token>
#include <tuple>

#include "quark/logging.hpp"
#include "quark/utility.hpp"
#include "quark/win32/utility.hpp"

namespace quark::inline win32 {
/**
 * @brief スレッドを管理するためのクラス
 *
 * よりセキュアな属性が既定で付与される。
 */
class Thread : MoveOnly {
public:
  Thread() = default;

  /**
   * @brief スレッドを生成する
   *
   * std::threadと異なり、引数を取ることはできない。
   *
   * @note テンプレートパラメータに`typename... Args`があると、clang-cl v22では"cannot mangle this pack expansion yet"というコンパイルエラーが発生する。
   */
  template <typename Fn>
    requires(std::invocable<Fn, std::stop_token>)
  explicit Thread(Fn&& fn) {
#ifdef QUARK_ENABLE_DEBUGGING_WORKAROUNDS
#ifdef NDEBUG
#warning "Debugging Workarounds are enabled on Release Build"
#endif
    QUARK_LOG_WARN("Debugging Workarounds are enabled");
    // デバッグ可能なセキュリティ記述子を作る
    void* const desc = nullptr;
#else
    // 外部からの一切を禁止するセキュリティ記述子を作る
    PSECURITY_DESCRIPTOR desc{};
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:P", SDDL_REVISION_1, &desc, nullptr) == 0) {
      throw Win32Error("Failed to ConvertStringSecurityDescriptorToSecurityDescriptor");
    }
    const UniqueHandle<PSECURITY_DESCRIPTOR, LocalFree> sd_ptr{desc};
#endif
    SECURITY_ATTRIBUTES attr{
      .nLength = sizeof(attr),
      .lpSecurityDescriptor = desc,
      .bInheritHandle = FALSE,
    };

    // 引数リストを作る
    using ArgList = std::tuple<std::decay_t<Fn>, std::stop_token>;
    auto arg_list = std::make_unique<ArgList>(std::forward<Fn>(fn), stop_source_.get_token());

    // スレッドを起動する
    handle_ = Handle{
      // NOLINTNEXTLINE(*-type-reinterpret-cast,performance-no-int-to-ptr)
      reinterpret_cast<HANDLE>(_beginthreadex(
          &attr, 0,
          [](void* data) -> unsigned {
            if (!data) return 0;
            const std::unique_ptr<ArgList> arg_list2{static_cast<ArgList*>(data)};
            std::apply(Invoker{}, std::move(*arg_list2));
            return 0;
          },
          arg_list.get(), 0, nullptr)),
    };
    if (!handle_) throw Win32Error(_doserrno, "Failed to _beginthreadex");

    // スレッドの起動に成功したので、引数リストの所有権を放棄する
    arg_list.release();
  }

  Thread(Thread&& other) noexcept
    : stop_source_{std::move(other.stop_source_)}, handle_{std::move(other.handle_)} {}

  Thread& operator=(Thread&& other) noexcept {
    Thread tmp{std::move(other)};
    swap(*this, tmp);
    return *this;
  }

  ~Thread() noexcept {
    if (!handle_) return;
    stop_source_.request_stop();
    WaitForSingleObject(handle_.get(), INFINITE);
  }

  bool request_stop() noexcept {
    return stop_source_.request_stop();
  }

  friend void swap(Thread& lhs, Thread& rhs) noexcept {
    std::swap(lhs.stop_source_, rhs.stop_source_);
    std::swap(lhs.handle_, rhs.handle_);
  }

private:
  struct Invoker {
    template <typename Fn, typename... Args>
    static constexpr auto operator()(Fn&& fn, Args&&... args) {
      return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
  };

  std::stop_source stop_source_;
  Handle handle_;
};
}  // namespace quark::inline win32
