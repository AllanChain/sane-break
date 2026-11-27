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
template <typename T>
Setting<T>* makeSetting(SanePreferences* owner, QSettings* settings, const QString& key,
                        const T& defaultValue,
                        typename Setting<T>::Validator validator = {}) {
  return new Setting<T>(settings, key, defaultValue, std::move(validator), owner);
}

template <typename T>
Setting<T>* makeSetting(SanePreferences* owner, QSettings* settings, const QString& key,
                        std::function<T()> defaultValueFunction,
                        typename Setting<T>::Validator validator = {}) {
  return new Setting<T>(settings, key, std::move(defaultValueFunction),
                        std::move(validator), owner);
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
  if (settings && !settings->parent()) settings->setParent(this);
  migrateSettings(settings);
  shownWelcome = makeSetting<bool>(this, settings, "shown-welcome", false);

  smallEvery = makeSetting<int>(this, settings, "break/small-every", 1200, positiveInt);
  smallFor = makeSetting<int>(this, settings, "break/small-for", 20, positiveInt);
  bigBreakEnabled = makeSetting<bool>(this, settings, "break/big-enabled", true);
  bigAfter = makeSetting<int>(this, settings, "break/big-after", 3, positiveInt);
  bigFor = makeSetting<int>(this, settings, "break/big-for", 60, positiveInt);

  focusSmallEvery =
      makeSetting<int>(this, settings, "focus/small-every", 1200, positiveInt);
  focusSmallFor = makeSetting<int>(this, settings, "focus/small-for", 20, positiveInt);
  focusBigBreakEnabled = makeSetting<bool>(this, settings, "focus/big-enabled", false);
  focusBigAfter = makeSetting<int>(this, settings, "focus/big-after", 3, positiveInt);
  focusBigFor = makeSetting<int>(this, settings, "focus/big-for", 60, positiveInt);

  postponeMaxMinutePercent =
      makeSetting<int>(this, settings, "postpone/max-minute-ratio", 50, nonNegativeInt);
  postponeShrinkNextPercent = makeSetting<int>(
      this, settings, "postpone/shrink-next-session-ratio", 100, nonNegativeInt);
  postponeExtendBreakPercent = makeSetting<int>(
      this, settings, "postpone/extend-break-ratio", 100, nonNegativeInt);

  minReasonLength =
      makeSetting<int>(this, settings, "reason/min-length", 0, nonNegativeInt);

  flashFor = makeSetting<int>(this, settings, "break/flash-for", 30, positiveInt);
  confirmAfter =
      makeSetting<int>(this, settings, "break/confirm-after", 30, positiveInt);
  flashTrayFor =
      makeSetting<int>(this, settings, "break/flash-tray-for", 30, nonNegativeInt);
  headsUpFor =
      makeSetting<int>(this, settings, "break/heads-up-for", 30, nonNegativeInt);
  flashSpeed =
      makeSetting<int>(this, settings, "break/flash-speed", 120, nonNegativeInt);
  countDownColor = makeSetting<QColor>(this, settings, "theme/count-down",
                                       QColor(236, 239, 244, 255));
  messageColor =
      makeSetting<QColor>(this, settings, "theme/message", QColor(236, 239, 244, 255));
  smallBackgroundColor = makeSetting<QColor>(this, settings, "theme/small-background",
                                             QColor(46, 52, 64, 255));
  bigBackgroundColor = makeSetting<QColor>(this, settings, "theme/big-background",
                                           QColor(46, 52, 64, 255));
  smallFlashColor = makeSetting<QColor>(this, settings, "theme/small-flash",
                                        QColor(235, 203, 139, 100));
  bigFlashColor = makeSetting<QColor>(this, settings, "theme/big-flash",
                                      QColor(180, 142, 173, 100));
  smallBackgroundImage =
      makeSetting<QString>(this, settings, "theme/small-background-image", "");
  bigBackgroundImage =
      makeSetting<QString>(this, settings, "theme/big-background-image", "");
  smallMessages = makeSetting<QStringList>(
      this, settings, "break/small-msg", std::function<QStringList()>([]() {
        return QStringList({tr("Time for a small break")});
      }));
  bigMessages = makeSetting<QStringList>(
      this, settings, "break/big-msg", std::function<QStringList()>([]() {
        return QStringList({tr("Time for a big break")});
      }));
  maxForceBreakExits = makeSetting<int>(this, settings, "break/max-force-break-exits",
                                        2, nonNegativeInt);
  autoCloseWindowAfterSmallBreak = makeSetting<bool>(
      this, settings, "break/auto-close-window-after-small-break", true);
  autoCloseWindowAfterBigBreak = makeSetting<bool>(
      this, settings, "break/auto-close-window-after-big-break", true);

  smallBreakShowProgressBar =
      makeSetting<bool>(this, settings, "ui/small-break-show-progress-bar", true);
  smallBreakShowCountdown =
      makeSetting<bool>(this, settings, "ui/small-break-show-countdown", true);
  smallBreakShowClock =
      makeSetting<bool>(this, settings, "ui/small-break-show-clock", false);
  smallBreakShowEndTime =
      makeSetting<bool>(this, settings, "ui/small-break-show-end-time", false);
  smallBreakShowButtons =
      makeSetting<bool>(this, settings, "ui/small-break-show-buttons", true);
  bigBreakShowProgressBar =
      makeSetting<bool>(this, settings, "ui/big-break-show-progress-bar", true);
  bigBreakShowCountdown =
      makeSetting<bool>(this, settings, "ui/big-break-show-countdown", true);
  bigBreakShowClock =
      makeSetting<bool>(this, settings, "ui/big-break-show-clock", false);
  bigBreakShowEndTime =
      makeSetting<bool>(this, settings, "ui/big-break-show-end-time", false);
  bigBreakShowButtons =
      makeSetting<bool>(this, settings, "ui/big-break-show-buttons", true);

  pauseOnIdleFor =
      makeSetting<int>(this, settings, "pause/on-idle-for", 180, positiveInt);
  resetAfterPause =
      makeSetting<int>(this, settings, "pause/reset-after", 120, positiveInt);
  resetCycleAfterPause =
      makeSetting<int>(this, settings, "pause/reset-cycle-after", 300, positiveInt);
  pauseOnBattery = makeSetting<bool>(this, settings, "pause/on-battery", false);
  treatInhibitorAsActivity =
      makeSetting<bool>(this, settings, "pause/treat-inhibitor-as-activity", true);
  programsToMonitor = makeSetting<QStringList>(
      this, settings, "pause/programs-to-monitor", QStringList());
  pauseOnUnknownMonitor =
      makeSetting<bool>(this, settings, "pause/on-unknown-monitor", false);
  knownMonitors =
      makeSetting<QStringList>(this, settings, "pause/known-monitors", QStringList());

  smallStartBell = makeSetting<QString>(this, settings, "bell/small-start", "");
  smallEndBell = makeSetting<QString>(this, settings, "bell/small-end", "");
  bigStartBell = makeSetting<QString>(this, settings, "bell/start", "");
  bigEndBell = makeSetting<QString>(this, settings, "bell/end", "");

  language = makeSetting<QString>(this, settings, "language", "");
  autoStart = makeSetting<bool>(this, settings, "auto-start", false);
  autoScreenLock =
      makeSetting<int>(this, settings, "break/auto-screen-lock", 0, nonNegativeInt);
  quickBreak = makeSetting<bool>(this, settings, "break/quick-break", false);
  hookScript = makeSetting<QString>(this, settings, "hook-script", "");
}

