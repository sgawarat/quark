// SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.cpp
 * @brief Qtフロントエンド
 */

#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QProcess>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleHints>
#include <QSystemTrayIcon>

/**
 * @brief メイン関数
 */
int main(int argc, char* argv[]) {
  QApplication app{argc, argv};
  QApplication::setQuitOnLastWindowClosed(false);

  // バックエンドプログラムのパス
#ifdef NDEBUG
  // const auto child_program = QStandardPaths::findExecutable("quark");
  const auto child_program = QDir{QApplication::applicationDirPath()}.absoluteFilePath("quark");
#else
  const auto child_program =
      QDir{QApplication::applicationDirPath()}.absoluteFilePath("../backend/quark");
#endif

  // アイコン
  struct IconSet {
    QIcon on;
    QIcon off;
  };
  const IconSet light_icon_set{
    .on = QIcon{":/assets/keyboard.svg"},
    .off = QIcon{":/assets/keyboard-off.svg"},
  };
  const IconSet dark_icon_set{
    .on = QIcon{":/assets/dark/keyboard.svg"},
    .off = QIcon{":/assets/dark/keyboard-off.svg"},
  };
  bool dark = false;

  // コンテキストメニュー
  QMenu context_menu;
  QAction* start_stop_action = context_menu.addAction("Start");
  QObject::connect(context_menu.addAction("Quit"), &QAction::triggered, &app, &QApplication::quit);

  // システムトレイアイコン
  QSystemTrayIcon tray_icon;
  tray_icon.setToolTip("Quark");
  tray_icon.setContextMenu(&context_menu);

  // バックエンドプロセス
  QProcess child;

  // コンテキストメニューからバックエンドプロセスを開始・停止させる
  QObject::connect(start_stop_action, &QAction::triggered, &app, [&] {
    if (child.state() == QProcess::NotRunning) {
      child.start(child_program);
    } else {
      child.terminate();
      constexpr int msecs = 1000;
      if (!child.waitForFinished(msecs)) child.kill();
    }
  });

  // システムトレイアイコンの色味をシステムのカラースキームに追従させる
  QObject::connect(
      QApplication::styleHints(), &QStyleHints::colorSchemeChanged, [&](Qt::ColorScheme scheme) {
        dark = scheme == Qt::ColorScheme::Dark;
        const IconSet& icons = dark ? dark_icon_set : light_icon_set;
        if (child.state() == QProcess::NotRunning) {
          tray_icon.setIcon(icons.off);
        } else {
          tray_icon.setIcon(icons.on);
        }
      });

  // システムトレイアイコンとコンテキストメニューをプロセスの実行状態に追従させる
  QObject::connect(&child, &QProcess::started, &app, [&] {
    const IconSet& icons = dark ? dark_icon_set : light_icon_set;
    tray_icon.setIcon(icons.on);
    if (start_stop_action) start_stop_action->setText("Stop");
  });
  QObject::connect(&child, &QProcess::finished, &app, [&] {
    const IconSet& icons = dark ? dark_icon_set : light_icon_set;
    tray_icon.setIcon(icons.off);
    if (start_stop_action) start_stop_action->setText("Start");
  });

  // システムトレイアイコンを最新の状態に更新する
  emit QApplication::styleHints() -> colorSchemeChanged(QApplication::styleHints()->colorScheme());
  tray_icon.show();

  // フロントエンドが終了するときにバックエンドも終了させる
  QObject::connect(&app, &QApplication::aboutToQuit, &app, [&] {
    if (child.state() == QProcess::NotRunning) return;
    child.terminate();
    constexpr int msecs = 1000;
    if (!child.waitForFinished(msecs)) child.kill();
  });

  // バックエンドプロセスを起動する
  child.start(child_program);

  return QApplication::exec();
}
