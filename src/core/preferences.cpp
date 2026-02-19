// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/preferences.h"

#include <QColor>
#include <QCoreApplication>
#include <QList>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

SanePreferences* SanePreferences::createDefault(QObject* parent) {
  // We prefer settings file next to the app executable to make app more portable
  QFile portableSettings(QCoreApplication::applicationDirPath() + "/SaneBreak.ini");
  if (!portableSettings.exists()) {
    return new SanePreferences(new QSettings(), parent);
  }
  return new SanePreferences(
      new QSettings(portableSettings.fileName(), QSettings::IniFormat), parent);
};

SanePreferences::SanePreferences(QSettings* settings, QObject* parent)
    : QObject(parent), settings(settings) {
  shownWelcome = new Setting<bool>(settings, "shown-welcome", false);

  smallEvery = new Setting<int>(settings, "break/small-every", 1200);
  smallFor = new Setting<int>(settings, "break/small-for", 20);
  bigBreakEnabled = new Setting<bool>(settings, "break/big-enabled", true);
  bigAfter = new Setting<int>(settings, "break/big-after", 3);
  bigFor = new Setting<int>(settings, "break/big-for", 60);

  postponeMaxMinutePercent =
      new Setting<int>(settings, "postpone/max-minute-ratio", 50);
  postponeShrinkNextPercent =
      new Setting<int>(settings, "postpone/shrink-next-session-ratio", 100);
  postponeExtendBreakPercent =
      new Setting<int>(settings, "postpone/extend-break-ratio", 100);

  flashFor = new Setting<int>(settings, "break/flash-for", 30);
  confirmAfter = new Setting<int>(settings, "break/confirm-after", 30);
  flashSpeed = new Setting<int>(settings, "break/flash-speed", 120);
  countDownColor =
      new Setting<QColor>(settings, "theme/count-down", QColor(236, 239, 244, 255));
  messageColor =
      new Setting<QColor>(settings, "theme/message", QColor(236, 239, 244, 255));
  backgroundColor =
      new Setting<QColor>(settings, "theme/background", QColor(46, 52, 64, 255));
  smallHighlightColor =
      new Setting<QColor>(settings, "theme/small-bg", QColor(235, 203, 139, 100));
  bigHighlightColor =
      new Setting<QColor>(settings, "theme/big-bg", QColor(180, 142, 173, 100));
  smallMessages = new Setting<QStringList>(settings, "break/small-msg", []() {
    return QStringList({tr("Time for a small break")});
  });
  bigMessages = new Setting<QStringList>(settings, "break/big-msg", []() {
    return QStringList({tr("Time for a big break")});
  });
  maxForceBreakExits = new Setting<int>(settings, "break/max-force-break-exits", 2);
  autoCloseWindowAfterSmallBreak =
      new Setting<bool>(settings, "break/auto-close-window-after-small-break", true);
  autoCloseWindowAfterBigBreak =
      new Setting<bool>(settings, "break/auto-close-window-after-big-break", true);

  smallBreakShowProgressBar =
      new Setting<bool>(settings, "ui/small-break-show-progress-bar", true);
  smallBreakShowCountdown =
      new Setting<bool>(settings, "ui/small-break-show-countdown", true);
  smallBreakShowClock = new Setting<bool>(settings, "ui/small-break-show-clock", false);
  smallBreakShowEndTime =
      new Setting<bool>(settings, "ui/small-break-show-end-time", false);
  smallBreakShowButtons =
      new Setting<bool>(settings, "ui/small-break-show-buttons", true);
  bigBreakShowProgressBar =
      new Setting<bool>(settings, "ui/big-break-show-progress-bar", true);
  bigBreakShowCountdown =
      new Setting<bool>(settings, "ui/big-break-show-countdown", true);
  bigBreakShowClock = new Setting<bool>(settings, "ui/big-break-show-clock", false);
  bigBreakShowEndTime =
      new Setting<bool>(settings, "ui/big-break-show-end-time", false);
  bigBreakShowButtons = new Setting<bool>(settings, "ui/big-break-show-buttons", true);

  pauseOnIdleFor = new Setting<int>(settings, "pause/on-idle-for", 180);
  resetAfterPause = new Setting<int>(settings, "pause/reset-after", 120);
  resetCycleAfterPause = new Setting<int>(settings, "pause/reset-cycle-after", 300);
  pauseOnBattery = new Setting<bool>(settings, "pause/on-battery", false);
  programsToMonitor =
      new Setting<QStringList>(settings, "pause/programs-to-monitor", QStringList());

  smallStartBell = new Setting<QString>(settings, "bell/small-start", "");
  smallEndBell = new Setting<QString>(settings, "bell/small-end", "");
  bigStartBell = new Setting<QString>(settings, "bell/start", "");
  bigEndBell = new Setting<QString>(settings, "bell/end", "");

  language = new Setting<QString>(settings, "language", "");
  autoStart = new Setting<bool>(settings, "auto-start", false);
  autoScreenLock = new Setting<int>(settings, "break/auto-screen-lock", 0);
  quickBreak = new Setting<bool>(settings, "break/quick-break", false);
}
