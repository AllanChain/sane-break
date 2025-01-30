// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

#include <QCoreApplication>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QtContainerFwd>

Setting<bool>* SanePreferences::shownWelcome =
    new Setting<bool>("shown-welcome", false);
QSettings getSettings() {
  // We prefer settings file next to the app executable to make app more portable
  QFile portableSettings(QCoreApplication::applicationDirPath() + "/SaneBreak.ini");
  if (!portableSettings.exists()) return QSettings();
  return QSettings(portableSettings.fileName(), QSettings::IniFormat);
};

Setting<int>* SanePreferences::smallEvery = new Setting<int>("break/small-every", 1200);
Setting<int>* SanePreferences::smallFor = new Setting<int>("break/small-for", 20);
Setting<int>* SanePreferences::bigAfter = new Setting<int>("break/big-after", 3);
Setting<int>* SanePreferences::bigFor = new Setting<int>("break/big-for", 60);
Setting<int>* SanePreferences::flashFor = new Setting<int>("break/flash-for", 30);
Setting<int>* SanePreferences::confirmAfter =
    new Setting<int>("break/confirm-after", 30);
Setting<int>* SanePreferences::autoScreenLock =
    new Setting<int>("break/auto-screen-lock", 0);
Setting<bool>* SanePreferences::quickBreak =
    new Setting<bool>("break/quick-break", false);
Setting<QString>* SanePreferences::language = new Setting<QString>("language", "");
Setting<QString>* SanePreferences::smallStartBell =
    new Setting<QString>("bell/small-start", "");
Setting<QString>* SanePreferences::smallEndBell =
    new Setting<QString>("bell/small-end", "");
Setting<QString>* SanePreferences::bigStartBell =
    new Setting<QString>("bell/start", "");
Setting<QString>* SanePreferences::bigEndBell = new Setting<QString>("bell/end", "");
Setting<int>* SanePreferences::pauseOnIdleFor =
    new Setting<int>("pause/on-idle-for", 180);
Setting<int>* SanePreferences::resetAfterPause =
    new Setting<int>("pause/reset-after", 120);
Setting<int>* SanePreferences::resetCycleAfterPause =
    new Setting<int>("pause/reset-cycle-after", 300);
Setting<bool>* SanePreferences::pauseOnBattery =
    new Setting<bool>("pause/on-battery", false);
Setting<QStringList>* SanePreferences::programsToMonitor =
    new Setting<QStringList>("pause/programs-to-monitor", QStringList());
