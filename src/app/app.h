// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

#include "app/pref-window.h"
#include "app/tray.h"
#include "core/app.h"
#include "core/preferences.h"

class SaneBreakApp : public AbstractApp {
  Q_OBJECT
 public:
  SaneBreakApp(const AppDependencies& deps, QObject* parent = nullptr);
  ~SaneBreakApp() = default;
  static SaneBreakApp* create(SanePreferences* preferences, QObject* parent = nullptr);

  void start() override;
  void doLockScreen() override;
  void openPostponeWindow();
  void openMeetingWindow();

  void showPreferences();

 signals:
  void quit();

 private:
  PreferenceWindow* prefWindow;
  StatusTrayWindow* tray;
  void confirmQuit();
};
