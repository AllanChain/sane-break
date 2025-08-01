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
#include <QPushButton>
#include <QTimer>
#include <QWidget>

#include "core/break-windows.h"

namespace Ui {
class BreakReminder;
}

class BreakWindow : public AbstractBreakWindow {
  Q_OBJECT
  Q_PROPERTY(QColor color MEMBER backgroundColor NOTIFY colorChanged)

 public:
  BreakWindow(BreakWindowData data, QWidget *parent = nullptr);
  ~BreakWindow() = default;
  static const int SMALL_WINDOW_WIDTH;
  static const int SMALL_WINDOW_HEIGHT;

  void start() override;
  void setTime(int remainingTime) override;
  void showFullScreen() override;
  void showFlashPrompt() override;
  void showButtons(Buttons buttons) override;

  void initSize(QScreen *screen);
  void colorChanged();

 private:
  Ui::BreakReminder *ui;
  QWidget *mainWidget;
  QColor backgroundColor;
  QPropertyAnimation *m_progressAnim;
  QPropertyAnimation *m_bgAnim;
  bool m_waylandWorkaround = false;
  int m_totalSeconds;

  static void colorizeButton(QPushButton *button, QColor color);
};