BreakTypeSettings SanePreferences::breakSettings(BreakType type) const {
  if (type == BreakType::Big) {
    return {
        .backgroundColor = bigBackgroundColor,
        .flashColor = bigFlashColor,
        .backgroundImage = bigBackgroundImage,
        .messages = bigMessages,
        .showProgressBar = bigBreakShowProgressBar,
        .showCountdown = bigBreakShowCountdown,
        .showClock = bigBreakShowClock,
        .showEndTime = bigBreakShowEndTime,
        .showButtons = bigBreakShowButtons,
        .autoCloseWindow = autoCloseWindowAfterBigBreak,
        .startBell = bigStartBell,
        .endBell = bigEndBell,
    };
  }
  return {
      .backgroundColor = smallBackgroundColor,
      .flashColor = smallFlashColor,
      .backgroundImage = smallBackgroundImage,
      .messages = smallMessages,
      .showProgressBar = smallBreakShowProgressBar,
      .showCountdown = smallBreakShowCountdown,
      .showClock = smallBreakShowClock,
      .showEndTime = smallBreakShowEndTime,
      .showButtons = smallBreakShowButtons,
      .autoCloseWindow = autoCloseWindowAfterSmallBreak,
      .startBell = smallStartBell,
      .endBell = smallEndBell,
  };
}
