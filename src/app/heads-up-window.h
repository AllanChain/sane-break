// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QPropertyAnimation>
#include <QScreen>
#include <QWidget>

class HeadsUpWindow : public QWidget {
  Q_OBJECT
  Q_PROPERTY(qreal progress MEMBER m_progress NOTIFY progressChanged)

 public:
  HeadsUpWindow(int totalSeconds, QColor backgroundColor, QColor flashColor,
                QColor textColor, QWidget* parent = nullptr);
  void initSize(QScreen* screen);
  void setTime(int remainingSeconds);
  // Raw screen identity captured in initSize; see BreakWindow::screenIdentity().
  QScreen* screenIdentity() const { return m_screen; }

 signals:
  void clicked();
  void progressChanged();

 protected:
  void paintEvent(QPaintEvent*) override;
  void mousePressEvent(QMouseEvent*) override;

 private:
  int m_totalSeconds;
  qreal m_progress = 1.0;
  QColor m_fillColor;
  QColor m_textColor;
  QPropertyAnimation* m_progressAnim;
  QScreen* m_screen = nullptr;

  static const int PILL_WIDTH = 280;
  static const int PILL_HEIGHT = 56;
};
