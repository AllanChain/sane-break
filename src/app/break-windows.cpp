// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-windows.h"

#include <qglobal.h>

#include <QApplication>
#include <QAudioOutput>
#include <QGuiApplication>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QRandomGenerator>
#include <QScreen>
#include <QSettings>
#include <QTime>
#include <QTimer>
#include <utility>

#include "config.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/preferences.h"
#include "gui/break-window.h"
#include "gui/sound-player.h"
#include "idle/factory.h"

#ifdef Q_OS_LINUX
#include <QGuiApplication>
#include <QPluginLoader>

#include "gui/layer-shell/interface.h"
#include "lib/linux/system-check.h"
#endif

BreakWindows::BreakWindows(QObject *parent) : AbstractBreakWindows(parent) {
  soundPlayer = new SoundPlayer(this);

  clockUpdateTimer = new QTimer(this);
  connect(clockUpdateTimer, &QTimer::timeout, this, &BreakWindows::updateClocks);

#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() == "wayland" && LinuxSystemSupport::layerShell) {
    QPluginLoader loader("libsanebreak_layer_shell");
    if (!loader.load()) {
      qWarning("Fail to find layer-shell plugin. Window layout may go wrong.");
    } else {
      layerShell = qobject_cast<LayerShellInterface *>(loader.instance());
      if (!layerShell) {
        qWarning("Fail to load layer-shell plugin. Window layout may go wrong.");
      } else {
      }
    }
  }
#endif
}

void BreakWindows::create(SaneBreak::BreakType type, SanePreferences *preferences) {
  QString promptMessage = type == SaneBreak::BreakType::Big
                              ? preferences->bigMessages->defaultValue()[0]
                              : preferences->smallMessages->defaultValue()[0];
  QStringList messagesToRoll = type == SaneBreak::BreakType::Big
                                   ? preferences->bigMessages->get()
                                   : preferences->smallMessages->get();
  QString fullScreenMessage = "";
  if (!messagesToRoll.empty()) {
    int randomIndex = QRandomGenerator::global()->bounded(messagesToRoll.size());
    fullScreenMessage = messagesToRoll[randomIndex];
  }

  BreakWindowData data = {
      .totalSeconds = type == SaneBreak::BreakType::Big ? preferences->bigFor->get()
                                                        : preferences->smallFor->get(),
      .message = {.prompt = promptMessage, .fullScreen = fullScreenMessage},
      .theme =
          {
              .mainBackground = preferences->backgroundColor->get(),
              .highlightBackground = type == SaneBreak::BreakType::Big
                                         ? preferences->bigHighlightColor->get()
                                         : preferences->smallHighlightColor->get(),
              .messageColor = preferences->messageColor->get(),
              .countDownColor = preferences->countDownColor->get(),
              .flashAnimationDuration = 500 / preferences->flashSpeed->get() * 100,
          },
      .show =
          {
              .prograssBar = preferences->showProgressBar->get(),
              .countdown = preferences->showCountdown->get(),
              .clock = preferences->showClock->get(),
              .endTime = preferences->showEndTime->get(),
              .buttons = preferences->showButtons->get(),
          },
  };
  QList<QScreen *> screens = QApplication::screens();
  for (QScreen *screen : std::as_const(screens)) {
    BreakWindow *w = new BreakWindow(data);
    m_windows.append(w);
    w->initSize(screen);
#ifdef Q_OS_LINUX
    if (layerShell) layerShell->layout(w->windowHandle());
#endif
    // GNOME mutter will make the window black if show full screen
    // See https://gitlab.gnome.org/GNOME/mutter/-/issues/2520
    // GNOME mutter will also refuse to make a window always on top if maximized.
    // Therefore, we use the same `show()` with and without Wayland workaround.
    w->show();
  }
  for (auto w : std::as_const(m_windows)) {
    connect(w, &BreakWindow::lockScreenRequested, this,
            &BreakWindows::lockScreenRequested);
    connect(w, &BreakWindow::exitForceBreakRequested, this,
            &BreakWindows::exitForceBreakRequested);
  }
  clockUpdateTimer->start(250);
}

void BreakWindows::destroy() {
  for (auto w : std::as_const(m_windows)) {
    w->close();
    w->deleteLater();
  }
  m_windows.clear();
  clockUpdateTimer->stop();
}

void BreakWindows::playEnterSound(SaneBreak::BreakType type,
                                  SanePreferences *preferences) {
  soundPlayer->play(type == SaneBreak::BreakType::Small
                        ? preferences->smallStartBell->get()
                        : preferences->bigStartBell->get());
}

void BreakWindows::playExitSound(SaneBreak::BreakType type,
                                 SanePreferences *preferences) {
  soundPlayer->play(type == SaneBreak::BreakType::Small
                        ? preferences->smallEndBell->get()
                        : preferences->bigEndBell->get());
}

void BreakWindows::setTime(int remainingTime) {
  QTime estimatedEndTime = QTime::currentTime().addSecs(remainingTime);
  // Round time to seconds
  if (estimatedEndTime.msec() > 500) {
    estimatedEndTime = estimatedEndTime.addMSecs(500);
  }
  for (auto w : std::as_const(m_windows)) {
    w->setTime(remainingTime, estimatedEndTime.toString("hh:mm:ss"));
  }
}
void BreakWindows::showFullScreen() {
  for (auto w : std::as_const(m_windows)) w->showFullScreen();
}
void BreakWindows::showFlashPrompt() {
  for (auto w : std::as_const(m_windows)) w->showFlashPrompt();
}
void BreakWindows::showButtons(Buttons buttons, bool show) {
  for (auto w : std::as_const(m_windows)) w->showButtons(buttons, show);
}
void BreakWindows::updateClocks() {
  QTime now = QTime::currentTime();
  QString hourMinute = now.toString("hh:mm");
  QString second = now.toString("ss");
  for (auto w : std::as_const(m_windows)) {
    w->setClock(hourMinute, second);
  }
}
