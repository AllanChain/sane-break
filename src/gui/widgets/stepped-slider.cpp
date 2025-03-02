// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stepped-slider.h"

#include <QMouseEvent>
#include <QPoint>
#include <QRect>
#include <QSlider>
#include <QString>
#include <QStyleOptionSlider>
#include <QWidget>
#include <Qt>
#include <QtContainerFwd>

SteppedSlider::SteppedSlider(QWidget *parent) : QSlider(parent) {
  setTickPosition(QSlider::TicksBelow);
}

void SteppedSlider::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    int value = calculateValueFromPosition(event->pos());
    setValue(value);
    event->accept();
  } else {
    QSlider::mousePressEvent(event);
  }
}

void SteppedSlider::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    int value = calculateValueFromPosition(event->pos());
    setValue(value);
    event->accept();
  } else {
    QSlider::mouseMoveEvent(event);
  }
}

int SteppedSlider::calculateValueFromPosition(const QPoint &pos) const {
  int min = minimum();
  int max = maximum();
  int step = singleStep();
  int range = max - min;

  QStyleOptionSlider opt;
  initStyleOption(&opt);
  QRect sliderRect =
      style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

  if (orientation() == Qt::Horizontal) {
    int sliderLength = width() - sliderRect.width();
    int posX = pos.x() - sliderRect.width() / 2;
    int value = min + (posX * range) / sliderLength;
    value = ((qBound(min, value, max) - min) / step) * step + min;
    return value;
  } else {
    int sliderLength = height() - sliderRect.height();
    int posY = pos.y() - sliderRect.height() / 2;
    int value = min + ((sliderLength - posY) * range) / sliderLength;
    value = ((qBound(min, value, max) - min) / step) * step + min;
    return value;
  }
}
