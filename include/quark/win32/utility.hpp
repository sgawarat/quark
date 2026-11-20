// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file utility.hpp
 * @brief 便利機能
 */
#pragma once

#include <Windows.h>

#include <memory>
#include <string>
#include <system_error>
#include <type_traits>

namespace quark::inline win32 {
/**
 * @brief Win32ハンドルを管理するためのunique_ptrラッパークラス
 *
 * @tparam T ハンドルを表すポインタ型
 * @tparam D ハンドルの破棄を定義する関数オブジェクト
 */
template <typename T, auto D>
  requires(std::is_pointer_v<T> && std::invocable<decltype(D), T>)
using UniqueHandle =
    std::unique_ptr<std::remove_pointer_t<T>, decltype([](T ptr) noexcept { D(ptr); })>;

using Handle = UniqueHandle<HANDLE, CloseHandle>;
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
