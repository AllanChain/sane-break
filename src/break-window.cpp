// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-window.h"

#include <QApplication>
#include <QColor>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QRect>
#include <QScreen>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>
#include <Qt>
#include <cmath>

BreakWindow::BreakWindow(BreakType type, QWidget *parent) : QMainWindow(parent) {
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
  setProperty("isFullScreen", false);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  QProgressBar *progressBar = new QProgressBar();
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  QVBoxLayout *textLayout = new QVBoxLayout();
  textLayout->setAlignment(Qt::AlignCenter);
  layout->addLayout(textLayout);

  QLabel *breakLabel = new QLabel(
      QString("Time for a %1 break").arg(type == BreakType::BIG ? "big" : "small"));
  breakLabel->setObjectName("breakLabel");
  textLayout->addWidget(breakLabel);

  countdownLabel = new QLabel(this);
  countdownLabel->setObjectName("countdownLabel");
  countdownLabel->setAlignment(Qt::AlignCenter);
  countdownLabel->setVisible(false);
  textLayout->addWidget(countdownLabel);

  progressAnim = new QPropertyAnimation(progressBar, "value");
  progressAnim->setStartValue(100);
  progressAnim->setEndValue(0);

  bgAnim = new QPropertyAnimation(this, "color");
  if (type == BreakType::BIG)
    bgAnim->setStartValue(QColor(180, 142, 173, 100));
  else
    bgAnim->setStartValue(QColor(235, 203, 139, 100));
  bgAnim->setEndValue(QColor(46, 52, 64, 255));
  bgAnim->setDuration(500);
  bgAnim->setLoopCount(-1);
}

BreakWindow::~BreakWindow() {}

void BreakWindow::colorChanged() {
  setStyleSheet(QString("BreakWindow, BreakWindow .QWidget { background: "
                        "rgba(%1, %2, %3, %4); }")
                    .arg(backgroundColor.red())
                    .arg(backgroundColor.green())
                    .arg(backgroundColor.blue())
                    .arg(backgroundColor.alpha()));
}

void BreakWindow::start(int totalTime) {
  this->totalTime = totalTime;
  progressAnim->setDuration(totalTime * 1000);
  bgAnim->start();
  setTime(totalTime);
}

void BreakWindow::setTime(int remainingTime) {
  if (remainingTime == totalTime) {
    progressAnim->stop();
    progressAnim->start();
  }
  countdownLabel->setText(QString("%1").arg(round(float(remainingTime))));
}

void BreakWindow::setFullScreen() {
  setProperty("isFullScreen", true);
  bgAnim->stop();
  setProperty("color", bgAnim->endValue());
  countdownLabel->setVisible(true);
  QPropertyAnimation *resizeAnim = new QPropertyAnimation(this, "geometry");
  resizeAnim->setStartValue(geometry());
  resizeAnim->setEndValue(screen()->geometry());
  resizeAnim->setDuration(300);
  resizeAnim->start();
}

void BreakWindow::resizeToNormal() {
  setProperty("isFullScreen", false);
  bgAnim->start();
  countdownLabel->setVisible(false);
  QPropertyAnimation *resizeAnim = new QPropertyAnimation(this, "geometry");
  QRect rect = screen()->geometry();
  QRect targetGeometry = QRect(rect.x() + rect.width() / 2 - 150, rect.y(), 300, 100);
  resizeAnim->setStartValue(geometry());
  resizeAnim->setEndValue(targetGeometry);
  resizeAnim->setDuration(100);
  resizeAnim->start();
}

void BreakWindow::initSize(QScreen *screen) {
  QRect rect = screen->geometry();
  setGeometry(rect.x() + rect.width() / 2 - 150, rect.y(), 300, 100);
}
