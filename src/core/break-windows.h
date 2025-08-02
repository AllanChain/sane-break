// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QList>
#include <QMainWindow>
#include <QObject>
#include <QScreen>

#include "core/flags.h"
#include "core/preferences.h"

struct BreakWindowData {
  int totalSeconds;
  struct {
    QString prompt;
    QString fullScreen;
  } message;
  struct {
    QColor mainBackground;
    QColor highlightBackground;
    QColor messageColor;
    QColor countDownColor;
    int flashAnimationDuration;
  } theme;
  struct {
    bool prograssBar;
    bool countdown;
    bool clock;
    bool endTime;
    bool buttons;
  } show;
};

class AbstractBreakWindows : public QObject {
  Q_OBJECT
 public:
  enum class Button {
    LockScreen = 1 << 0,
    ExitForceBreak = 1 << 1,
  };
  Q_DECLARE_FLAGS(Buttons, Button)
  using QObject::QObject;
  ~AbstractBreakWindows() = default;
  // static BreakWindowData breakData(SaneBreak::BreakType, SanePreferences *);
  virtual void create(SaneBreak::BreakType, SanePreferences *) = 0;
  virtual void destroy() = 0;
  virtual void setTime(int remainingTime) = 0;
  virtual void showFullScreen() = 0;
  virtual void showFlashPrompt() = 0;
  virtual void showButtons(Buttons, bool show = true) = 0;
  virtual void playEnterSound(SaneBreak::BreakType, SanePreferences *) = 0;
  virtual void playExitSound(SaneBreak::BreakType, SanePreferences *) = 0;

 signals:
  void lockScreenRequested();
  void exitForceBreakRequested();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractBreakWindows::Buttons)
