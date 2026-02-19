// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tray.h"

#include <qnamespace.h>

#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <optional>

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

  postponeMenu = menu->addAction(tr("Postpone"));
  connect(postponeMenu, &QAction::triggered, this,
          &StatusTrayWindow::postponeRequested);
  meetingAction = menu->addAction(tr("Meeting Mode"));
  connect(meetingAction, &QAction::triggered, this,
          &StatusTrayWindow::meetingRequested);
  focusAction = menu->addAction(tr("Focus Mode"));
  connect(focusAction, &QAction::triggered, this,
          &StatusTrayWindow::focusRequested);
  endFocusAction = menu->addAction(tr("End Focus && Break"));
  endFocusAction->setVisible(false);
  connect(endFocusAction, &QAction::triggered, this,
          &StatusTrayWindow::endFocusRequested);
  endMeetingAction = menu->addAction(tr("End Meeting && Break Now"));
  endMeetingAction->setVisible(false);
  connect(endMeetingAction, &QAction::triggered, this,
          &StatusTrayWindow::endMeetingBreakNowRequested);
  extendMeetingMenu = menu->addMenu(tr("Extend Meeting"));
  extendMeetingMenu->menuAction()->setVisible(false);
  auto addExtendOpt = [this](const QString& label, int secs) {
    QAction* action = extendMeetingMenu->addAction(label);
    extendOptions.append({action, secs});
    connect(action, &QAction::triggered, this,
            [this, secs]() { emit extendMeetingRequested(secs); });
  };
  addExtendOpt(tr("+15 min"), 15 * 60);
  addExtendOpt(tr("+30 min"), 30 * 60);
  addExtendOpt(tr("+1 hour"), 60 * 60);
  enableBreak = menu->addAction(tr("Enable Break"));
  enableBreak->setVisible(false);
  connect(enableBreak, &QAction::triggered, this,
          &StatusTrayWindow::enableBreakRequested);

  menu->addSeparator();

  connect(menu->addAction(tr("Preferences")), &QAction::triggered, this,
          &StatusTrayWindow::preferenceWindowRequested);
  connect(menu->addAction(tr("Statistics")), &QAction::triggered, this,
          &StatusTrayWindow::statsRequested);

  quitAction = menu->addAction(tr("Quit"));
  connect(quitAction, &QAction::triggered, this, &StatusTrayWindow::quitRequested);
}

void StatusTrayWindow::update(TrayData data) {
  nextBreakAction->setText(
      tr("Next break after %1").arg(formatTime(data.secondsToNextBreak)));
  bigBreakAction->setText(
      tr("Next big break after %1").arg(formatTime(data.secondsToNextBigBreak)));

  enableBreak->setVisible(data.pauseReasons);
  nextBreakAction->setVisible(!data.isBreaking && !data.pauseReasons &&
                              !data.isInMeeting);
  bigBreakAction->setVisible(data.bigBreakEnabled && !data.isBreaking &&
                             !data.pauseReasons && !data.isInMeeting);
  smallBreakInsteadAction->setVisible(data.bigBreakEnabled && data.isBreaking &&
                                      data.smallBreaksBeforeBigBreak == 0);
  postponeMenu->setVisible(!data.isInMeeting && !data.pauseReasons &&
                           !data.isFocusMode);
  meetingAction->setVisible(!data.isBreaking && !data.isInMeeting &&
                            !data.pauseReasons && !data.isPostponing &&
                            !data.isFocusMode);
  focusAction->setVisible(!data.isBreaking && !data.isInMeeting &&
                          !data.pauseReasons && !data.isPostponing &&
                          !data.isFocusMode);
  endFocusAction->setVisible(data.isFocusMode && !data.isBreaking);
  endMeetingAction->setVisible(data.isInMeeting);
  extendMeetingMenu->menuAction()->setVisible(data.isInMeeting);

  if (data.isFocusMode && !data.isInMeeting) {
    int cyclesDone = data.focusTotalCycles - data.focusCyclesRemaining;
    setTitle(tr("focus: %1 %2 (%3/%4)")
                 .arg(data.smallBreaksBeforeBigBreak == 0 ? tr("big break")
                                                          : tr("break"),
                      formatTime(data.secondsToNextBreak))
                 .arg(cyclesDone)
                 .arg(data.focusTotalCycles));
  } else if (data.isInMeeting) {
    QTime meetingEndTime = QTime::currentTime().addSecs(data.meetingSecondsRemaining);
    QString endTimeStr = QLocale().toString(meetingEndTime, QLocale::ShortFormat);

    if (data.meetingSecondsRemaining > 0) {
      endMeetingAction->setText(tr("Meeting until %1").arg(endTimeStr));
      setTitle(tr("Meeting mode — until %1 (%2 left)")
                   .arg(endTimeStr, formatTime(data.meetingSecondsRemaining)));
    } else {
      endMeetingAction->setText(tr("End Meeting && Break Now"));
      setTitle(tr("Meeting ended — waiting"));
    }
    for (const auto& opt : extendOptions) {
      QTime newEndTime =
          QTime::currentTime().addSecs(data.meetingSecondsRemaining + opt.seconds);
      QString newEndTimeStr = QLocale().toString(newEndTime, QLocale::ShortFormat);
      opt.action->setText(
          tr("+%1 (until %2)").arg(formatTime(opt.seconds), newEndTimeStr));
    }
  } else if (data.pauseReasons) {
    if (data.pauseReasons.testFlag(PauseReason::OnBattery)) {
      setTitle(tr("Paused on battery"));
    } else if (data.pauseReasons.testFlag(PauseReason::AppOpen)) {
      setTitle(tr("Paused on app running"));
    } else if (data.pauseReasons.testFlag(PauseReason::Idle)) {
      setTitle(tr("Paused on idle"));
    }
  } else if (data.bigBreakEnabled) {
    setTitle(QString("%1 %2").arg(
        data.smallBreaksBeforeBigBreak == 0 ? tr("big break") : tr("small break"),
        formatTime(data.secondsToNextBreak)));
  } else {
    setTitle(QString("%1 %2").arg(tr("break"), formatTime(data.secondsToNextBreak)));
  }
}

