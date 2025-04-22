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
#include <QToolButton>
#include <QWidget>

#include "core/window-control.h"

namespace Ui {
class BreakReminder;
}

class BreakWindow : public AbstractBreakWindow {
  Q_OBJECT
  Q_PROPERTY(QColor color MEMBER backgroundColor NOTIFY colorChanged)

 public:
  BreakWindow(BreakData data, QWidget *parent = nullptr);
  ~BreakWindow() = default;
  static const int SMALL_WINDOW_WIDTH;
  static const int SMALL_WINDOW_HEIGHT;

  void start() override;
  void setTime(int remainingTime) override;
  void setFullScreen() override;
  void resizeToNormal() override;
  void showButtons(bool show = true) override;

  void initSize(QScreen *screen);
  void colorChanged();

 private:
  Ui::BreakReminder *ui;
  QWidget *mainWidget;
  QColor backgroundColor;
  QPropertyAnimation *progressAnim;
  QPropertyAnimation *bgAnim;
  bool waylandWorkaround = false;

  static void colorizeButton(QToolButton *button, QColor color);
};
