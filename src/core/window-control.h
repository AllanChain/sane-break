// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QList>
#include <QMainWindow>
#include <QObject>
#include <QScreen>

#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"

struct WindowDependencies {
  SanePreferences *preferences = nullptr;
  SystemIdleTime *idleTimer = nullptr;
};

struct BreakTheme {
  QColor mainBackground;
  QColor highlightBackground;
  QColor messageColor;
  QColor countDownColor;
  int flashAnimationDuration;
};

struct BreakData {
  int totalSeconds;
  QString message;
  BreakTheme theme;
};

class AbstractBreakWindow : public QMainWindow {
  Q_OBJECT

 public:
  AbstractBreakWindow(BreakData data, QWidget *parent = nullptr)
      : data(data), QMainWindow(parent) {};
  ~AbstractBreakWindow() = default;

  virtual void start() {};
  virtual void setTime(int remainingTime) {};
  virtual void setFullScreen() {};
  virtual void resizeToNormal() {};
  virtual void showKillTip() {};

 protected:
  int totalTime;
  BreakData data;
};

class AbstractWindowControl : public QObject {
  Q_OBJECT

 public:
  AbstractWindowControl(const WindowDependencies &deps, QObject *parent = nullptr);
  ~AbstractWindowControl() = default;

  virtual void show(SaneBreak::BreakType type);
  virtual void close();
  void tick();
  bool isShowing() { return m_isShowing; };

 signals:
  void timeout();
  void aborted();
  void countDownStateChanged(bool countingDown);

 protected:
  SaneBreak::BreakType m_currentType;
  bool m_isShowing = false;
  bool m_isIdle = false;
  bool m_isForceBreak = false;
  int m_secondsToForceBreak;
  int m_remainingSeconds;
  int m_totalSeconds;
  QList<AbstractBreakWindow *> m_windows;

  SanePreferences *preferences;
  SystemIdleTime *m_idleTimer;

  virtual void createWindows(SaneBreak::BreakType type) = 0;
  virtual void deleteWindows();
  BreakData breakData(SaneBreak::BreakType type);
  void forceBreak();
  void onIdleStart();
  void onIdleEnd();
};
