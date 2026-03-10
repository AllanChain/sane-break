// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "heads-up-window.h"

#include <qglobal.h>

#include <QFont>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QSequentialAnimationGroup>
#include <QWidget>

HeadsUpWindow::HeadsUpWindow(int totalSeconds, QColor bgColor, QColor highlightColor,
                             QColor textColor, QWidget* parent)
    : QWidget(parent),
      m_bgColor(bgColor),
      m_flashColor(bgColor),
      m_textColor(textColor) {
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setAttribute(Qt::WA_MacAlwaysShowToolWindow);
  setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
#ifdef Q_OS_MACOS
  setWindowFlag(Qt::ToolTip);
#else
  setWindowFlag(Qt::Tool);
#endif
  setCursor(Qt::PointingHandCursor);
  setFixedSize(PILL_WIDTH, PILL_HEIGHT);

  // Animate progress from 1.0 to 0.0
  m_progressAnim = new QPropertyAnimation(this, "progress");
  m_progressAnim->setStartValue(1.0);
  m_progressAnim->setEndValue(0.0);
  m_progressAnim->setDuration(totalSeconds * 1000);
  connect(this, &HeadsUpWindow::progressChanged, this,
          QOverload<>::of(&QWidget::update));
  m_progressAnim->start();

  // Slow flash animation: highlight → main, 2.5s per cycle
  auto* flashGroup = new QSequentialAnimationGroup(this);
  auto* toHighlight = new QPropertyAnimation(this, "flashColor");
  toHighlight->setStartValue(bgColor);
  toHighlight->setEndValue(highlightColor);
  toHighlight->setDuration(1250);
  auto* toMain = new QPropertyAnimation(this, "flashColor");
  toMain->setStartValue(highlightColor);
  toMain->setEndValue(bgColor);
  toMain->setDuration(1250);
  flashGroup->addAnimation(toHighlight);
  flashGroup->addAnimation(toMain);
  flashGroup->setLoopCount(-1);
  m_flashAnim = flashGroup;
  connect(this, &HeadsUpWindow::flashColorChanged, this,
          QOverload<>::of(&QWidget::update));
  m_flashAnim->start();
}

void HeadsUpWindow::initSize(QScreen* screen) {
  QRect geo = screen->availableGeometry();
  move(geo.x() + (geo.width() - PILL_WIDTH) / 2, geo.y() + 16);
}

void HeadsUpWindow::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QRectF rect(0, 0, width(), height());
  qreal radius = height() / 2.0;

  // Clip to pill shape
  QPainterPath pillPath;
  pillPath.addRoundedRect(rect, radius, radius);
  painter.setClipPath(pillPath);

  // Draw empty background (semi-transparent)
  QColor emptyColor = m_flashColor;
  emptyColor.setAlphaF(m_flashColor.alphaF() * 0.3);
  painter.fillRect(rect, emptyColor);

  // Draw filled portion (progress from left)
  if (m_progress > 0) {
    QRectF fillRect(0, 0, width() * m_progress, height());
    painter.fillRect(fillRect, m_flashColor);
  }

  painter.setClipping(false);

  // Draw title text
  painter.setPen(m_textColor);
  QFont titleFont = painter.font();
  titleFont.setPixelSize(16);
  titleFont.setBold(true);
  painter.setFont(titleFont);
  QRectF topRect(0, 8, width(), height() / 2);
  painter.drawText(topRect, Qt::AlignHCenter | Qt::AlignTop, tr("Break soon"));

  // Draw subtitle text
  QFont subtitleFont = painter.font();
  subtitleFont.setPixelSize(12);
  subtitleFont.setBold(false);
  painter.setFont(subtitleFont);
  QRectF bottomRect(0, 0, width(), height() - 8);
  painter.drawText(bottomRect, Qt::AlignHCenter | Qt::AlignBottom,
                   tr("Click to start"));
}

void HeadsUpWindow::mousePressEvent(QMouseEvent*) { emit clicked(); }