TrayIconSpec trayIconSpec(TrayData data) {
  if (data.pauseReasons || data.isBreaking ||
      data.secondsToNextBreak > data.secondsFromLastBreakToNext)
    return {.baseIcon = ":/images/icon-pause.png",
            .arc = std::nullopt,
            .dot = std::nullopt};

  if (data.isInMeeting) {
    float arcRatio =
        data.meetingTotalSeconds > 0
            ? float(data.meetingSecondsRemaining) / data.meetingTotalSeconds
            : 0;
    return {
        .baseIcon = ":/images/icon-meeting.png",
        .arc = TrayArcSpec{QColor(8, 47, 73), QColor(224, 242, 254), arcRatio},
        .dot = std::nullopt,
    };
  }

  float arcRatio = float(data.secondsToNextBreak) / data.secondsFromLastBreakToNext;

  if (data.isFocusMode) {
    return {
        .baseIcon = ":/images/icon-focus.png",
        .arc = TrayArcSpec{QColor(0x58, 0x1c, 0x87), QColor(0xf3, 0xe8, 0xff), arcRatio},
        .dot = std::nullopt,
    };
  }

  return {
      .baseIcon = ":/images/icon.png",
      .arc = TrayArcSpec{QColor(5, 46, 22), QColor(220, 252, 231), arcRatio},
      .dot = data.bigBreakEnabled && data.smallBreaksBeforeBigBreak == 0
                 ? std::optional(QColor(202, 138, 4))
                 : std::nullopt,
  };
}

QPixmap renderTrayIcon(TrayIconSpec spec) {
  QPixmap pixmap(spec.baseIcon);
  if (!spec.arc && !spec.dot) return pixmap;

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);

  QRect rect = pixmap.rect();
  rect.setSize(QSize(rect.width() - 8, rect.height() - 8));
  rect.setTopLeft(QPoint(8, 8));

  if (spec.arc) {
    QPen pen(spec.arc->dark);
    pen.setWidth(16);
    painter.setPen(pen);
    painter.drawArc(rect, 0, 360 * 16);

    pen.setColor(spec.arc->light);
    painter.setPen(pen);
    int spanAngle = 360 * 16 * spec.arc->ratio;
    painter.drawArc(rect, 90 * 16, spanAngle);
  }

  if (spec.dot) {
    int dotSize = 64;
    QRect dotRect(pixmap.width() - 12 - dotSize, 12, dotSize, dotSize);
    painter.setBrush(QBrush(*spec.dot));
    QPen pen;
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawEllipse(dotRect);
  }

  return pixmap;
}

StatusTray::StatusTray(SanePreferences* preferences, QObject* parent)
    : StatusTrayWindow(preferences, parent) {
  icon = new QSystemTrayIcon();
  icon->setContextMenu(menu);
  connect(icon, &QSystemTrayIcon::activated, this, &StatusTray::onIconTrigger);

  emptyIconPixmap = QPixmap(32, 32);
  emptyIconPixmap.fill(Qt::transparent);

  flashTimer = new QTimer(this);
  connect(flashTimer, &QTimer::timeout, this,
          [this]() { icon->setIcon(emptyIconPixmap); });
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
  icon->setIcon(renderTrayIcon(trayIconSpec(data)));

  if (data.secondsToNextBreak <= 10 && !data.pauseReasons && !data.isBreaking) {
    if (!flashTimer->isActive()) {
      QTimer::singleShot(500, [this]() {
        icon->setIcon(emptyIconPixmap);
        flashTimer->start(1000);
      });
    }
  } else if (flashTimer->isActive()) {
    flashTimer->stop();
  }
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
  widget->icon->setPixmap(renderTrayIcon(trayIconSpec(data)));
}
void StatusWindow::setTitle(QString str) { widget->info->setText(str); }
