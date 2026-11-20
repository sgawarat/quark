// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file injected.hpp
 * @brief 自前のINJECTEDフラグ
 */
#pragma once

#include <Windows.h>

namespace quark::inline win32 {
/**
 * @brief 自前のINJECTEDフラグ
 *
 * キーイベントが自身に由来するかをフックプロシージャ内で判別できるようにするため、この値をdwExtraInfoに格納する。
 *
 * @note IMEの状態遷移を伴うキー入力をSendInputで送るときに発生する補助的なキー入力は、LLKHF_INJECTEDフラグを持たないが、dwExtraInfoをそのまま持つので、これで判別できるようになる。
 */
extern const ULONG_PTR EXTRA_INFO_INJECTED;
}  // namespace quark::inline win32
