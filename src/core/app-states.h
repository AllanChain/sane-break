// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>
#include <memory>

#include "core/app-data.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"

class AppContext;

class AppState {
 public:
  virtual void enter(AppContext *) {};
  virtual void exit(AppContext *) {};
  virtual void tick(AppContext *) {};
  virtual void onIdleStart(AppContext *) {};
  virtual void onIdleEnd(AppContext *) {};
  virtual ~AppState() = default;
};

class AbstractWindowControl {
 public:
  virtual void createWindows(SaneBreak::BreakType type) = 0;
  virtual void deleteWindows() = 0;
  virtual void showFullScreen() = 0;
  virtual void showFlashPrompt() = 0;
};

// Minimal interface for app used by states
class AppContext {
 public:
  AppData *data;
  SanePreferences *preferences;
  SystemIdleTime *idleTimer;
  AbstractWindowControl *windowControl;

  void transitionTo(std::unique_ptr<AppState> state);

 protected:
  std::unique_ptr<AppState> m_currentState;
};

class AppStateNormal : public AppState {
 public:
  void enter(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleStart(AppContext *app) override;
};

class AppStatePaused : public AppState {
 public:
  void exit(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleEnd(AppContext *app) override;
};

// Nested states only makes things complicated. We just flatten the AppStateBreak into
// four states: enter, prompt, fullscreen, exit
class AppStateBreak : public AppState {
 public:
  void enter(AppContext *app) override;
};

class AppStateBreakPrompt : public AppState {
 public:
  void enter(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleStart(AppContext *app) override;
};

class AppStateBreakFullScreen : public AppState {
 public:
  void enter(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleEnd(AppContext *app) override;
};

class AppStateBreakNormalExit : public AppState {
  void enter(AppContext *app) override;
};
