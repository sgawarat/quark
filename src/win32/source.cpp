// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.cpp
 * @brief Win32のSource処理
 */
#include <quark/win32/source.hpp>

#include <stdexcept>

#include <quark/win32/injected.hpp>

namespace quark::inline win32 {
namespace {
/**
 * @brief スレッドが現在使っているSourceQueueを格納するためのシングルトンクラス
 */
class CurrentQueue final {
public:
  static SourceQueue* get() noexcept {
    return get_instance();
  }

  static void set(SourceQueue* queue) noexcept {
    get_instance() = queue;
  }

private:
  static SourceQueue*& get_instance() noexcept {
    thread_local SourceQueue* instance{};
    return instance;
  }
};

/**
 * @brief フックプロシージャ
 *
 * @note フック処理には制限時間が設けられているので、正常フローで重い処理を行わないこと。
 */
LRESULT CALLBACK hookproc(int code, WPARAM wparam, LPARAM lparam) noexcept {
  switch (code) {
    case HC_ACTION: {
      const auto* info_ptr = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);  // NOLINT
      if (!info_ptr) break;

      // 自身に由来するキーイベントを素通りさせる
      if (info_ptr->dwExtraInfo == EXTRA_INFO_INJECTED) break;

      // 送り先がなければ、代わりに素通りさせる
      auto* current_queue = CurrentQueue::get();
      if (!current_queue) break;

      // キー入力を奪ってエンジンに横流しする
      try {
        current_queue->push(*info_ptr);
      } catch (...) {
        current_queue->catch_exception_in_hookproc();
        CurrentQueue::set(nullptr);
        PostQuitMessage(EXIT_FAILURE);
        break;
      }
      return TRUE;
    }
    default: break;
  }

  // 素通りさせたキー入力を下流に流す
  return CallNextHookEx(nullptr, code, wparam, lparam);
}
}  // namespace

SourceQueue::SourceQueue() = default;

SourceQueue::~SourceQueue() noexcept = default;

void SourceQueue::push(const SourceEvent& event) {
  {
    const std::scoped_lock<std::mutex> lock{queue_mtx_};
    queue_.push_back({.time = SourceClock::now(), .event = event});
  }
  queue_cv_.notify_one();
}

bool SourceQueue::pop(
    const std::stop_token& stop_token, SourceClock::time_point time, SourceEvent& out) {
  std::unique_lock<std::mutex> lock{queue_mtx_};

  if (queue_.empty()) {
    //  値かないので、値か停止要求か時間経過を待つ
    if (!queue_cv_.wait_until(lock, stop_token, time, [&] { return !queue_.empty(); })) {
      // 停止要求が来たなら、それに答える
      if (stop_token.stop_requested()) return false;

      // 時間が来たなら、tick eventを返す
      out = {};
      return true;
    }
  }

  // イベントが時間経過後に来たなら、代わりにtick eventを返す
  const auto& item = queue_.front();
  if (item.time >= time) {
    out = {};
    return true;
  }

  // 時間経過前のイベントがあるので、それを返す
  out = item.event;
  queue_.pop_front();
  return true;
}

SourceContext::SourceContext(SourceQueue& queue)
  : queue_{queue}, hook_{SetWindowsHookEx(WH_KEYBOARD_LL, hookproc, GetModuleHandle(nullptr), 0)} {
  CurrentQueue::set(&queue);
  if (!hook_) throw std::runtime_error("Failed to SetWindowsHookEx");
}

SourceContext::~SourceContext() noexcept {
  if (hook_) UnhookWindowsHookEx(hook_);
  CurrentQueue::set(nullptr);
}

void SourceContext::run(const std::stop_token& stop_token) {
  const DWORD thread_id = GetCurrentThreadId();
  const std::stop_callback _{
    stop_token, [=] { PostThreadMessage(thread_id, WM_QUIT, EXIT_SUCCESS, 0); }};

  MSG msg{};
  while (true) {
    const BOOL result = GetMessage(&msg, nullptr, 0, 0);
    if (result < 0) throw std::runtime_error("Failed to GetMessage");

    if (result == 0) {  // WM_QUITを受け取った
      if (msg.wParam == EXIT_FAILURE) queue_.rethrow_exception_in_hookproc();
      break;
    }
  }
}
}  // namespace quark::inline win32
