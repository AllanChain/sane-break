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
  QString message;
  struct {
    QColor mainBackground;
    QColor highlightBackground;
    QColor messageColor;
    QColor countDownColor;
    int flashAnimationDuration;
  } theme;
};

class AbstractBreakWindow : public QMainWindow {
  Q_OBJECT

 public:
  enum class Button {
    LockScreen = 1 << 0,
    ExitForceBreak = 1 << 1,
  };
  Q_DECLARE_FLAGS(Buttons, Button)

  AbstractBreakWindow(BreakWindowData data, QWidget *parent = nullptr)
      : QMainWindow(parent), data(data) {};
  ~AbstractBreakWindow() = default;

  virtual void start() = 0;
  virtual void setTime(int remainingTime) = 0;
  virtual void showFullScreen() = 0;
  virtual void showFlashPrompt() = 0;
  virtual void showButtons(Buttons buttons) = 0;

 signals:
  void lockScreenRequested();
  void exitForceBreakRequested();

 protected:
  BreakWindowData data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractBreakWindow::Buttons)

class AbstractBreakWindows : public QObject {
  Q_OBJECT
 public:
  using QObject::QObject;
  ~AbstractBreakWindows() = default;
  void create(SaneBreak::BreakType type, SanePreferences *preferences);
  virtual void destroy();  // virtual for easy testing
  void setTime(int remainingTime);
  void showFullScreen();
  void showFlashPrompt();
  void showButtons(AbstractBreakWindow::Buttons buttons);
  virtual void playEnterSound(SaneBreak::BreakType, SanePreferences *) {};
  virtual void playExitSound(SaneBreak::BreakType, SanePreferences *) {};

 signals:
  void lockScreenRequested();
  void exitForceBreakRequested();

 protected:
  QList<AbstractBreakWindow *> m_windows;
  virtual void createWindows(BreakWindowData data) = 0;
};
