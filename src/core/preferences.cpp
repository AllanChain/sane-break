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
#include <initializer_list>

namespace {
// Validators for integer settings. Return std::nullopt when valid, otherwise a
// human-readable reason. A rejected value falls back to the setting's default.
auto positiveInt = [](int v) -> std::optional<QString> {
  if (v > 0) return std::nullopt;
  return QStringLiteral("must be a positive whole number");
};
auto nonNegativeInt = [](int v) -> std::optional<QString> {
  if (v >= 0) return std::nullopt;
  return QStringLiteral("must not be negative");
};

void migrateSettings(QSettings* settings) {
  auto copy = [settings](const char* legacy,
                         std::initializer_list<const char*> targets) {
    if (!settings->contains(legacy)) return;
    for (const char* target : targets) {
      if (settings->contains(target)) continue;
      settings->setValue(target, settings->value(legacy));
    }
  };
  copy("theme/background", {"theme/small-background", "theme/big-background"});
  copy("theme/background-image",
       {"theme/small-background-image", "theme/big-background-image"});
  copy("theme/small-bg", {"theme/small-flash"});
  copy("theme/big-bg", {"theme/big-flash"});
}
}  // namespace

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
  migrateSettings(settings);
  shownWelcome = new Setting<bool>(settings, "shown-welcome", false);

  smallEvery = new Setting<int>(settings, "break/small-every", 1200, positiveInt);
  smallFor = new Setting<int>(settings, "break/small-for", 20, positiveInt);
  bigBreakEnabled = new Setting<bool>(settings, "break/big-enabled", true);
  bigAfter = new Setting<int>(settings, "break/big-after", 3, positiveInt);
  bigFor = new Setting<int>(settings, "break/big-for", 60, positiveInt);

  focusSmallEvery = new Setting<int>(settings, "focus/small-every", 1200, positiveInt);
  focusSmallFor = new Setting<int>(settings, "focus/small-for", 20, positiveInt);
  focusBigBreakEnabled = new Setting<bool>(settings, "focus/big-enabled", false);
  focusBigAfter = new Setting<int>(settings, "focus/big-after", 3, positiveInt);
  focusBigFor = new Setting<int>(settings, "focus/big-for", 60, positiveInt);

  postponeMaxMinutePercent =
      new Setting<int>(settings, "postpone/max-minute-ratio", 50, nonNegativeInt);
  postponeShrinkNextPercent = new Setting<int>(
      settings, "postpone/shrink-next-session-ratio", 100, nonNegativeInt);
  postponeExtendBreakPercent =
      new Setting<int>(settings, "postpone/extend-break-ratio", 100, nonNegativeInt);

  minReasonLength = new Setting<int>(settings, "reason/min-length", 0, nonNegativeInt);

  flashFor = new Setting<int>(settings, "break/flash-for", 30, positiveInt);
  confirmAfter = new Setting<int>(settings, "break/confirm-after", 30, positiveInt);
  flashTrayFor = new Setting<int>(settings, "break/flash-tray-for", 30, nonNegativeInt);
  headsUpFor = new Setting<int>(settings, "break/heads-up-for", 30, nonNegativeInt);
  flashSpeed = new Setting<int>(settings, "break/flash-speed", 120, nonNegativeInt);
  countDownColor =
      new Setting<QColor>(settings, "theme/count-down", QColor(236, 239, 244, 255));
  messageColor =
      new Setting<QColor>(settings, "theme/message", QColor(236, 239, 244, 255));
  smallBackgroundColor =
      new Setting<QColor>(settings, "theme/small-background", QColor(46, 52, 64, 255));
  bigBackgroundColor =
      new Setting<QColor>(settings, "theme/big-background", QColor(46, 52, 64, 255));
  smallFlashColor =
      new Setting<QColor>(settings, "theme/small-flash", QColor(235, 203, 139, 100));
  bigFlashColor =
      new Setting<QColor>(settings, "theme/big-flash", QColor(180, 142, 173, 100));
  smallBackgroundImage =
      new Setting<QString>(settings, "theme/small-background-image", "");
  bigBackgroundImage = new Setting<QString>(settings, "theme/big-background-image", "");
  smallMessages = new Setting<QStringList>(settings, "break/small-msg", []() {
    return QStringList({tr("Time for a small break")});
  });
  bigMessages = new Setting<QStringList>(settings, "break/big-msg", []() {
    return QStringList({tr("Time for a big break")});
  });
  maxForceBreakExits =
      new Setting<int>(settings, "break/max-force-break-exits", 2, nonNegativeInt);
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

  pauseOnIdleFor = new Setting<int>(settings, "pause/on-idle-for", 180, positiveInt);
  resetAfterPause = new Setting<int>(settings, "pause/reset-after", 120, positiveInt);
  resetCycleAfterPause =
      new Setting<int>(settings, "pause/reset-cycle-after", 300, positiveInt);
  pauseOnBattery = new Setting<bool>(settings, "pause/on-battery", false);
  treatInhibitorAsActivity =
      new Setting<bool>(settings, "pause/treat-inhibitor-as-activity", true);
  programsToMonitor =
      new Setting<QStringList>(settings, "pause/programs-to-monitor", QStringList());
  pauseOnUnknownMonitor =
      new Setting<bool>(settings, "pause/on-unknown-monitor", false);
  knownMonitors =
      new Setting<QStringList>(settings, "pause/known-monitors", QStringList());

  smallStartBell = new Setting<QString>(settings, "bell/small-start", "");
  smallEndBell = new Setting<QString>(settings, "bell/small-end", "");
  bigStartBell = new Setting<QString>(settings, "bell/start", "");
  bigEndBell = new Setting<QString>(settings, "bell/end", "");

  language = new Setting<QString>(settings, "language", "");
  autoStart = new Setting<bool>(settings, "auto-start", false);
  autoScreenLock =
      new Setting<int>(settings, "break/auto-screen-lock", 0, nonNegativeInt);
  quickBreak = new Setting<bool>(settings, "break/quick-break", false);
}
