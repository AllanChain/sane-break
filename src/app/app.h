// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_APP_H
#define SANE_APP_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include "gui/pref-window.h"
#include "gui/tray.h"
#include "gui/window-manager.h"
#include "lib/app-core.h"
#include "lib/preferences.h"
#include "lib/system-monitor.h"

class Timer : public ITimer {
  Q_OBJECT
 public:
  Timer(QObject *parent = nullptr) : ITimer(parent) {
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ITimer::timeout);
  };
  ~Timer() = default;
  void start() override { timer->start(); };
  void start(int msec) override { timer->start(msec); };
  void stop() override { timer->stop(); };
  bool isActive() override { return timer->isActive(); };
  void setInterval(int msec) override { timer->setInterval(msec); };
  int interval() override { return timer->interval(); };
  void setSingleShot(bool singleShot) override { timer->setSingleShot(singleShot); };
  bool isSingleShot() override { return timer->isSingleShot(); };

 private:
  QTimer *timer;
};

class SaneBreakApp : public AbstractApp {
  Q_OBJECT
 public:
  SaneBreakApp(const AppDependencies &deps, QObject *parent = nullptr);
  ~SaneBreakApp() = default;
  static SaneBreakApp *create(SanePreferences *preferences, QObject *parent = nullptr);

  void start() override;
  void openBreakWindow(bool isBigBreak) override;
  void closeBreakWindow() override;
  void updateTray() override;
  void mayLockScreen() override;

  void confirmQuit();
  void onIconTrigger();
  void onBatterySettingChange();
  void onPostponeMinutesChange();

 signals:
  void quit();

 private:
  PreferenceWindow *prefWindow;
  BreakWindowManager *breakManager;
  SystemMonitor *systemMonitor;
  StatusTrayWindow *tray;
  QMenu *menu;
  QMenu *postponeMenu;
  QAction *quitAction;
  QAction *nextBreakAction;
  QAction *bigBreakAction;
  QAction *enableBreak;
  void createMenu();
};

#endif  // SANE_APP_H
