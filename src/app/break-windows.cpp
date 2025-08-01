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

#ifdef WITH_LAYER_SHELL
#include <LayerShellQt/shell.h>
#endif
BreakWindows::BreakWindows(QObject *parent) : AbstractBreakWindows(parent) {
  soundPlayer = new SoundPlayer(this);

  clockUpdateTimer = new QTimer(this);
  connect(clockUpdateTimer, &QTimer::timeout, this, &BreakWindows::updateClocks);

#ifdef WITH_LAYER_SHELL
  if (QGuiApplication::platformName() == "wayland")
    LayerShellQt::Shell::useLayerShell();
#endif
}

void BreakWindows::create(SaneBreak::BreakType type, SanePreferences *preferences) {
  QStringList messagesToRoll = type == SaneBreak::BreakType::Big
                                   ? preferences->bigMessages->get()
                                   : preferences->smallMessages->get();
  QString message = "";
  if (!messagesToRoll.empty()) {
    int randomIndex = QRandomGenerator::global()->bounded(messagesToRoll.size());
    message = messagesToRoll[randomIndex];
  }

  BreakWindowData data = {
      .totalSeconds = type == SaneBreak::BreakType::Big ? preferences->bigFor->get()
                                                        : preferences->smallFor->get(),
      .message = message,
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
