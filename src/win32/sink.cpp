// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file sink.cpp
 * @brief Win32のSink処理
 */
#include <quark/win32/sink.hpp>

extern "C" {
#include <report.h>
}  // extern "C"

#include <quark/logging.hpp>
#include <quark/win32/injected.hpp>
#include <quark/win32/scancode.hpp>

namespace quark::inline win32 {
namespace {
/**
 * @brief INPUT_KEYBOARDのためのINPUTのラッパークラス
 */
struct KeyInput final : INPUT {
  /**
   * @brief スキャンコードで初期化する
   */
  explicit KeyInput(Scancode scancode) noexcept
    : INPUT{
        .type = INPUT_KEYBOARD,
        .ki{
          .wScan = static_cast<uint8_t>(scancode),
          .dwFlags =
              static_cast<uint16_t>(scancode) > UINT8_MAX ? DWORD{KEYEVENTF_EXTENDEDKEY} : DWORD{0},
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      } {
    using enum Scancode;

    // Pauseは正確なスキャンコードが分からないためVKで送る
    if (scancode == SC_PAUSE) {
      ki.wVk = VK_PAUSE;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    } else if (scancode == SC_NUM_LOCK) {
      ki.wVk = VK_NUMLOCK;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    } else {
      ki.dwFlags |= KEYEVENTF_SCANCODE;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    }
  }

  /**
   * @brief キーコードで初期化する
   */
  explicit KeyInput(uint8_t keycode) noexcept : KeyInput{to_scancode(keycode)} {}

  /**
   * @brief Generic Desktop PageのUsage IDで初期化する
   */
  explicit KeyInput(desktop_usages usage) noexcept : KeyInput{to_scancode(usage)} {}

  /**
   * @brief Consumer PageのUsage IDで初期化する
   */
  explicit KeyInput(consumer_usages usage) noexcept : KeyInput{to_scancode(usage)} {}

  /**
   * @brief キーを押すイベントを送信する
   */
  KeyInput& press() noexcept {
    if (ki.wScan != 0) {                      // NOLINT(cppcoreguidelines-pro-type-union-access)
      ki.dwFlags &= ~DWORD{KEYEVENTF_KEYUP};  // NOLINT(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }
    return *this;
  }

  /**
   * @brief キーを離すイベントを送信する
   */
  KeyInput& release() noexcept {
    if (ki.wScan != 0) {              // NOLINT(cppcoreguidelines-pro-type-union-access)
      ki.dwFlags |= KEYEVENTF_KEYUP;  // NOLINT(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }
    return *this;
  }

  /**
   * @brief キーを押して離すイベントを送信する
   */
  KeyInput& tap() noexcept {
    return press().release();
  }
};

/**
 * @brief INPUT_MOUSEのためのINPUTのラッパークラス
 */
struct MouseInput final : INPUT {
  static constexpr uint8_t MOUSE_BTN_MASK_123 = MOUSE_BTN1 | MOUSE_BTN2 | MOUSE_BTN3;
  static constexpr uint8_t MOUSE_BTN_MASK_678 = MOUSE_BTN6 | MOUSE_BTN7 | MOUSE_BTN8;

  MouseInput() noexcept
    : INPUT{
        .type = INPUT_MOUSE,
        .mi{
          .dwExtraInfo = EXTRA_INFO_INJECTED,
        },
      } {}

  /**
   * @brief ボタンを押すイベントを送信する
   */
  MouseInput& press(uint8_t buttons) noexcept {
    // 左右中央ボタン
    if ((buttons & MOUSE_BTN_MASK_123) != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = 0;
      if ((buttons & MOUSE_BTN1) != 0) mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
      if ((buttons & MOUSE_BTN2) != 0) mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
      if ((buttons & MOUSE_BTN3) != 0) mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // 戻るボタン
    if ((buttons & MOUSE_BTN4) != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = XBUTTON1;
      mi.dwFlags = MOUSEEVENTF_XDOWN;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // 進むボタン
    if ((buttons & MOUSE_BTN5) != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = XBUTTON2;
      mi.dwFlags = MOUSEEVENTF_XDOWN;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // ボタン6～8
    if ((buttons & MOUSE_BTN_MASK_678) != 0) mouse_button_6_7_8_not_supported();

    return *this;
  }

  /**
   * @brief ボタンを離すイベントを送信する
   */
  MouseInput& release(uint8_t buttons) noexcept {
    // 左右中央ボタン
    if ((buttons & MOUSE_BTN_MASK_123) != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = 0;
      if ((buttons & MOUSE_BTN1) != 0) mi.dwFlags |= MOUSEEVENTF_LEFTUP;
      if ((buttons & MOUSE_BTN2) != 0) mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
      if ((buttons & MOUSE_BTN3) != 0) mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // 戻るボタンを離す
    if ((buttons & MOUSE_BTN4) != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = XBUTTON1;
      mi.dwFlags = MOUSEEVENTF_XUP;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // 進むボタン
    if ((buttons & MOUSE_BTN5) != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = XBUTTON2;
      mi.dwFlags = MOUSEEVENTF_XUP;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // ボタン6～8
    if ((buttons & MOUSE_BTN_MASK_678) != 0) mouse_button_6_7_8_not_supported();

    return *this;
  }

  /**
   * @brief ホイールを動かすイベントを送信する
   */
  MouseInput& wheel(const report_mouse_t& report) noexcept {
    // ホイール縦
    if (report.v != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = static_cast<DWORD>(report.v * WHEEL_DELTA);
      mi.dwFlags = MOUSEEVENTF_WHEEL;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    // ホイール横
    if (report.h != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.mouseData = static_cast<DWORD>(report.h * WHEEL_DELTA);
      mi.dwFlags = MOUSEEVENTF_HWHEEL;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    return *this;
  }

  /**
   * @brief カーソルを動かすイベントを送信する
   */
  MouseInput& move(const report_mouse_t& report) noexcept {
    // カーソル
    if (report.x != 0 || report.y != 0) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
      mi.dx = report.x;
      mi.dy = report.y;
      mi.mouseData = 0;
      mi.dwFlags = MOUSEEVENTF_MOVE;
      // NOLINTEND(cppcoreguidelines-pro-type-union-access)
      SendInput(1, this, sizeof(*this));
    }

    return *this;
  }

private:
  static void mouse_button_6_7_8_not_supported() noexcept {
    static std::atomic<size_t> total_count{};
    if (const auto count = total_count++; count == 0) {
      warn_log("Mouse button 6,7,8 is not supported.");
    }
  }
};
}  // namespace

SinkQueue::SinkQueue() = default;

SinkQueue::~SinkQueue() noexcept = default;

void SinkQueue::push(const SinkEvent& event) {
  {
    const std::scoped_lock<std::mutex> lock{queue_mtx_};
    queue_.push_back(event);
  }
  queue_cv_.notify_one();
}

bool SinkQueue::pop(const std::stop_token& stop_token, SinkEvent& event) {
  std::unique_lock<std::mutex> lock{queue_mtx_};

  if (queue_.empty()) {
    //  イベントかないので、イベントか停止要求を待つ
    if (!queue_cv_.wait(lock, stop_token, [&] { return !queue_.empty(); })) {
      // イベントが来なかった
      return false;
    }
  }

  // イベントがあるので、それを返す
  event = queue_.front();
  queue_.pop_front();
  return true;
}

void SinkContext::run(const std::stop_token& stop_token) {
  SinkEvent event;
  while (queue_.pop(stop_token, event)) {
    std::visit([this](const auto& report) { update(report); }, event);
    std::this_thread::yield();
  }
}

void SinkContext::update(std::monostate) noexcept {}

void SinkContext::update(const report_keyboard_t& report) noexcept {
  const KeySet keys = [&] {
    KeySet out{};
    for (auto key : std::span(report.keys)) {
      if (key >= KC_LEFT_CTRL && key <= KC_RIGHT_GUI) continue;
      if (key != KC_NO) out.set(key);
    }
    return out;
  }();
  const ModKeySet mods{report.mods};

  prev_keys_.and_not(keys).scan([](size_t i) {  // 非修飾キーを離す
    KeyInput{static_cast<uint8_t>(i)}.release();
  });
  prev_mods_.and_not(mods).scan([](size_t i) {  // 修飾キーを離す
    KeyInput{static_cast<uint8_t>(KC_LEFT_CTRL + i)}.release();
  });
  mods.and_not(prev_mods_).scan([this](size_t i) {  // 修飾キーを押す
    last_input_ = KeyInput{static_cast<uint8_t>(KC_LEFT_CTRL + i)}.press();
  });
  keys.and_not(prev_keys_).scan([this](size_t i) {  // 非修飾キーを押す
    last_input_ = KeyInput{static_cast<uint8_t>(i)}.press();
  });

  prev_keys_ = keys;
  prev_mods_ = mods;
}

void SinkContext::update(const report_nkro_t& report) noexcept {
  const KeySet keys{std::span{report.bits}};  // 今回のキー状態
  const ModKeySet mods{report.mods};          // 今回の修飾キー状態

  prev_keys_.and_not(keys).scan([](size_t i) {  // 非修飾キーを離す
    KeyInput{static_cast<uint8_t>(i)}.release();
  });
  prev_mods_.and_not(mods).scan([](size_t i) {  // 修飾キーを離す
    KeyInput{static_cast<uint8_t>(KC_LEFT_CTRL + i)}.release();
  });
  mods.and_not(prev_mods_).scan([this](size_t i) {  // 修飾キーを押す
    last_input_ = KeyInput{static_cast<uint8_t>(KC_LEFT_CTRL + i)}.press();
  });
  keys.and_not(prev_keys_).scan([this](size_t i) {  // 非修飾キーを押す
    last_input_ = KeyInput{static_cast<uint8_t>(i)}.press();
  });

  prev_keys_ = keys;
  prev_mods_ = mods;
}

void SinkContext::update(const report_mouse_t& report) noexcept {
  MouseInput{}
      .release(prev_mouse_buttons_ & ~report.buttons)
      .press(~prev_mouse_buttons_ & report.buttons)
      .wheel(report)
      .move(report);
  prev_mouse_buttons_ = report.buttons;
}

void SinkContext::update(const report_extra_t& report) noexcept {
  switch (report.report_id) {
    case REPORT_ID_SYSTEM: {
      KeyInput{static_cast<desktop_usages>(report.usage)}.tap();
      break;
    }
    case REPORT_ID_CONSUMER: {
      KeyInput{static_cast<consumer_usages>(report.usage)}.tap();
      break;
    }
    default: break;
  }
}

void SinkContext::update(SinkSignal signal) noexcept {
  switch (signal) {
    case SinkSignal::KEY_REPEAT: {
      SendInput(1, &last_input_, sizeof(last_input_));
      break;
    }
    case SinkSignal::RESET: {
      prev_keys_.scan([](size_t i) {  // 非修飾キーを離す
        KeyInput{static_cast<uint8_t>(i)}.release();
      });
      prev_mods_.scan([](size_t i) {  // 修飾キーを離す
        KeyInput{static_cast<uint8_t>(KC_LEFT_CTRL + i)}.release();
      });
      MouseInput{}.release(prev_mouse_buttons_);
      prev_keys_ = {};
      prev_mods_ = {};
      prev_mouse_buttons_ = {};
      break;
    }
  }
}
}  // namespace quark::inline win32
