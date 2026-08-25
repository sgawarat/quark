// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.cpp
 * @brief Source
 */
#include <quark/source.hpp>

#include <atomic>
#include <exception>
#include <thread>

#ifdef _WIN32
#include "win32/receiver.hpp"
#endif

namespace quark {
namespace {
std::thread thread_;                       ///< スレッド
std::atomic<bool> running_{false};         ///< スレッドが動作中かどうか
std::atomic<bool> stop_requested_{false};  ///< スレッドに対する停止要求
EventReceiver receiver_;                   ///< OSから入力イベントを受け取るためのクラス
}  // namespace

bool start_source() {
  // スレッドがすでに動作中であれば何もしない
  if (running_.load(std::memory_order_acquire)) return false;

  // スレッドを完全に停止させる
  if (thread_.joinable()) thread_.join();

  // 新しくスレッドを生成する
  stop_requested_.store(false, std::memory_order_release);
  thread_ = std::thread([] {
    const struct ScopedRunning {
      ScopedRunning() {
        running_.store(true, std::memory_order_release);
      }
      ~ScopedRunning() {
        running_.store(false, std::memory_order_release);
      }
    } _running{};

    try {
      const struct ScopedInit {
        ScopedInit() {
          receiver_.enable();
        }
        ~ScopedInit() {
          receiver_.disable();
        }
      } _init{};

      while (!stop_requested_.load(std::memory_order_acquire)) {
        receiver_.poll();
        std::this_thread::yield();
      }
    } catch (const std::exception& e) {
      on_source_error(e);
    }
  });

  return true;
}

void stop_source() {
  // すでにスレッドが完全に停止していれば何もしない
  if (!thread_.joinable()) return;

  // スレッドが動作中であれば、停止を要求する
  if (running_.load(std::memory_order_acquire)) {
    stop_requested_.store(true, std::memory_order_release);
    receiver_.notify();
  }

  // スレッドが完全に停止するのを待つ
  thread_.join();
}

SourceStatus get_source_status() noexcept {
  if (!running_.load(std::memory_order_acquire)) {
    return SourceStatus::STOPPED;
  }
  if (stop_requested_.load(std::memory_order_acquire)) {
    return SourceStatus::STOP_REQUESTED;
  }
  return SourceStatus::RUNNING;
}
}  // namespace quark
