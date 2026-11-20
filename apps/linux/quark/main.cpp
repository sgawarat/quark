// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief バックエンドアプリケーション
 */

#include <sys/signalfd.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <optional>
#include <utility>

#include "quark/linux/fd/base.hpp"
#include "quark/linux/result.hpp"
#include "quark/logging.hpp"
#include "quark/result.hpp"

namespace quark::inline evdev {
/**
 * @brief Signal FDを管理するためのクラス
 */
class SignalFd final : public FdBase {
public:
  static Result<SignalFd> create() noexcept {
    sigset_t sigset{};
    if (sigemptyset(&sigset) < 0 || sigaddset(&sigset, SIGINT) < 0 ||
        sigaddset(&sigset, SIGTERM) < 0 || sigaddset(&sigset, SIGHUP) < 0) {
      return make_error(EINVAL);
    }
    if (const auto ret = pthread_sigmask(SIG_BLOCK, &sigset, nullptr); ret != 0) {
      return make_error(ret);
    }
    QUARK_EVDEV_RESULT_TRY(SignalFd sigfd, signalfd(-1, &sigset, SFD_CLOEXEC));
    return std::move(sigfd);
  }

  SignalFd() noexcept = default;

  [[nodiscard]] Result<std::optional<signalfd_siginfo>> read() const noexcept {
    signalfd_siginfo siginfo{};
    const auto ret = ::read(handle(), &siginfo, sizeof(siginfo));
    if (ret < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) return std::nullopt;
      return make_error(errno);
    }
    if (ret != sizeof(siginfo)) return make_error(EIO);
    return std::optional{siginfo};
  }

private:
  explicit SignalFd(int fd) noexcept : FdBase{fd} {}
};

class App final {
public:
  int run() {
    // メッセージループ
    while (true) {
      // ブロッキングモードでシグナルを待つ
      auto siginfo_opt = sigfd.read().or_throw("Failed to read SignalFd").value();
      if (!siginfo_opt) continue;
      auto&& siginfo = *siginfo_opt;

      // 終了シグナルがきたら、正常に終了させる
      if (siginfo.ssi_signo == SIGINT || siginfo.ssi_signo == SIGTERM ||
          siginfo.ssi_signo == SIGHUP) {
        break;
      }
    }
  }
};
}  // namespace quark::inline evdev

/**
 * @brief メイン関数
 */
int main(int argc, char* argv[]) {
  using namespace quark;
  try {
    App app{argc, argv};
    return app.run();
  } catch (const std::exception& e) {
    QUARK_LOG_FATAL("Main thread crashed: {}", e.what());
    return EXIT_FAILURE;
  } catch (...) {
    QUARK_LOG_FATAL("Main thread crashed: Unknown error");
    return EXIT_FAILURE;
  }
}
