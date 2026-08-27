// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file receiver.hpp
 * @brief Win32の入力イベントを受け取るやつ
 */
#pragma once

#include <stdexcept>

#include <Windows.h>

#include <quark/keyboard.hpp>

#include "injected.hpp"

namespace quark::inline win32 {
class EventReceiver final {
public:
  /**
   * @brief 有効化
   */
  void enable() noexcept {
    thread_id_ = GetCurrentThreadId();
    hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, hook_proc, GetModuleHandle(nullptr), 0);
  }

  /**
   * @brief 無効化
   */
  void disable() noexcept {
    if (hook_) {
      UnhookWindowsHookEx(hook_);
      thread_id_ = 0;
      hook_ = nullptr;
    }
  }

  /**
   * @brief イベントを受け取って処理する
   *
   * 停止要求を受けたり異常終了したりしない限りリターンされない。
   */
  void poll() {
    MSG msg{};
    while (true) {
      const BOOL result = GetMessage(&msg, nullptr, 0, 0);
      if (result < 0) throw std::runtime_error("Failed to GetMessage");
      if (result == 0) {  // WM_QUITを受け取った
        if (ep_) {
          std::exception_ptr ep{};
          std::swap(ep, ep_);
          std::rethrow_exception(std::move(ep));
        }
        break;
      }
    }
  }

  /**
   * @brief pollを抜けるよう通知する
   */
  void notify() noexcept {
    if (thread_id_ > 0) PostThreadMessage(thread_id_, WM_QUIT, 0, 0);
  }

private:
  /**
   * @brief フックプロシージャ
   *
   * @note フック処理にはミリ秒単位の制限時間が設けられているので、重たい処理はフックの外で行う。
   */
  static LRESULT CALLBACK hook_proc(int code, WPARAM wparam, LPARAM lparam) noexcept {
    switch (code) {
      case HC_ACTION: {
        KBDLLHOOKSTRUCT* info_ptr = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
        if (!info_ptr) break;

        // エラー発生中は正常に処理できない可能性があるので、安全のために素通りさせる
        if (ep_) break;

        // ハードウェア由来でないキー入力を素通りさせる
        if (info_ptr->flags & (LLKHF_LOWER_IL_INJECTED | LLKHF_INJECTED)) break;

        // 自身に由来するキーイベントを素通りさせる
        if (remove_injected(*info_ptr)) break;

        // キー入力を奪ってエンジンに横流しする
        try {
          send_to_keyboard(*info_ptr);
        } catch (...) {
          ep_ = std::current_exception();
          PostQuitMessage(0);  // 失敗を伝える
          break;
        }
        return TRUE;
      }
      default: break;
    }

    // 素通りさせたキー入力を下流に流す
    return CallNextHookEx(nullptr, code, wparam, lparam);
  }

  HHOOK hook_ = nullptr;                       ///< フックのハンドル
  DWORD thread_id_ = 0;                        ///< スレッドID
  static thread_local std::exception_ptr ep_;  ///< 例外ポインタの一時保管場所
};
}  // namespace quark::inline win32
