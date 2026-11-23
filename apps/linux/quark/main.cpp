// SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief バックエンドアプリケーション
 */

#include <linux/limits.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <optional>

#include "quark/engine.hpp"
#include "quark/linux/fd/base.hpp"
#include "quark/linux/result.hpp"
#include "quark/logging.hpp"
#include "quark/result.hpp"

namespace quark::inline evdev {
namespace {
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
    return sigfd;
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

/**
 * @brief コマンドオプションを処理するクラス
 */
struct ArgList {
  explicit ArgList(std::span<const std::string_view> args) {
    for (const auto& arg : args) {
      if (arg.empty()) continue;
      if (arg == "--log-level=info") log_level = LogSeverity::INFO;
      if (arg == "--log-file") log_file = true;
    }
  }

  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  LogSeverity log_level = DEFAULT_LOG_LEVEL;
  bool log_file{};
  // NOLINTEND(misc-non-private-member-variables-in-classes)
};

/**
 * @brief 現在のプロセスの実行ファイルへのパスを取得する
 *
 * @return std::filesystem::path
 */
std::filesystem::path get_current_module_path() {
  std::array<char, PATH_MAX> buffer{};
  const ssize_t ret = readlink("/proc/self/exe", buffer.data(), buffer.size());
  if (ret <= 0) throw std::runtime_error("Unknown module path");
  const auto size = static_cast<size_t>(ret);
  if (size >= buffer.size()) throw std::runtime_error("Unknown module path");
  buffer.at(size) = '\0';
  return buffer.data();
}
}  // namespace
}  // namespace quark::inline evdev

/**
 * @brief メイン関数
 */
int main(int argc, char* argv[]) {
  using namespace quark;
  try {
    Logger::set_module("main");

// ここだけはどうしようもないので警告をオフにする
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    // argvを安全なコンテナにラップする
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic,clang-analyzer-security.ArrayBound)
    const std::vector<std::string_view> args{&argv[1], &argv[argc]};
#ifdef __clang__
#pragma clang diagnostic pop
#endif

    // コマンド引数
    const ArgList arg_list{args};

    // ロガー
    const std::filesystem::path log_path = get_current_module_path().parent_path() / "quark.log";
    const Logger logger{arg_list.log_level, arg_list.log_file ? log_path : std::filesystem::path{}};

    // シグナルハンドラ
    const auto sigfd = SignalFd::create().or_throw("Failed to create a signal FD").value();

    // キーボードエンジン
    Engine engine{[] { kill(getpid(), SIGTERM); }};
    engine.start();

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

    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    QUARK_LOG_FATAL("Main thread crashed: {}", e.what());
    return EXIT_FAILURE;
  } catch (...) {
    QUARK_LOG_FATAL("Main thread crashed: Unknown error");
    return EXIT_FAILURE;
  }
}
