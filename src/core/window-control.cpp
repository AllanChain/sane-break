// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/window-control.h"

#include <QGuiApplication>
#include <QList>
#include <QObject>
#include <QRandomGenerator>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <utility>

#include "config.h"
#include "core/flags.h"
#include "core/idle-time.h"

AbstractWindowControl::AbstractWindowControl(const WindowDependencies &deps,
                                             QObject *parent)
    : QObject(parent), preferences(deps.preferences), m_idleTimer(deps.idleTimer) {
  if (!m_idleTimer->parent()) m_idleTimer->setParent(this);

  connect(m_idleTimer, &SystemIdleTime::idleStart, this,
          &AbstractWindowControl::onIdleStart);
  connect(m_idleTimer, &SystemIdleTime::idleEnd, this,
          &AbstractWindowControl::onIdleEnd);
}

void AbstractWindowControl::show(SaneBreak::BreakType type) {
  m_isShowing = true;
  m_currentType = type;
  m_totalSeconds = type == SaneBreak::BreakType::Big ? preferences->bigFor->get()
                                                     : preferences->smallFor->get();
  m_remainingSeconds = m_totalSeconds;
  m_isIdle = false;
  m_isForceBreak = false;
  m_secondsToForceBreak = preferences->flashFor->get();

  createWindows(type);
  for (auto w : std::as_const(m_windows)) w->start();

  m_idleTimer->startWatching();
}

BreakData AbstractWindowControl::breakData(SaneBreak::BreakType type) {
  BreakTheme theme = {
      .mainBackground = preferences->backgroundColor->get(),
      .highlightBackground = type == SaneBreak::BreakType::Big
                                 ? preferences->bigHighlightColor->get()
                                 : preferences->smallHighlightColor->get(),
      .messageColor = preferences->messageColor->get(),
      .countDownColor = preferences->countDownColor->get(),
      .flashAnimationDuration = 500 / preferences->flashSpeed->get() * 100,
  };
  QStringList messagesToRoll = type == SaneBreak::BreakType::Big
                                   ? preferences->bigMessages->get()
                                   : preferences->smallMessages->get();
  QString message = "";
  if (!messagesToRoll.empty()) {
    int randomIndex = QRandomGenerator::global()->bounded(messagesToRoll.size());
    message = messagesToRoll[randomIndex];
  }

  return {
      .totalSeconds = type == SaneBreak::BreakType::Big ? preferences->bigFor->get()
                                                        : preferences->smallFor->get(),
      .message = message,
      .theme = theme,
  };
}

void AbstractWindowControl::close() {
  // Do nothing if window is already closed
  if (!m_isShowing) return;
  m_isShowing = false;
  m_idleTimer->stopWatching();
  deleteWindows();
  if (m_remainingSeconds > 0)
    emit aborted();
  else
    emit timeout();
}

void AbstractWindowControl::tick() {
  bool shouldCountDown = m_isForceBreak || m_isIdle;
  if (shouldCountDown) m_remainingSeconds--;
  for (auto w : std::as_const(m_windows)) w->setTime(m_remainingSeconds);

  // Confirm break
  if (m_totalSeconds - m_remainingSeconds >= preferences->confirmAfter->get()) {
    m_isForceBreak = true;
    if (preferences->showKillTip->get())
      for (auto w : std::as_const(m_windows)) w->showKillTip();
  }
  if (m_remainingSeconds <= 0)
    close();
  else if (!m_isForceBreak && --m_secondsToForceBreak <= 0)
    forceBreak();
}

void AbstractWindowControl::forceBreak() {
  emit countDownStateChanged(true);
  m_isForceBreak = true;
  for (auto w : std::as_const(m_windows)) w->setFullScreen();
  if (preferences->showKillTip->get())
    for (auto w : std::as_const(m_windows)) w->showKillTip();
}

void AbstractWindowControl::onIdleStart() {
  if (m_isForceBreak || m_remainingSeconds <= 0) return;
  emit countDownStateChanged(true);
  m_isIdle = true;
  for (auto w : std::as_const(m_windows)) w->setFullScreen();
}

void AbstractWindowControl::onIdleEnd() {
  // Here we ignore idleEnd when remainingTime is smaller than 3 seconds.
  // Because in most cases, user can wait for 3 seconds, but they don't want to
  // have the whole break again just because they accidentally touch the mouse
  // when the break almost finishes.
  if (m_isForceBreak || m_remainingSeconds <= 3) return;
  emit countDownStateChanged(false);
  for (auto w : std::as_const(m_windows)) {
    w->resizeToNormal();
  }
  m_isIdle = false;
  m_remainingSeconds = m_totalSeconds;
}

void AbstractWindowControl::deleteWindows() {
  for (auto w : std::as_const(m_windows)) {
    w->close();
    w->deleteLater();
  }
  m_windows.clear();
}
