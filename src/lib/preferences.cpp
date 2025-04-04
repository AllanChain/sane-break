// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

#include <QCoreApplication>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

SanePreferences* SanePreferences::createDefault(QObject* parent) {
  // We prefer settings file next to the app executable to make app more portable
  QFile portableSettings(QCoreApplication::applicationDirPath() + "/SaneBreak.ini");
  if (!portableSettings.exists()) {
    return new SanePreferences(new QSettings());
  }
  return new SanePreferences(
      new QSettings(portableSettings.fileName(), QSettings::IniFormat));
};

SanePreferences::SanePreferences(QSettings* settings, QObject* parent)
    : QObject(parent), settings(settings) {
  shownWelcome = new Setting<bool>(settings, "shown-welcome", false);
  smallEvery = new Setting<int>(settings, "break/small-every", 1200);
  smallFor = new Setting<int>(settings, "break/small-for", 20);
  bigAfter = new Setting<int>(settings, "break/big-after", 3);
  bigFor = new Setting<int>(settings, "break/big-for", 60);
  flashFor = new Setting<int>(settings, "break/flash-for", 30);
  confirmAfter = new Setting<int>(settings, "break/confirm-after", 30);
  postponeMinutes = new Setting<QStringList>(settings, "break/postpone-minutes",
                                             QStringList({"5", "10", "30", "60"}));
  autoScreenLock = new Setting<int>(settings, "break/auto-screen-lock", 0);
  quickBreak = new Setting<bool>(settings, "break/quick-break", false);
  flashSpeed = new Setting<int>(settings, "break/flash-speed", 100);
  textTransparency = new Setting<int>(settings, "break/text-transparency", 0);
  smallStartBell = new Setting<QString>(settings, "bell/small-start", "");
  smallEndBell = new Setting<QString>(settings, "bell/small-end", "");
  bigStartBell = new Setting<QString>(settings, "bell/start", "");
  bigEndBell = new Setting<QString>(settings, "bell/end", "");
  pauseOnIdleFor = new Setting<int>(settings, "pause/on-idle-for", 180);
  resetAfterPause = new Setting<int>(settings, "pause/reset-after", 120);
  resetCycleAfterPause = new Setting<int>(settings, "pause/reset-cycle-after", 300);
  pauseOnBattery = new Setting<bool>(settings, "pause/on-battery", false);
  programsToMonitor =
      new Setting<QStringList>(settings, "pause/programs-to-monitor", QStringList());
  language = new Setting<QString>(settings, "language", "");
  autoStart = new Setting<bool>(settings, "auto-start", false);
}
