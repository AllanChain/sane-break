// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <qglobal.h>
#include <qlogging.h>

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStyleHints>
#include <QTimer>
#include <QWindow>
#include <Qt>

#include "app/window-control.h"
#include "core/app.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "gui/pref-window.h"
#include "gui/tray.h"
#include "gui/widgets/language-select.h"
#include "lib/screen-lock.h"
#include "lib/system-monitor.h"
#include "lib/timer.h"

SaneBreakApp::SaneBreakApp(const AppDependencies &deps, QObject *parent)
    : AbstractApp(deps, parent) {
  prefWindow = new PreferenceWindow(preferences);
  tray = StatusTrayWindow::createTrayOrWindow(preferences, this);

  connect(this, &SaneBreakApp::trayDataUpdated, tray, &StatusTrayWindow::update);
  connect(tray, &StatusTrayWindow::nextBreakRequested, this, &SaneBreakApp::breakNow);
  connect(tray, &StatusTrayWindow::nextBigBreakRequested, this,
          &SaneBreakApp::bigBreakNow);
  connect(tray, &StatusTrayWindow::smallBreakInsteadRequested, this,
          &SaneBreakApp::smallBreakInstead);
  connect(tray, &StatusTrayWindow::postponeRequested, this, &SaneBreakApp::postpone);
  connect(tray, &StatusTrayWindow::preferenceWindowRequested, this,
          &SaneBreakApp::showPreferences);
  connect(tray, &StatusTrayWindow::enableBreakRequested, this,
          &SaneBreakApp::enableBreak);
  connect(tray, &StatusTrayWindow::quitRequested, this, &SaneBreakApp::confirmQuit);
  connect(preferences->language, &SettingWithSignal::changed, this,
          [this]() { LanguageSelect::setLanguage(preferences->language->get()); });
  connect(this, &SaneBreakApp::quit, qApp, &QApplication::quit, Qt::QueuedConnection);
}

SaneBreakApp *SaneBreakApp::create(SanePreferences *preferences, QObject *parent) {
  auto countDownTimer = new Timer();
  auto oneshotIdleTimer = SystemIdleTime::createIdleTimer();
  auto screenLockTimer = new Timer();
  auto systemMonitor = new SystemMonitor(preferences);
  auto windowControl = BreakWindowControl::create(preferences);

  AppDependencies deps = {
      .preferences = preferences,
      .countDownTimer = countDownTimer,
      .oneshotIdleTimer = oneshotIdleTimer,
      .screenLockTimer = screenLockTimer,
      .systemMonitor = systemMonitor,
      .windowControl = windowControl,
  };
  return new SaneBreakApp(deps, parent);
}

void SaneBreakApp::start() {
  AbstractApp::start();
  tray->show();
}

void SaneBreakApp::doLockScreen() { lockScreen(); }

void SaneBreakApp::showPreferences() {
  prefWindow->show();
  prefWindow->windowHandle()->raise();
  prefWindow->windowHandle()->requestActivate();
}

void SaneBreakApp::confirmQuit() {
  int largestMinutes = 0;
  for (const QString &minuteString : preferences->postponeMinutes->get()) {
    int minute = minuteString.toInt();
    largestMinutes = largestMinutes > minute ? largestMinutes : minute;
  }
  QMessageBox msgBox;
  msgBox.setText(tr("Are you sure to quit Sane Break?"));
  msgBox.setInformativeText(tr("You can postpone the breaks instead."));
  msgBox.setIcon(QMessageBox::Icon::Question);
  msgBox.addButton(QMessageBox::Cancel)->setText(tr("Cancel"));
  msgBox.addButton(tr("Postpone %n min", "", largestMinutes), QMessageBox::NoRole);
  msgBox.addButton(QMessageBox::Yes)->setText(tr("Yes"));

  msgBox.setDefaultButton(QMessageBox::Cancel);
  switch (msgBox.exec()) {
    case QMessageBox::Yes:
      emit quit();
      return;
    case QMessageBox::Cancel:
      return;
    default:
      postpone(largestMinutes * 60);
      return;
  }
}
