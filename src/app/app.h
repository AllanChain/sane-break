// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

#include "app/window-control.h"
#include "core/app.h"
#include "core/preferences.h"
#include "gui/pref-window.h"
#include "gui/tray.h"

class SaneBreakApp : public AbstractApp {
  Q_OBJECT
 public:
  SaneBreakApp(const AppDependencies &deps, QObject *parent = nullptr);
  ~SaneBreakApp() = default;
  static SaneBreakApp *create(SanePreferences *preferences, QObject *parent = nullptr);

  void start() override;
  void doLockScreen() override;

  void showPreferences();

 signals:
  void quit();

 private:
  PreferenceWindow *prefWindow;
  StatusTrayWindow *tray;
  void confirmQuit();
};
