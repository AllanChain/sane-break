// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>
#include <QTimer>

#include "app/pref-window.h"
#include "app/tray.h"
#include "core/app.h"
#include "core/preferences.h"
#include "focus-window.h"
#include "meeting-prompt.h"
#include "meeting-window.h"
#include "postpone-window.h"
#include "stats-window.h"

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
  void openFocusWindow();

  void showPreferences();
  void openStatsWindow();

 signals:
  void quit();

 private:
  PreferenceWindow* prefWindow;
  StatusTrayWindow* tray;
  QPointer<FocusWindow> focusWindow;
  QPointer<PostponeWindow> postponeWindow;
  QPointer<MeetingWindow> meetingWindow;
  QPointer<StatsWindow> statsWindow;
  void confirmQuit();
};
