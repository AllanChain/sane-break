// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractAnimation>
#include <QColor>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QWindow>

#include "core/break-windows.h"

namespace Ui {
class BreakReminder;
}

class BreakWindow : public QMainWindow {
  Q_OBJECT
  Q_PROPERTY(QColor color MEMBER backgroundColor NOTIFY colorChanged)

 public:
  BreakWindow(BreakWindowData data, QWidget* parent = nullptr);
  ~BreakWindow() = default;
  static const int SMALL_WINDOW_WIDTH;
  static const int SMALL_WINDOW_HEIGHT;

  void setTime(int remainingTime, QString estimatedEndTime);
  void setClock(QString hourMinite);
  void showFullScreen();
  void showFlashPrompt();
  void showButtons(AbstractBreakWindows::Buttons buttons, bool show = true);

  void initSize(QScreen* screen);
  void colorChanged();

 signals:
  void lockScreenRequested();
  void exitForceBreakRequested();

 private:
  Ui::BreakReminder* ui;
  QWidget* mainWidget;
  QColor backgroundColor;
  BreakWindowData m_data;
  QPropertyAnimation* m_progressAnim;
  QAbstractAnimation* m_bgAnim;
  QLabel* m_bgImageLabel = nullptr;
  QGraphicsOpacityEffect* m_bgImageOpacity = nullptr;
  bool m_waylandWorkaround = false;
  bool m_supportTransparentInput = true;
  int m_totalSeconds;

  static void colorizeButton(QPushButton* button, QColor color);
};
