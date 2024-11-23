// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "window-manager.h"

#include <qglobal.h>

#include <utility>

#include "config.h"
#include "preferences.h"

#ifdef LayerShellQt_FOUND
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#elif defined Q_OS_MACOS
#include "macos/workspace.h"
#endif
#include <QApplication>
#include <QAudioOutput>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "break-window.h"
#include "idle-time.h"

BreakWindowManager::BreakWindowManager() : QObject() {
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

#ifdef LayerShellQt_FOUND
  if (QGuiApplication::platformName() == "wayland")
    LayerShellQt::Shell::useLayerShell();
#endif
}

BreakWindowManager::~BreakWindowManager() {};

void BreakWindowManager::createWindows() {
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : std::as_const(screens)) {
    BreakWindow *w = new BreakWindow(currentType);
    windows.append(w);
    w->initSize(screen);
    w->show();
    w->hide();
#ifdef LayerShellQt_FOUND
    if (QGuiApplication::platformName() == "wayland")
      if (auto window = LayerShellQt::Window::get(w->windowHandle())) {
        using namespace LayerShellQt;
        window->setCloseOnDismissed(true);
        window->setLayer(Window::LayerOverlay);
        window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
        window->setAnchors(Window::AnchorTop);
        // We do not want to reserve space for widgets like taskbar (#19)
        window->setExclusiveZone(-1);
      }
#elif defined Q_OS_MACOS
    macSetAllWorkspaces(w->windowHandle());
#endif
    w->show();
  }
}

void BreakWindowManager::show(BreakType type) {
  currentType = type;
  totalTime = type == BreakType::BIG ? SanePreferences::bigFor->get()
                                     : SanePreferences::smallFor->get();
  remainingTime = totalTime;
  isIdle = false;
  isForceBreak = false;
  createWindows();
  for (auto w : std::as_const(windows)) w->start(totalTime);
  countdownTimer->start();
  forceBreakTimer->setInterval(SanePreferences::flashFor->get() * 1000);
  forceBreakTimer->start();
  idleTimer->startWatching(NOTIFY_FIRST_IDLE);
  playSound(SanePreferences::bellStart->get());
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
  if (!shouldCountDown) {
    for (auto w : std::as_const(windows)) w->setTime(remainingTime);
  } else {
    remainingTime--;
    for (auto w : std::as_const(windows)) w->setTime(remainingTime);
  }
  if (remainingTime <= 0) {
    playSound(SanePreferences::bellEnd->get());
    close();
  }
}

void BreakWindowManager::forceBreak() {
  isForceBreak = true;
  for (auto w : std::as_const(windows)) w->setFullScreen();
}

void BreakWindowManager::onIdleStart() {
  if (isForceBreak || remainingTime <= 0) return;
  for (auto w : std::as_const(windows)) {
    w->setFullScreen();
  }
  isIdle = true;
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

void BreakWindowManager::playSound(QString soundFile) {
  if (soundFile.isEmpty()) return;
  if (currentType == BreakType::SMALL && !SanePreferences::bellInSmall->get()) return;
  QMediaPlayer *soundPlayer = new QMediaPlayer(this);
  QAudioOutput *audioOutput = new QAudioOutput();
  soundPlayer->setAudioOutput(audioOutput);
  audioOutput->setVolume(100);
  soundPlayer->setSource(QUrl(soundFile));
  soundPlayer->play();
  connect(soundPlayer, &QMediaPlayer::playbackStateChanged, this,
          [=](QMediaPlayer::PlaybackState state) {
            if (state != QMediaPlayer::PlaybackState::StoppedState) return;
            soundPlayer->deleteLater();
            audioOutput->deleteLater();
          });
}
