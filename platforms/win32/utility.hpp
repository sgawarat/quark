// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file utility.hpp
 * @brief ユーティリティ
 */
#pragma once

#include <span>

#include <Windows.h>
#include <shellapi.h>

namespace quark::inline win32 {
/**
 * @brief 通知領域にアイコンを登録する
 *
 * @param wnd ウィンドウのハンドル
 * @param id 識別番号
 * @param callback_message ウィンドウプロシージャに送られるメッセージのID
 * @param icon アイコンのハンドル
 * @param title アプリケーションの名前
 * @retval true 登録に成功した
 * @retval false 登録に失敗した
 */
inline bool add_notify_icon(
    HWND wnd, UINT id, UINT callback_message, HICON icon, LPCWSTR title = nullptr) noexcept {
  NOTIFYICONDATA nid{
    .cbSize = sizeof(NOTIFYICONDATA),
    .hWnd = wnd,
    .uID = id,
    .uFlags = static_cast<UINT>(NIF_MESSAGE) | NIF_SHOWTIP | (icon ? NIF_ICON : 0) |
              (title ? NIF_TIP : 0),
    .uCallbackMessage = callback_message,
    .hIcon = icon,
    .szTip = {},
    .uVersion = NOTIFYICON_VERSION_4,
  };

  if (title) {
    const auto tip_span = std::span{nid.szTip};
    lstrcpyn(tip_span.data(), title, tip_span.size());
  }

  // 高負荷などが原因で登録に失敗することがあるので何回か試みる
  constexpr size_t ITERATION_COUNT = 5;
  bool result = false;
  for (size_t i = 0; i < ITERATION_COUNT; ++i) {
    if (Shell_NotifyIcon(NIM_ADD, &nid)) {
      result = true;
      break;
    }

    // タイムアウトした場合、応答が返ってこなかっただけかもしれないので、少し待ってから上手くいっていたかどうかを確認する
    if (GetLastError() == ERROR_TIMEOUT) {
      constexpr DWORD WAIT_MSEC = 100;
      Sleep(WAIT_MSEC);
      if (Shell_NotifyIcon(NIM_MODIFY, &nid)) {
        result = true;
        break;
      }
    }
  }
  if (!result) return false;

  // 新しいバージョンを用いる
  if (!Shell_NotifyIcon(NIM_SETVERSION, &nid)) {
    Shell_NotifyIcon(NIM_DELETE, &nid);
    return false;
  }

  return true;
}

/**
 * @brief 通知領域からアイコンを取り除く
 *
 * @param wnd ウィンドウのハンドル
 * @param id 識別番号
 */
inline void remove_notify_icon(HWND wnd, UINT id) noexcept {
  NOTIFYICONDATA nid{
    .cbSize = sizeof(NOTIFYICONDATA),
    .hWnd = wnd,
    .uID = id,
  };
  Shell_NotifyIcon(NIM_DELETE, &nid);
}

/**
 * @brief 通知領域のアイコンを変更する
 *
 * @param wnd ウィンドウのハンドル
 * @param id 識別番号
 * @param icon アイコンのハンドル
 */
inline void modify_notify_icon(HWND wnd, UINT id, HICON icon) noexcept {
  NOTIFYICONDATA nid{
    .cbSize = sizeof(NOTIFYICONDATA),
    .hWnd = wnd,
    .uID = id,
    .uFlags = NIF_SHOWTIP | NIF_ICON,
    .hIcon = icon,
  };
  Shell_NotifyIcon(NIM_MODIFY, &nid);
}
}  // namespace quark::inline win32
