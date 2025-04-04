// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "window-manager.h"

#include <qglobal.h>

#include <utility>

#include "config.h"

#ifdef WITH_LAYER_SHELL
#include <LayerShellQt/shell.h>
#endif
#include <QApplication>
#include <QAudioOutput>
#include <QGuiApplication>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "break-window.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"
#include "sound-player.h"

BreakWindowManager::BreakWindowManager(SanePreferences *preferences, QObject *parent)
    : QObject(parent), preferences(preferences) {
  countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000);
  connect(countdownTimer, &QTimer::timeout, this, &BreakWindowManager::tick);

  forceBreakTimer = new QTimer(this);
  forceBreakTimer->setSingleShot(true);
  connect(forceBreakTimer, &QTimer::timeout, this, &BreakWindowManager::forceBreak);

  idleTimer = SystemIdleTime::createIdleTimer();
  connect(idleTimer, &SystemIdleTime::idleStart, this,
          &BreakWindowManager::onIdleStart);
  connect(idleTimer, &SystemIdleTime::idleEnd, this, &BreakWindowManager::onIdleEnd);
  soundPlayer = new SoundPlayer(this);

#ifdef WITH_LAYER_SHELL
  if (QGuiApplication::platformName() == "wayland")
    LayerShellQt::Shell::useLayerShell();
#endif
}

BreakWindowManager::~BreakWindowManager() {}

void BreakWindowManager::createWindows() {
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : std::as_const(screens)) {
    BreakWindow *w = new BreakWindow(preferences);
    windows.append(w);
    w->initSize(screen);
  }
}

void BreakWindowManager::show(BreakWindow::BreakType type) {
  currentType = type;
  totalTime = type == BreakWindow::BreakType::BIG ? preferences->bigFor->get()
                                                  : preferences->smallFor->get();
  remainingTime = totalTime;
  isIdle = false;
  isForceBreak = false;
  createWindows();
  for (auto w : std::as_const(windows)) w->start(type, totalTime);
  countdownTimer->start();
  forceBreakTimer->setInterval(preferences->flashFor->get() * 1000);
  forceBreakTimer->start();
  idleTimer->startWatching();
  soundPlayer->play(type == BreakWindow::BreakType::SMALL
                        ? preferences->smallStartBell->get()
                        : preferences->bigStartBell->get());
}

bool BreakWindowManager::isShowing() { return windows.size() != 0; }

void BreakWindowManager::close() {
  // Do nothing if window is already closed
  if (!isShowing()) return;
  countdownTimer->stop();
  forceBreakTimer->stop();
  for (auto w : std::as_const(windows)) {
    w->close();
    w->deleteLater();
  }
  windows.clear();
  idleTimer->stopWatching();
  emit timeout();
}

void BreakWindowManager::tick() {
  bool shouldCountDown = isForceBreak || isIdle;
  if (shouldCountDown) remainingTime--;
  for (auto w : std::as_const(windows)) w->setTime(remainingTime);
  if (totalTime - remainingTime >= preferences->confirmAfter->get())
    isForceBreak = true;
  if (remainingTime <= 0) {
    soundPlayer->play(currentType == BreakWindow::BreakType::SMALL
                          ? preferences->smallEndBell->get()
                          : preferences->bigEndBell->get());
    close();
  }
}

void BreakWindowManager::forceBreak() {
  emit resume();
  isForceBreak = true;
  for (auto w : std::as_const(windows)) w->setFullScreen();
}

void BreakWindowManager::onIdleStart() {
  if (isForceBreak || remainingTime <= 0) return;
  emit resume();
  isIdle = true;
  for (auto w : std::as_const(windows)) w->setFullScreen();
}

void BreakWindowManager::onIdleEnd() {
  // Here we ignore idleEnd when remainingTime is smaller than 3 seconds.
  // Because in most cases, user can wait for 3 seconds, but they don't want to
  // have the whole break again just because they accidentally touch the mouse
  // when the break almost finishes.
  if (isForceBreak || remainingTime <= 3) return;
  for (auto w : std::as_const(windows)) {
    w->resizeToNormal();
  }
  isIdle = false;
  remainingTime = totalTime;
}
