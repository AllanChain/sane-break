// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

#include "lib/flags.h"
#include "lib/preferences.h"

class BreakWindow : public QMainWindow {
  Q_OBJECT
  Q_PROPERTY(QColor color MEMBER backgroundColor NOTIFY colorChanged)

 public:
  BreakWindow(SanePreferences *preferences, QWidget *parent = nullptr);
  ~BreakWindow();
  static const int SMALL_WINDOW_WIDTH;
  static const int SMALL_WINDOW_HEIGHT;
  void start(SaneBreak::BreakType type, int totalTime);
  void setTime(int remainingTime);
  void setFullScreen();
  void resizeToNormal();
  void initSize(QScreen *screen);
  void colorChanged();

 private:
  int totalTime;
  SanePreferences *preferences;
  QWidget *mainWidget;
  QLabel *countdownLabel;
  QLabel *breakLabel;
  QColor backgroundColor;
  QPropertyAnimation *progressAnim;
  QPropertyAnimation *bgAnim;
  bool waylandWorkaround = false;
};
