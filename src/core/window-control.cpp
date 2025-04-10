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
#include "core/timer.h"

AbstractWindowControl::AbstractWindowControl(const WindowDependencies &deps,
                                             QObject *parent)
    : QObject(parent),
      preferences(deps.preferences),
      m_countDownTimer(deps.countDownTimer),
      m_forceBreakTimer(deps.forceBreakTimer),
      m_idleTimer(deps.idleTimer) {
  if (!m_countDownTimer->parent()) m_countDownTimer->setParent(this);
  if (!m_forceBreakTimer->parent()) m_forceBreakTimer->setParent(this);
  if (!m_idleTimer->parent()) m_idleTimer->setParent(this);

  m_countDownTimer->setInterval(1000);
  m_forceBreakTimer->setSingleShot(true);

  connect(m_countDownTimer, &AbstractTimer::timeout, this,
          &AbstractWindowControl::tick);
  connect(m_forceBreakTimer, &AbstractTimer::timeout, this,
          &AbstractWindowControl::forceBreak);
  connect(m_idleTimer, &SystemIdleTime::idleStart, this,
          &AbstractWindowControl::onIdleStart);
  connect(m_idleTimer, &SystemIdleTime::idleEnd, this,
          &AbstractWindowControl::onIdleEnd);
}

void AbstractWindowControl::show(SaneBreak::BreakType type) {
  m_isShowing = true;
  m_currentType = type;
  m_totalTime = type == SaneBreak::BreakType::Big ? preferences->bigFor->get()
                                                  : preferences->smallFor->get();
  m_remainingTime = m_totalTime;
  m_isIdle = false;
  m_isForceBreak = false;
  createWindows(type);
  for (auto w : std::as_const(m_windows)) w->start();
  m_countDownTimer->start();
  m_forceBreakTimer->setInterval(preferences->flashFor->get() * 1000);
  m_forceBreakTimer->start();
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
  m_countDownTimer->stop();
  m_forceBreakTimer->stop();
  m_idleTimer->stopWatching();
  deleteWindows();
  if (m_remainingTime > 0)
    emit aborted();
  else
    emit timeout();
}

void AbstractWindowControl::tick() {
  bool shouldCountDown = m_isForceBreak || m_isIdle;
  if (shouldCountDown) m_remainingTime--;
  for (auto w : std::as_const(m_windows)) w->setTime(m_remainingTime);
  if (m_totalTime - m_remainingTime >= preferences->confirmAfter->get()) {
    m_isForceBreak = true;  // confirm break
    if (preferences->showKillTip->get())
      for (auto w : std::as_const(m_windows)) w->showKillTip();
  }
  if (m_remainingTime <= 0) close();
}

void AbstractWindowControl::forceBreak() {
  emit countDownStateChanged(true);
  m_isForceBreak = true;
  for (auto w : std::as_const(m_windows)) w->setFullScreen();
  if (preferences->showKillTip->get())
    for (auto w : std::as_const(m_windows)) w->showKillTip();
}

void AbstractWindowControl::onIdleStart() {
  if (m_isForceBreak || m_remainingTime <= 0) return;
  emit countDownStateChanged(true);
  m_isIdle = true;
  for (auto w : std::as_const(m_windows)) w->setFullScreen();
}

void AbstractWindowControl::onIdleEnd() {
  // Here we ignore idleEnd when remainingTime is smaller than 3 seconds.
  // Because in most cases, user can wait for 3 seconds, but they don't want to
  // have the whole break again just because they accidentally touch the mouse
  // when the break almost finishes.
  if (m_isForceBreak || m_remainingTime <= 3) return;
  emit countDownStateChanged(false);
  for (auto w : std::as_const(m_windows)) {
    w->resizeToNormal();
  }
  m_isIdle = false;
  m_remainingTime = m_totalTime;
}
