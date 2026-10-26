// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file quantum.cpp
 * @brief Quantum管理
 */
#include "quark/quantum/quantum.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <exception>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>

extern "C" {
#include <debounce.h>
#include <host.h>
#include <host_driver.h>
#include <keyboard.h>
#include <matrix.h>

extern matrix_row_t matrix[MATRIX_ROWS];
extern matrix_row_t raw_matrix[MATRIX_ROWS];
}  // extern "C"

#include "quark/quantum/key.hpp"

namespace quark {
namespace {
// TODO: グローバル変数を使用しない書き方にする
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<QuantumHost*> s_current_host_of_quantum{};  ///< Quantumを使用している現在のホスト
std::exception_ptr s_exception_ptr_from_quantum{};      ///< Quantum内部から投げられた例外のポインタ
host_driver_t s_host_driver{};                          ///< Quantumのホストドライバ
bool s_raw_matrix_changed{false};                       ///< 直前の更新からraw_matrixが変化したか
bool s_matrix_changed{false};                           ///< 直前の更新からmatrixが変化したか
const std::span s_matrix{matrix};
const std::span s_raw_matrix{raw_matrix};
std::array<matrix_row_t, MATRIX_ROWS> s_matrix_mask{};  ///< 入力成立後にmatrixから消すビットの列
bool s_debounce_disabled{false};                        ///< debounce処理を通るか
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace

QuantumHost::QuantumHost() {
  // QMKの使用許可を得る
  QuantumHost* prev_host{nullptr};
  if (!s_current_host_of_quantum.compare_exchange_strong(prev_host, this)) {
    throw std::logic_error("Quantum is already in use");
  }

  try {
    // QMKを初期化する
    keyboard_setup();
    keyboard_init();

    // ホストドライバをセットする
    constexpr auto send_report = [](auto* report_ptr) noexcept {
      // NOLINTNEXTLINE(misc-const-correctness)
      QuantumHost* const host = s_current_host_of_quantum.load(std::memory_order_acquire);
      if (!host || !report_ptr) return;
      try {
        host->send_report(*report_ptr);
      } catch (...) {
        s_exception_ptr_from_quantum = std::current_exception();
      }
    };
    s_host_driver = {
      .keyboard_leds = [] -> uint8_t { return 0; },
      .send_keyboard = send_report,
      .send_nkro = send_report,
      .send_mouse = send_report,
      .send_extra = send_report,
    };
    host_set_driver(&s_host_driver);
  } catch (...) {
    // Quantumの使用権を返却する
    QuantumHost* prev_host{this};
    s_current_host_of_quantum.compare_exchange_strong(prev_host, nullptr);
    throw;
  }
}

QuantumHost::~QuantumHost() noexcept {
  // Quantumの使用権を返却する
  if (s_current_host_of_quantum != this) return;
  host_set_driver(nullptr);
  s_host_driver = {};
  s_current_host_of_quantum.store(nullptr);
}

void QuantumHost::matrix_set(KeyCode keycode) {
  const auto keypos = get_key_property<keypos_t>(keycode);
  if (keypos.row >= MATRIX_ROWS || keypos.col >= MATRIX_COLS) return;

  // NOLINTNEXTLINE(*-constant-array-index,*-avoid-unchecked-container-access))
  s_raw_matrix[keypos.row] |= matrix_row_t{1} << keypos.col;
  s_raw_matrix_changed = true;

  // キーリリースが発生しないキーであれば、押下成立後にmatrixからビットを消すためのフラグを立てる
  if (get_key_property<NonHoldable>(keycode) == NonHoldable::YES) {
    s_matrix_mask.at(keypos.row) |= matrix_row_t{1} << keypos.col;
  }
}

void QuantumHost::matrix_reset(KeyCode keycode) {
  const auto keypos = get_key_property<keypos_t>(keycode);
  if (keypos.row >= MATRIX_ROWS || keypos.col >= MATRIX_COLS) return;

  // NOLINTNEXTLINE(*-constant-array-index,*-avoid-unchecked-container-access))
  s_raw_matrix[keypos.row] &= ~(matrix_row_t{1} << keypos.col);
  s_raw_matrix_changed = true;
}

void QuantumHost::matrix_clear() {
  for (auto& row : s_raw_matrix) {
    row = 0;
  }
  s_raw_matrix_changed = true;
}

void QuantumHost::keyboard_task() {
  ::keyboard_task();
  if (s_exception_ptr_from_quantum) {
    std::rethrow_exception(std::move(s_exception_ptr_from_quantum));
  }

  // 押下の成立したNonHoldableキーがあれば、それを離した状態に更新する
  if (s_matrix_changed) {
    s_matrix_changed = false;
    for (auto&& [row, raw, mask] : std::views::zip(s_matrix, s_raw_matrix, s_matrix_mask)) {
      const matrix_row_t masked_row = row & mask;
      if (masked_row != 0) {
        s_matrix_changed = true;
        row &= ~masked_row;
        raw &= ~masked_row;
        mask &= ~masked_row;
      }
    }
    if (s_matrix_changed) {
      s_debounce_disabled = true;
      ::keyboard_task();
      if (s_exception_ptr_from_quantum) {
        std::rethrow_exception(std::move(s_exception_ptr_from_quantum));
      }
    }
  }
}

extern "C" {
void matrix_init_custom(void) {
  s_matrix_mask = {};
  s_raw_matrix_changed = false;
  s_matrix_changed = false;
  s_debounce_disabled = false;
}

uint8_t matrix_scan(void) {
  if (s_debounce_disabled) {
    s_debounce_disabled = false;
  } else {
    const bool raw_matrix_changed = s_raw_matrix_changed;
    s_raw_matrix_changed = false;

#ifdef SPLIT_KEYBOARD
    // NOLINTNEXTLINE(*-array-to-pointer-decay)
    s_matrix_changed = debounce(raw_matrix, matrix + thisHand, raw_matrix_changed);
    s_matrix_changed || = matrix_post_scan();
#else
    // NOLINTNEXTLINE(*-array-to-pointer-decay)
    s_matrix_changed = debounce(raw_matrix, matrix, raw_matrix_changed);
    matrix_scan_kb();
#endif
  }

  return s_matrix_changed ? 1 : 0;
}
}  // extern "C"
}  // namespace quark
