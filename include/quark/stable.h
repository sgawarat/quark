// SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file stable.h
 * @brief 事前コンパイルされるヘッダ
 */
#pragma once

#ifdef _WIN32
#include <windows.h>

// 名前衝突するマクロを退避させる
// TODO: Windows.hとQMKのマクロが衝突しないように、もっと穏便な方法で解決したい
static const unsigned int QUARK_WIN32_KEY_EVENT = KEY_EVENT;
static const unsigned long QUARK_WIN32_DELETE = DELETE;
static const unsigned int QUARK_WIN32_ERROR = ERROR;
#undef KEY_EVENT
#undef DELETE
#undef ERROR
#endif

#ifdef __linux__
#include <unistd.h>
#endif
