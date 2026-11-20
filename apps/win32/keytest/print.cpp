// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file print.cpp
 * @brief print関数
 */
#include "print.hpp"

#include <Windows.h>

#include <array>
#include <format>
#include <iostream>

namespace quark::keytest::inline win32 {
namespace {
constexpr size_t MAX_KEY_NAME_LENGTH = 256;

std::wstring_view get_vk_name(DWORD vk) noexcept {
  switch (vk) {
    case VK_LBUTTON: return L"VK_LBUTTON";
    case VK_RBUTTON: return L"VK_RBUTTON";
    case VK_CANCEL: return L"VK_CANCEL";
    case VK_MBUTTON: return L"VK_MBUTTON";
    case VK_XBUTTON1: return L"VK_XBUTTON1";
    case VK_XBUTTON2: return L"VK_XBUTTON2";
    case VK_BACK: return L"VK_BACK";
    case VK_TAB: return L"VK_TAB";
    case VK_CLEAR: return L"VK_CLEAR";
    case VK_RETURN: return L"VK_RETURN";
    case VK_SHIFT: return L"VK_SHIFT";
    case VK_CONTROL: return L"VK_CONTROL";
    case VK_MENU: return L"VK_MENU";
    case VK_PAUSE: return L"VK_PAUSE";
    case VK_CAPITAL: return L"VK_CAPITAL";
    case VK_KANA: return L"VK_KANA";
    // case VK_HANGUL: return L"VK_HANGUL";
    case VK_IME_ON: return L"VK_IME_ON";
    case VK_JUNJA: return L"VK_JUNJA";
    case VK_FINAL: return L"VK_FINAL";
    // case VK_HANJA: return L"VK_HANJA";
    case VK_KANJI: return L"VK_KANJI";
    case VK_IME_OFF: return L"VK_IME_OFF";
    case VK_ESCAPE: return L"VK_ESCAPE";
    case VK_CONVERT: return L"VK_CONVERT";
    case VK_NONCONVERT: return L"VK_NONCONVERT";
    case VK_ACCEPT: return L"VK_ACCEPT";
    case VK_MODECHANGE: return L"VK_MODECHANGE";
    case VK_SPACE: return L"VK_SPACE";
    case VK_PRIOR: return L"VK_PRIOR";
    case VK_NEXT: return L"VK_NEXT";
    case VK_END: return L"VK_END";
    case VK_HOME: return L"VK_HOME";
    case VK_LEFT: return L"VK_LEFT";
    case VK_UP: return L"VK_UP";
    case VK_RIGHT: return L"VK_RIGHT";
    case VK_DOWN: return L"VK_DOWN";
    case VK_SELECT: return L"VK_SELECT";
    case VK_PRINT: return L"VK_PRINT";
    case VK_EXECUTE: return L"VK_EXECUTE";
    case VK_SNAPSHOT: return L"VK_SNAPSHOT";
    case VK_INSERT: return L"VK_INSERT";
    case VK_DELETE: return L"VK_DELETE";
    case VK_HELP: return L"VK_HELP";
    case '0': return L"0";
    case '1': return L"1";
    case '2': return L"2";
    case '3': return L"3";
    case '4': return L"4";
    case '5': return L"5";
    case '6': return L"6";
    case '7': return L"7";
    case '8': return L"8";
    case '9': return L"9";
    case 'A': return L"A";
    case 'B': return L"B";
    case 'C': return L"C";
    case 'D': return L"D";
    case 'E': return L"E";
    case 'F': return L"F";
    case 'G': return L"G";
    case 'H': return L"H";
    case 'I': return L"I";
    case 'J': return L"J";
    case 'K': return L"K";
    case 'L': return L"L";
    case 'M': return L"M";
    case 'N': return L"N";
    case 'O': return L"O";
    case 'P': return L"P";
    case 'Q': return L"Q";
    case 'R': return L"R";
    case 'S': return L"S";
    case 'T': return L"T";
    case 'U': return L"U";
    case 'V': return L"V";
    case 'W': return L"W";
    case 'X': return L"X";
    case 'Y': return L"Y";
    case 'Z': return L"Z";
    case VK_LWIN: return L"VK_LWIN";
    case VK_RWIN: return L"VK_RWIN";
    case VK_APPS: return L"VK_APPS";
    case VK_SLEEP: return L"VK_SLEEP";
    case VK_NUMPAD0: return L"VK_NUMPAD0";
    case VK_NUMPAD1: return L"VK_NUMPAD1";
    case VK_NUMPAD2: return L"VK_NUMPAD2";
    case VK_NUMPAD3: return L"VK_NUMPAD3";
    case VK_NUMPAD4: return L"VK_NUMPAD4";
    case VK_NUMPAD5: return L"VK_NUMPAD5";
    case VK_NUMPAD6: return L"VK_NUMPAD6";
    case VK_NUMPAD7: return L"VK_NUMPAD7";
    case VK_NUMPAD8: return L"VK_NUMPAD8";
    case VK_NUMPAD9: return L"VK_NUMPAD9";
    case VK_MULTIPLY: return L"VK_MULTIPLY";
    case VK_ADD: return L"VK_ADD";
    case VK_SEPARATOR: return L"VK_SEPARATOR";
    case VK_SUBTRACT: return L"VK_SUBTRACT";
    case VK_DECIMAL: return L"VK_DECIMAL";
    case VK_DIVIDE: return L"VK_DIVIDE";
    case VK_F1: return L"VK_F1";
    case VK_F2: return L"VK_F2";
    case VK_F3: return L"VK_F3";
    case VK_F4: return L"VK_F4";
    case VK_F5: return L"VK_F5";
    case VK_F6: return L"VK_F6";
    case VK_F7: return L"VK_F7";
    case VK_F8: return L"VK_F8";
    case VK_F9: return L"VK_F9";
    case VK_F10: return L"VK_F10";
    case VK_F11: return L"VK_F11";
    case VK_F12: return L"VK_F12";
    case VK_F13: return L"VK_F13";
    case VK_F14: return L"VK_F14";
    case VK_F15: return L"VK_F15";
    case VK_F16: return L"VK_F16";
    case VK_F17: return L"VK_F17";
    case VK_F18: return L"VK_F18";
    case VK_F19: return L"VK_F19";
    case VK_F20: return L"VK_F20";
    case VK_F21: return L"VK_F21";
    case VK_F22: return L"VK_F22";
    case VK_F23: return L"VK_F23";
    case VK_F24: return L"VK_F24";
    case VK_NAVIGATION_VIEW: return L"VK_NAVIGATION_VIEW";
    case VK_NAVIGATION_MENU: return L"VK_NAVIGATION_MENU";
    case VK_NAVIGATION_UP: return L"VK_NAVIGATION_UP";
    case VK_NAVIGATION_DOWN: return L"VK_NAVIGATION_DOWN";
    case VK_NAVIGATION_LEFT: return L"VK_NAVIGATION_LEFT";
    case VK_NAVIGATION_RIGHT: return L"VK_NAVIGATION_RIGHT";
    case VK_NAVIGATION_ACCEPT: return L"VK_NAVIGATION_ACCEPT";
    case VK_NAVIGATION_CANCEL: return L"VK_NAVIGATION_CANCEL";
    case VK_NUMLOCK: return L"VK_NUMLOCK";
    case VK_SCROLL: return L"VK_SCROLL";
    case VK_OEM_NEC_EQUAL: return L"VK_OEM_NEC_EQUAL";
    // case VK_OEM_FJ_JISHO: return L"VK_OEM_FJ_JISHO";
    case VK_OEM_FJ_MASSHOU: return L"VK_OEM_FJ_MASSHOU";
    case VK_OEM_FJ_TOUROKU: return L"VK_OEM_FJ_TOUROKU";
    case VK_OEM_FJ_LOYA: return L"VK_OEM_FJ_LOYA";
    case VK_OEM_FJ_ROYA: return L"VK_OEM_FJ_ROYA";
    case VK_LSHIFT: return L"VK_LSHIFT";
    case VK_RSHIFT: return L"VK_RSHIFT";
    case VK_LCONTROL: return L"VK_LCONTROL";
    case VK_RCONTROL: return L"VK_RCONTROL";
    case VK_LMENU: return L"VK_LMENU";
    case VK_RMENU: return L"VK_RMENU";
    case VK_BROWSER_BACK: return L"VK_BROWSER_BACK";
    case VK_BROWSER_FORWARD: return L"VK_BROWSER_FORWARD";
    case VK_BROWSER_REFRESH: return L"VK_BROWSER_REFRESH";
    case VK_BROWSER_STOP: return L"VK_BROWSER_STOP";
    case VK_BROWSER_SEARCH: return L"VK_BROWSER_SEARCH";
    case VK_BROWSER_FAVORITES: return L"VK_BROWSER_FAVORITES";
    case VK_BROWSER_HOME: return L"VK_BROWSER_HOME";
    case VK_VOLUME_MUTE: return L"VK_VOLUME_MUTE";
    case VK_VOLUME_DOWN: return L"VK_VOLUME_DOWN";
    case VK_VOLUME_UP: return L"VK_VOLUME_UP";
    case VK_MEDIA_NEXT_TRACK: return L"VK_MEDIA_NEXT_TRACK";
    case VK_MEDIA_PREV_TRACK: return L"VK_MEDIA_PREV_TRACK";
    case VK_MEDIA_STOP: return L"VK_MEDIA_STOP";
    case VK_MEDIA_PLAY_PAUSE: return L"VK_MEDIA_PLAY_PAUSE";
    case VK_LAUNCH_MAIL: return L"VK_LAUNCH_MAIL";
    case VK_LAUNCH_MEDIA_SELECT: return L"VK_LAUNCH_MEDIA_SELECT";
    case VK_LAUNCH_APP1: return L"VK_LAUNCH_APP1";
    case VK_LAUNCH_APP2: return L"VK_LAUNCH_APP2";
    case VK_OEM_1: return L"VK_OEM_1";
    case VK_OEM_PLUS: return L"VK_OEM_PLUS";
    case VK_OEM_COMMA: return L"VK_OEM_COMMA";
    case VK_OEM_MINUS: return L"VK_OEM_MINUS";
    case VK_OEM_PERIOD: return L"VK_OEM_PERIOD";
    case VK_OEM_2: return L"VK_OEM_2";
    case VK_OEM_3: return L"VK_OEM_3";
    case VK_GAMEPAD_A: return L"VK_GAMEPAD_A";
    case VK_GAMEPAD_B: return L"VK_GAMEPAD_B";
    case VK_GAMEPAD_X: return L"VK_GAMEPAD_X";
    case VK_GAMEPAD_Y: return L"VK_GAMEPAD_Y";
    case VK_GAMEPAD_RIGHT_SHOULDER: return L"VK_GAMEPAD_RIGHT_SHOULDER";
    case VK_GAMEPAD_LEFT_SHOULDER: return L"VK_GAMEPAD_LEFT_SHOULDER";
    case VK_GAMEPAD_LEFT_TRIGGER: return L"VK_GAMEPAD_LEFT_TRIGGER";
    case VK_GAMEPAD_RIGHT_TRIGGER: return L"VK_GAMEPAD_RIGHT_TRIGGER";
    case VK_GAMEPAD_DPAD_UP: return L"VK_GAMEPAD_DPAD_UP";
    case VK_GAMEPAD_DPAD_DOWN: return L"VK_GAMEPAD_DPAD_DOWN";
    case VK_GAMEPAD_DPAD_LEFT: return L"VK_GAMEPAD_DPAD_LEFT";
    case VK_GAMEPAD_DPAD_RIGHT: return L"VK_GAMEPAD_DPAD_RIGHT";
    case VK_GAMEPAD_MENU: return L"VK_GAMEPAD_MENU";
    case VK_GAMEPAD_VIEW: return L"VK_GAMEPAD_VIEW";
    case VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON: return L"VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON";
    case VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON: return L"VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON";
    case VK_GAMEPAD_LEFT_THUMBSTICK_UP: return L"VK_GAMEPAD_LEFT_THUMBSTICK_UP";
    case VK_GAMEPAD_LEFT_THUMBSTICK_DOWN: return L"VK_GAMEPAD_LEFT_THUMBSTICK_DOWN";
    case VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT: return L"VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT";
    case VK_GAMEPAD_LEFT_THUMBSTICK_LEFT: return L"VK_GAMEPAD_LEFT_THUMBSTICK_LEFT";
    case VK_GAMEPAD_RIGHT_THUMBSTICK_UP: return L"VK_GAMEPAD_RIGHT_THUMBSTICK_UP";
    case VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN: return L"VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN";
    case VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT: return L"VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT";
    case VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT: return L"VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT";
    case VK_OEM_4: return L"VK_OEM_4";
    case VK_OEM_5: return L"VK_OEM_5";
    case VK_OEM_6: return L"VK_OEM_6";
    case VK_OEM_7: return L"VK_OEM_7";
    case VK_OEM_8: return L"VK_OEM_8";
    case VK_OEM_AX: return L"VK_OEM_AX";
    case VK_OEM_102: return L"VK_OEM_102";
    case VK_ICO_HELP: return L"VK_ICO_HELP";
    case VK_ICO_00: return L"VK_ICO_00";
    case VK_PROCESSKEY: return L"VK_PROCESSKEY";
    case VK_ICO_CLEAR: return L"VK_ICO_CLEAR";
    case VK_PACKET: return L"VK_PACKET";
    case VK_OEM_RESET: return L"VK_OEM_RESET";
    case VK_OEM_JUMP: return L"VK_OEM_JUMP";
    case VK_OEM_PA1: return L"VK_OEM_PA1";
    case VK_OEM_PA2: return L"VK_OEM_PA2";
    case VK_OEM_PA3: return L"VK_OEM_PA3";
    case VK_OEM_WSCTRL: return L"VK_OEM_WSCTRL";
    case VK_OEM_CUSEL: return L"VK_OEM_CUSEL";
    case VK_OEM_ATTN: return L"VK_OEM_ATTN";
    case VK_OEM_FINISH: return L"VK_OEM_FINISH";
    case VK_OEM_COPY: return L"VK_OEM_COPY";
    case VK_OEM_AUTO: return L"VK_OEM_AUTO";
    case VK_OEM_ENLW: return L"VK_OEM_ENLW";
    case VK_OEM_BACKTAB: return L"VK_OEM_BACKTAB";
    case VK_ATTN: return L"VK_ATTN";
    case VK_CRSEL: return L"VK_CRSEL";
    case VK_EXSEL: return L"VK_EXSEL";
    case VK_EREOF: return L"VK_EREOF";
    case VK_PLAY: return L"VK_PLAY";
    case VK_ZOOM: return L"VK_ZOOM";
    case VK_NONAME: return L"VK_NONAME";
    case VK_PA1: return L"VK_PA1";
    case VK_OEM_CLEAR: return L"VK_OEM_CLEAR";
    default: break;
  }
  return L"Unknown";
}

std::wstring_view get_sc_name(DWORD scan, bool extended = false) noexcept {
  static std::array<wchar_t, MAX_KEY_NAME_LENGTH> buffer{};
  extended = extended || (scan > UINT8_MAX);
  const LONG lparam = static_cast<LONG>((scan << 16) | (extended ? (1 << 24) : 0));
  const auto size = GetKeyNameTextW(lparam, buffer.data(), buffer.size());
  if (size <= 0) return L"Unknown";
  return buffer.data();
}
}  // namespace

void print_key_event(DWORD vk, DWORD scan) {
  const auto vk_name = get_vk_name(vk);
  const auto sc_name = get_sc_name(scan);
  std::wcout << std::format(L"vk:{}({:#04x}), sc:{}({:#06x})\n", vk_name, vk, sc_name, scan);
}

void print_key_event(const KBDLLHOOKSTRUCT& info) {
  const auto vk_name = get_vk_name(info.vkCode);
  const auto sc_name = get_sc_name(info.scanCode, (info.flags & LLKHF_EXTENDED) != 0);
  std::wcout << std::format(L"time:{}, vk:{}({:#04x}), sc:{}({:#04x})", info.time, vk_name,
      info.vkCode, sc_name, info.scanCode);
  if (info.flags != 0) {
    std::wcout << L", flags:";
    if ((info.flags & LLKHF_EXTENDED) != 0) std::wcout << L"X";
    if ((info.flags & LLKHF_LOWER_IL_INJECTED) != 0) std::wcout << L"L";
    if ((info.flags & LLKHF_INJECTED) != 0) std::wcout << L"I";
    if ((info.flags & LLKHF_ALTDOWN) != 0) std::wcout << L"A";
    if ((info.flags & LLKHF_UP) != 0) std::wcout << L"U";
  }
  std::wcout << L'\n';
}
}  // namespace quark::keytest::inline win32
