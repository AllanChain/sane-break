// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_WIDGETS_SLIDER_H
#define SANE_WIDGETS_SLIDER_H

#include <QMouseEvent>
#include <QPoint>
#include <QSlider>
#include <QWidget>

class SteppedSlider : public QSlider {
  Q_OBJECT

 public:
  SteppedSlider(QWidget *parent = nullptr);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

 private:
  int calculateValueFromPosition(const QPoint &pos) const;
};

#endif  // SANE_WIDGETS_SLIDER_H
