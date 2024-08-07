// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_WINDOW_H
#define SANE_BREAK_WINDOW_H

#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>

class BreakWindow : public QMainWindow {
  Q_OBJECT
  Q_PROPERTY(QColor color MEMBER backgroundColor NOTIFY colorChanged)

 public:
  BreakWindow(QWidget *parent = nullptr);
  ~BreakWindow();
  void start(int totalTime);
  void setTime(int remainingTime);
  void resetProgressbar();
  void setFullScreen();
  void resizeToNormal();
  void initSize(QScreen *screen);
  void colorChanged();

 private:
  int totalTime;
  QLabel *countdownLabel;
  QColor backgroundColor;
  QPropertyAnimation *progressAnim;
  QPropertyAnimation *bgAnim;
};
#endif  // SANE_BREAK_WINDOW_H
