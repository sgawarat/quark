// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file source.cpp
 * @brief Win32におけるSource処理
 */
#include "quark/win32/source.hpp"

#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <stdexcept>

#include "quark/engine.hpp"
#include "quark/event.hpp"
#include "quark/logging.hpp"
#include "quark/quantum/key.hpp"
#include "quark/win32/injected.hpp"
#include "quark/win32/key.hpp"

namespace quark::inline win32 {
namespace {
// TODO: グローバル変数を使用しない書き方にする
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<SourceContext*> s_current_host{};        ///< フックを使っているホスト
std::exception_ptr s_exception_ptr_from_hookproc{};  ///< hookprocから投げられた例外のポインタ
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

constexpr uint16_t VSC_E0 = 0xe000;
constexpr uint16_t VSC_E0_RIGHT_SHIFT{VSC_E0 | +Key::RIGHT_SHIFT};
constexpr uint16_t VSC_E1_PAUSE = 0xe11d;

KeyCode to_keycode(const KBDLLHOOKSTRUCT& ev) noexcept {
  uint16_t vsc = 0;
  if (ev.scanCode == 0) {
    vsc = static_cast<uint16_t>(MapVirtualKey(ev.vkCode, MAPVK_VK_TO_VSC_EX));
    // MapVirtualKeyはVK_PAUSEに例外的な値を返すので特別扱いする
    if (vsc == VSC_E1_PAUSE) return KC_PAUSE;
  } else {
    const auto code = static_cast<uint16_t>(ev.scanCode & 0xff);
    const bool extended = ((ev.flags & LLKHF_EXTENDED) != 0) || (ev.scanCode > 0xff);
    vsc = code | (extended ? VSC_E0 : 0);
  }

  // バックスラッシュ（VK_OEM_5）でないBACKSLASHキーはNONUS_HASHに相当すると仮定してキーコードを割り当てる
  // TODO: BACKSLASHとNONUS_HASHの対応関係が正しいか検証する
  if (vsc == +Key::BACKSLASH && ev.vkCode != VK_OEM_5) {
    return KC_NONUS_HASH;
  }

  // 右シフトがE0で来ることがあるので、通常の右シフトと統合する
  if (vsc == VSC_E0_RIGHT_SHIFT) return KC_RIGHT_SHIFT;

  // SysRqはAlt+PrtScなのでPrtScキーとして扱う
  if (vsc == +Key::SYSTEM_REQUEST) return KC_PRINT_SCREEN;

  // BreakはCtrl+PauseなのでPauseキーとして扱う
  if (vsc == +Key::CANCEL) return KC_PAUSE;

  // それ以外は対応表で変換する
  return to_keycode(Key{vsc});
}

/**
 * @brief 最後に押したキーをフィルタリングする
 */
constexpr KeyCode filter_last_key(DWORD vkcode, KeyCode keycode) noexcept {
  // 半角全角・英数・かなキーは離したときにイベントを寄越さずキーリピートの終わりを特定できないので、押してすぐ離したことにする
  if ((vkcode == VK_OEM_AUTO || vkcode == VK_OEM_ENLW || vkcode == VK_OEM_COPY ||
          vkcode == VK_OEM_ATTN) &&
      (keycode == KC_GRAVE || keycode == KC_CAPS_LOCK || keycode == KC_LANGUAGE_2)) {
    return KC_NO;
  }
  return keycode;
}
}  // namespace

SourceContext::SourceContext(std::shared_ptr<KeyboardEventQueue> producer,
    std::shared_ptr<SourceEventQueue> consumer, const SourceContextDesc&)
  : producer_{std::move(producer)}, consumer_{std::move(consumer)} {
  if (!producer_) throw std::invalid_argument("Invalid KeyboardEventQueue");
  if (!consumer_) throw std::invalid_argument("Invalid SourceEventQueue");

  try {
    // フックの使用許可を得る
    SourceContext* prev_host{nullptr};
    if (!s_current_host.compare_exchange_strong(prev_host, this)) {
      throw std::logic_error("WH_KEYBOAD_LL is already in use");
    }

    // フック開始
    s_exception_ptr_from_hookproc = {};
    hook_ = Hook{
      SetWindowsHookEx(
          WH_KEYBOARD_LL,
          // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
          [](int code, WPARAM wparam, LPARAM lparam) noexcept -> LRESULT {
            try {
              auto* const current_host = s_current_host.load(std::memory_order_relaxed);
              if (!current_host) throw std::logic_error{"Invalid host of hookproc"};
              return current_host->hookproc(code, wparam, lparam);
            } catch (...) {
              s_exception_ptr_from_hookproc = std::current_exception();
              PostQuitMessage(EXIT_FAILURE);
            }
            return CallNextHookEx(nullptr, code, wparam, lparam);
          },
          GetModuleHandle(nullptr), 0),
    };
    if (!hook_) throw std::runtime_error("Failed to SetWindowsHookEx");
  } catch (...) {
    // Quantumの使用権を返却する
    SourceContext* prev_host{this};
    s_current_host.compare_exchange_strong(prev_host, nullptr);
    throw;
  }
}

SourceContext::~SourceContext() noexcept {
  // Quantumの使用権を返却する
  if (s_current_host != this) return;
  s_current_host.store(nullptr);
}

void SourceContext::run(const std::stop_token& stop_token) {
  const DWORD thread_id = GetCurrentThreadId();
  const std::stop_callback _{
    stop_token, [=] { PostThreadMessage(thread_id, WM_QUIT, EXIT_SUCCESS, 0); }};

  MSG msg{};
  const std::array handles{
    consumer_->handle(),
  };
  while (true) {
    const auto ret =
        MsgWaitForMultipleObjects(handles.size(), handles.data(), FALSE, INFINITE, QS_ALLINPUT);
    if (ret == WAIT_OBJECT_0) {
      // イベントが届いた場合
      const auto packet_opt = consumer_->pop();
      if (packet_opt) {
        packet_opt->visit([this](const auto& time, const auto& event) { update(time, event); });
      }
    } else if (ret == WAIT_OBJECT_0 + handles.size()) {
      // メッセージが届いた場合
      const BOOL result = GetMessage(&msg, nullptr, 0, 0);
      if (result < 0) throw std::runtime_error("Failed to GetMessage");

      if (result == 0) {  // WM_QUITを受け取った
        if (s_exception_ptr_from_hookproc) {
          std::rethrow_exception(s_exception_ptr_from_hookproc);
        }
        break;
      }
    } else {
      throw std::runtime_error("Failed to MsgWaitForMultipleObjects");
    }
  }
}

/**
 * @brief フックプロシージャ
 *
 * @note フック処理には制限時間が設けられているので、正常フローで重い処理を行わないこと。
 */
LRESULT SourceContext::hookproc(int code, WPARAM wparam, LPARAM lparam) {
  switch (code) {
    case HC_ACTION: {
      // NOLINTNEXTLINE(*-type-reinterpret-cast,*-no-int-to-ptr)
      const auto* const info_ptr = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
      if (!info_ptr) break;

      // 自身に由来するキーイベントを素通りさせる
      if (info_ptr->dwExtraInfo == EXTRA_INFO_INJECTED) break;

      // 仮想キーボードが処理できないキーは素通りさせる
      const KeyCode keycode = to_keycode(*info_ptr);
      if (keycode == KC_NO) {
        last_key_ = KC_NO;
        break;
      }

      // 入力イベントを仮想キーボードへ横流しする
      if ((info_ptr->flags & LLKHF_UP) != 0) {
        QUARK_LOG_DEBUG("Source: release {}", +keycode);
        producer_->emplace<KeyReleaseEvent>(keycode);
        last_key_ = KC_NO;
      } else {
        // 初回のキー押下のみを仮想キーボードに送る
        if (keycode != last_key_) {
          QUARK_LOG_DEBUG("Source: press {}", +keycode);
          producer_->emplace<KeyPressEvent>(keycode);
          last_key_ = filter_last_key(info_ptr->vkCode, keycode);
        }
      }
      return TRUE;
    }
  }
  // 素通りさせたキー入力を下流に流す
  return CallNextHookEx(nullptr, code, wparam, lparam);
}

void SourceContext::update(SourceEventTime, TickEvent) {}

void SourceContext::update(SourceEventTime, EngineSignal signal) {
  switch (signal) {
    case EngineSignal::SCREEN_LOCKED: {
      break;
    }
    case EngineSignal::SCREEN_UNLOCKED: {
      QUARK_LOG_DEBUG("Source: SCREEN_UNLOCKED signal");
      last_key_ = KC_NO;
      break;
    }
  }
}
}  // namespace quark::inline win32
