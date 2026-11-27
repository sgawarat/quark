// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file utility.hpp
 * @brief 便利機能
 */
#pragma once

#include <windows.h>

#include <memory>
#include <string>
#include <system_error>
#include <type_traits>

#include "quark/handle.hpp"

namespace quark::inline win32 {
namespace detail {
inline void close_handle(HANDLE h) noexcept {
  if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
};
}  // namespace detail

using Handle = UniqueHandle<HANDLE, detail::close_handle>;
using Hook = UniqueHandle<HHOOK, UnhookWindowsHookEx>;
using LocalMemory = UniqueHandle<HLOCAL, LocalFree>;

/**
 * @brief Win32のシステムエラーを表す例外クラス
 */
class Win32Error : public std::system_error {
public:
  explicit Win32Error(const char* what)
    : system_error{HRESULT_FROM_WIN32(GetLastError()), std::system_category(), what} {}

  explicit Win32Error(DWORD ev, const char* what)
    : system_error{HRESULT_FROM_WIN32(ev), std::system_category(), what} {}
};

/**
 * @brief モジュール自身のファイルパスを取得する
 */
inline std::wstring get_current_module_path() {
  constexpr size_t MAX_PATH_LENGTH = 32768;
  std::wstring out;
  out.resize_and_overwrite(MAX_PATH_LENGTH, [](wchar_t* data, size_t size) {
    const auto ret = GetModuleFileNameW(nullptr, data, static_cast<DWORD>(size));
    if (ret == 0 || ret >= size) throw Win32Error("Failed to GetModuleFileName");
    return ret;
  });
  return out;
}
}  // namespace quark::inline win32
