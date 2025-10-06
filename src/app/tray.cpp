// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tray.h"

#include <qnamespace.h>

#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QVBoxLayout>
#include <QWidget>

#include "core/app.h"
#include "core/flags.h"
#include "core/preferences.h"
#include "lib/utils.h"

StatusTrayWindow* StatusTrayWindow::createTrayOrWindow(SanePreferences* preferences,
                                                       QObject* parent) {
  if (QSystemTrayIcon::isSystemTrayAvailable())
    return new StatusTray(preferences, parent);
  return new StatusWindow(preferences, parent);
}

StatusTrayWindow::StatusTrayWindow(SanePreferences* preferences, QObject* parent)
    : QObject(parent), preferences(preferences) {
  menu = new QMenu();
  nextBreakAction = new QAction(this);
  menu->addAction(nextBreakAction);
  connect(nextBreakAction, &QAction::triggered, this,
          &StatusTrayWindow::nextBreakRequested);

  bigBreakAction = new QAction(this);
  menu->addAction(bigBreakAction);
  connect(bigBreakAction, &QAction::triggered, this,
          &StatusTrayWindow::nextBigBreakRequested);

  smallBreakInsteadAction = menu->addAction(tr("Take a small break instead"));
  smallBreakInsteadAction->setVisible(false);
  connect(smallBreakInsteadAction, &QAction::triggered, this,
          &StatusTrayWindow::smallBreakInsteadRequested);

  menu->addSeparator();

  postponeMenu = menu->addMenu(tr("Postpone"));
  for (const QString& minuteString : preferences->postponeMinutes->get()) {
    int minute = minuteString.toInt();
    connect(postponeMenu->addAction(tr("%n min", "", minute)), &QAction::triggered,
            this, [this, minute]() { emit this->postponeRequested(minute * 60); });
  }
  enableBreak = menu->addAction(tr("Enable Break"));
  enableBreak->setVisible(false);
  connect(enableBreak, &QAction::triggered, this,
          &StatusTrayWindow::enableBreakRequested);

  menu->addSeparator();

  connect(menu->addAction(tr("Preferences")), &QAction::triggered, this,
          &StatusTrayWindow::preferenceWindowRequested);

  quitAction = menu->addAction(tr("Quit"));
  connect(quitAction, &QAction::triggered, this, &StatusTrayWindow::quitRequested);
}

void StatusTrayWindow::update(TrayData data) {
  nextBreakAction->setText(
      tr("Next break after %1").arg(formatTime(data.secondsToNextBreak)));
  bigBreakAction->setText(
      tr("Next big break after %1").arg(formatTime(data.secondsToNextBigBreak)));

  enableBreak->setVisible(data.pauseReasons);
  nextBreakAction->setVisible(!data.isBreaking && !data.pauseReasons);
  bigBreakAction->setVisible(!data.isBreaking && !data.pauseReasons);
  smallBreakInsteadAction->setVisible(data.isBreaking &&
                                      data.smallBreaksBeforeBigBreak == 0);

  if (data.pauseReasons) {
    if (data.pauseReasons.testFlag(PauseReason::OnBattery)) {
      setTitle(tr("Paused on battery"));
    } else if (data.pauseReasons.testFlag(PauseReason::AppOpen)) {
      setTitle(tr("Paused on app running"));
    } else if (data.pauseReasons.testFlag(PauseReason::Idle)) {
      setTitle(tr("Paused on idle"));
    }
  } else {
    setTitle(QString("%1 %2").arg(
        data.smallBreaksBeforeBigBreak == 0 ? tr("big break") : tr("small break"),
        formatTime(data.secondsToNextBreak)));
  }
}

QPixmap StatusTrayWindow::drawIcon(TrayData data) {
  if (data.pauseReasons || data.isBreaking ||
      data.secondsToNextBreak > data.secondsFromLastBreakToNext)
    return QPixmap(":/images/icon-pause.png");

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
  float arcRatio = float(data.secondsToNextBreak) / data.secondsFromLastBreakToNext;
  int spanAngle = 360 * 16 * arcRatio;
  painter.drawArc(rect, 90 * 16, spanAngle);

  if (data.smallBreaksBeforeBigBreak == 0) {
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

void StatusTrayWindow::onPostponeMinutesChange() {
  postponeMenu->clear();
  for (const QString& minuteString : preferences->postponeMinutes->get()) {
    int minute = minuteString.toInt();
    connect(postponeMenu->addAction(tr("%n min", "", minute)), &QAction::triggered,
            this, [this, minute]() { emit postponeRequested(minute * 60); });
  }
}

StatusTray::StatusTray(SanePreferences* preferences, QObject* parent)
    : StatusTrayWindow(preferences, parent) {
  icon = new QSystemTrayIcon();
  icon->setContextMenu(menu);
  connect(icon, &QSystemTrayIcon::activated, this, &StatusTray::onIconTrigger);
}
void StatusTray::onIconTrigger(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
      if (preferences->quickBreak->get()) emit nextBreakRequested();
      break;
#ifdef Q_OS_WINDOWS
    case QSystemTrayIcon::Trigger:
      icon->contextMenu()->popup(QCursor::pos());
      break;
#endif
    default:
      break;
  }
}
void StatusTray::show() { icon->show(); }
void StatusTray::update(TrayData data) {
  StatusTrayWindow::update(data);
  icon->setIcon(drawIcon(data));
}
void StatusTray::setTitle(QString str) { icon->setToolTip("Sane Break\n" + str); }

StatusWindowWidget::StatusWindowWidget(QMenu* menu) : menu(menu) {
  setWindowIcon(QIcon(":/images/icon.png"));
  setWindowTitle("Sane Break");
  setWindowFlag(Qt::Dialog);
  QVBoxLayout* layout = new QVBoxLayout();
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
void StatusWindowWidget::contextMenuEvent(QContextMenuEvent* event) {
  menu->exec(event->globalPos());
}
StatusWindow::StatusWindow(SanePreferences* preferences, QObject* parent)
    : StatusTrayWindow(preferences, parent) {
  widget = new StatusWindowWidget(menu);
}
StatusWindow::~StatusWindow() { widget->deleteLater(); }
void StatusWindow::show() { widget->show(); }
void StatusWindow::update(TrayData data) {
  StatusTrayWindow::update(data);
  widget->icon->setPixmap(drawIcon(data));
}
void StatusWindow::setTitle(QString str) { widget->info->setText(str); }
