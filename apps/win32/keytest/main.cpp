// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief キーの値を確認するためのアプリケーション
 */
#include <Windows.h>

#include <iostream>
#include <print>

#include "print.hpp"

namespace quark::keytest::inline win32 {
namespace {
LRESULT CALLBACK hookproc(int code, WPARAM wparam, LPARAM lparam) noexcept {
  switch (code) {
    case HC_ACTION: {
      const auto* info_ptr = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);  // NOLINT
      if (!info_ptr) break;

      // 押したキーの番号を表示する
      print_key_event(*info_ptr);

      // Alt+Escで終了する
      if (info_ptr->vkCode == VK_ESCAPE &&
          (info_ptr->flags & (LLKHF_ALTDOWN | LLKHF_UP)) == LLKHF_ALTDOWN) {
        PostQuitMessage(0);
      }

      return TRUE;
    }
    default: break;
  }

  return CallNextHookEx(nullptr, code, wparam, lparam);
}
}  // namespace
}  // namespace quark::keytest::inline win32

int main() {
  try {
    using namespace quark::keytest;

    std::wcout.imbue(std::locale(""));

    // 有効なスキャンコードを列挙する
    std::println("-----  VK  -----");
    for (uint32_t vk = 0x00; vk <= UINT8_MAX; ++vk) {
      const UINT scan = MapVirtualKey(vk, MAPVK_VK_TO_VSC_EX);
      if (scan > 0) print_key_event(vk, scan);
    }
    std::println("-----  SC  -----");
    for (uint32_t scan = 0x0000; scan <= UINT16_MAX; ++scan) {
      const UINT vk = MapVirtualKey(scan, MAPVK_VSC_TO_VK_EX);
      if (vk > 0) print_key_event(vk, scan);
    }
    std::println("----- HOOK -----");

    // 押したキーの番号を表示するため、キーイベントを覗き見る
    auto* const hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookproc, GetModuleHandle(nullptr), 0);
    MSG msg{};
    while (true) {
      const auto result = GetMessage(&msg, nullptr, 0, 0);
      if (result == 0) break;  // WM_QUITを受け取った
      if (result > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    UnhookWindowsHookEx(hook);

    std::println("----- ---- -----");

    return static_cast<int>(msg.wParam);
  } catch (...) {
    return EXIT_FAILURE;
  }
}
