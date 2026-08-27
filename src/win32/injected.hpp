// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file injected.hpp
 * @brief 自前のINJECTEDフラグ
 */
#pragma once

#include <Windows.h>

namespace quark::inline win32 {
/**
 * @brief EXTRA_INFO_INJECTEDの値をメモリアドレスで代用するためのオブジェクト
 */
extern volatile int extra_info_injected;

/**
 * @brief 自前のINJECTEDフラグ
 *
 * IMEの状態遷移を含むキー入力をSendInputすると、INJECTEDフラグを持たない補助的なキー入力を発生させることがある。それらはフックプロシージャ内で区別できないので、このフラグを使って無理矢理に識別できるようにする。
 */
inline const ULONG_PTR EXTRA_INFO_INJECTED = reinterpret_cast<ULONG_PTR>(&extra_info_injected);

/**
 * @brief INJECTEDフラグを追加する
 *
 * @param ki キー入力情報
 */
inline void add_injected(KEYBDINPUT& ki) noexcept {
  // HACK: 用途外の方法で自前のINJECTEDフラグを立てる
  ki.dwExtraInfo = EXTRA_INFO_INJECTED;
}

/**
 * @brief INJECTEDフラグを追加する
 *
 * @param mi マウス入力情報
 */
inline void add_injected(MOUSEINPUT& mi) noexcept {
  // HACK: 用途外の方法で自前のINJECTEDフラグを立てる
  mi.dwExtraInfo = EXTRA_INFO_INJECTED;
}

/**
 * @brief INJECTEDフラグを取り除いて無害化する
 *
 * @param info キー入力イベント
 * @return true 除去に成功
 * @return false INJECTEDされていなかった
 */
inline bool remove_injected(KBDLLHOOKSTRUCT& info) noexcept {
  if (info.dwExtraInfo != EXTRA_INFO_INJECTED) return false;
  info.flags |= LLKHF_INJECTED;
  info.dwExtraInfo = 0;
  return true;
}
}  // namespace quark::inline win32
