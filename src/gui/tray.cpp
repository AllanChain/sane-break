// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tray.h"

#include <qnamespace.h>

#include <QLabel>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>

StatusTrayWindow *StatusTrayWindow::createTrayOrWindow(QMenu *menu, QObject *parent) {
  if (QSystemTrayIcon::isSystemTrayAvailable()) return new StatusTray(menu, parent);
  return new StatusWindow(menu, parent);
}

QPixmap StatusTrayWindow::drawIcon(float arcRatio, IconVariants flags) {
  if (flags.testFlag(IconVariant::PAUSED)) return QPixmap(":/images/icon-pause.png");

  QPixmap pixmap(":/images/icon.png");
  QPen pen(QColor(5, 46, 22, 255));
  QPainter painter(&pixmap);

  painter.setRenderHint(QPainter::Antialiasing, true);
  pen.setWidth(16);
  painter.setPen(pen);
  QRect rect = pixmap.rect();
  rect.setSize(QSize(rect.width() - 8, rect.height() - 8));
  rect.setTopLeft(QPoint(8, 8));
  // Draw dark circle background
  painter.drawArc(rect, 0, 360 * 16);  // angles are in 1/16th of a degree

  // Draw light tracks
  pen.setColor(QColor(220, 252, 231, 255));
  painter.setPen(pen);
  int spanAngle = 360 * 16 * arcRatio;
  painter.drawArc(rect, 90 * 16, spanAngle);

  if (flags.testFlag(IconVariant::WILL_BIG)) {
    int dotSize = 64;
    QRect smallRect(pixmap.width() - 12 - dotSize, 12, dotSize, dotSize);
    QBrush brush(QColor(202, 138, 4, 255));
    painter.setBrush(brush);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawEllipse(smallRect);
  }

  return pixmap;
}

StatusTray::StatusTray(QMenu *menu, QObject *parent) : StatusTrayWindow(menu, parent) {
  icon = new QSystemTrayIcon();
  icon->setContextMenu(menu);
  connect(icon, &QSystemTrayIcon::activated, this,
          [=](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick ||
                reason == QSystemTrayIcon::MiddleClick) {
              emit breakTriggered();
            }
          });
}
void StatusTray::show() { icon->show(); }
void StatusTray::updateIcon(float arcRatio, IconVariants flags) {
  icon->setIcon(drawIcon(arcRatio, flags));
}
void StatusTray::setTitle(QString str) { icon->setToolTip("Sane Break\n" + str); }

StatusWindowWidget::StatusWindowWidget(QMenu *menu) : menu(menu) {
  setWindowIcon(QIcon(":/images/icon.png"));
  setWindowTitle("Sane Break");
  setWindowFlag(Qt::Dialog);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(12);
  setLayout(layout);

  icon = new QLabel(this);
  icon->setScaledContents(true);
  icon->setMaximumWidth(96);
  icon->setMaximumHeight(96);
  layout->addWidget(icon);
  layout->setAlignment(icon, Qt::AlignHCenter);

  info = new QLabel(this);
  layout->addWidget(info);
  layout->setAlignment(info, Qt::AlignHCenter);
};
void StatusWindowWidget::contextMenuEvent(QContextMenuEvent *event) {
  menu->exec(event->globalPos());
}
StatusWindow::StatusWindow(QMenu *menu, QObject *parent)
    : StatusTrayWindow(menu, parent) {
  widget = new StatusWindowWidget(menu);
}
StatusWindow::~StatusWindow() { widget->deleteLater(); }
void StatusWindow::show() { widget->show(); }
void StatusWindow::updateIcon(float arcRatio, IconVariants flags) {
  widget->icon->setPixmap(drawIcon(arcRatio, flags));
}
void StatusWindow::setTitle(QString str) { widget->info->setText(str); }
