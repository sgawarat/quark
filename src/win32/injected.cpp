// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file injected.cpp
 * @brief 自前のINJECTEDフラグ
 */
#include <quark/win32/injected.hpp>

#include <bit>

namespace quark::inline win32 {
namespace {
/**
 * @brief EXTRA_INFO_INJECTEDの値にメモリアドレスを与えるためのダミー変数
 */
const char dummy = 0;
}  // namespace

const ULONG_PTR EXTRA_INFO_INJECTED = std::bit_cast<ULONG_PTR>(&dummy);
}  // namespace quark::inline win32
