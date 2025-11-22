// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>
#include <memory>

#include "core/app-data.h"
#include "core/break-windows.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "timer.h"

// Actions that can be triggered from the application menu or UI
enum class MenuAction {
  BreakNow,           // Trigger a small break immediately
  BigBreakNow,        // Trigger a big break immediately
  SmallBreakInstead,  // Change the current big break to a small one
  EnableBreaks,       // Clear all pauses and resume normal break schedule
  ExitForceBreak,     // Exit the force break
};

class AppContext;

// Base class for application states managing different modes of operation
class AppState {
 public:
  enum StateID { Normal, Paused, Break };
  virtual StateID getID() = 0;
  virtual void enter(AppContext*) {};
  virtual void exit(AppContext*) {};
  virtual void tick(AppContext*) {};
  virtual void onIdleStart(AppContext*) {};
  virtual void onIdleEnd(AppContext*) {};
  virtual void onMenuAction(AppContext*, MenuAction) {};
  virtual void onPauseRequest(AppContext*, PauseReasons) {};
  virtual void OnResumeRequest(AppContext*, PauseReasons) {};
  virtual ~AppState() = default;
};

// Minimal interface for app used by states
class AppContext : public QObject {
  Q_OBJECT
 public:
  using QObject::QObject;
  AppData* data;
  SanePreferences* preferences;
  SystemIdleTime* idleTimer;
  AbstractTimer* screenLockTimer;
  AbstractBreakWindows* breakWindows;
  BreakDatabase* db;

  void transitionTo(std::unique_ptr<AppState> state);
  void exitCurrentState();

 protected:
  std::unique_ptr<AppState> m_currentState;

  void tick();
  void onIdleStart();
  void onIdleEnd();
  void onMenuAction(MenuAction action);
  void onPauseRequest(PauseReasons reasons);
  void onResumeRequest(PauseReasons reasons);
};

// Normal state: actively counting down time
class AppStateNormal : public AppState {
 public:
  StateID getID() override { return Normal; };
  void enter(AppContext* app) override;
  void exit(AppContext* app) override;
  void tick(AppContext* app) override;
  void onIdleStart(AppContext* app) override;
  void onMenuAction(AppContext* app, MenuAction action) override;
  void onPauseRequest(AppContext* app, PauseReasons reasons) override;
};

// Paused state: break schedule is temporarily suspended
class AppStatePaused : public AppState {
 public:
  StateID getID() override { return Paused; };
  void enter(AppContext* app) override;
  void exit(AppContext* app) override;
  void tick(AppContext* app) override;
  void onIdleStart(AppContext* app) override;
  void onIdleEnd(AppContext* app) override;
  void onMenuAction(AppContext* app, MenuAction action) override;
  void OnResumeRequest(AppContext* app, PauseReasons reasons) override;
};

class BreakPhase;

// Break state: managing the active break with different phases
class AppStateBreak : public AppState {
 public:
  void transitionTo(AppContext* app, std::unique_ptr<BreakPhase> phase);

  StateID getID() override { return Break; };
  void enter(AppContext* app) override;
  void exit(AppContext* app) override;
  void tick(AppContext* app) override;
  void onIdleStart(AppContext* app) override;
  void onIdleEnd(AppContext* app) override;
  void onMenuAction(AppContext* app, MenuAction action) override;
  void onPauseRequest(AppContext* app, PauseReasons reasons) override;

  BreaksDataInit dataInit(AppContext*);
  std::unique_ptr<BreaksData> data;

 protected:
  std::unique_ptr<BreakPhase> m_currentPhase;
};

// Base class for break phases (prompt, fullscreen, post-break)
class BreakPhase {
 public:
  virtual void enter(AppContext*, AppStateBreak*) {};
  virtual void exit(AppContext*, AppStateBreak*) {};
  virtual void tick(AppContext*, AppStateBreak*) {};
  virtual void onIdleStart(AppContext*, AppStateBreak*) {};
  virtual void onIdleEnd(AppContext*, AppStateBreak*) {};
  virtual ~BreakPhase() = default;
};

// Prompt phase: Shows a flash prompt to get user's attention before full screen
class BreakPhasePrompt : public BreakPhase {
 public:
  void enter(AppContext* app, AppStateBreak* breakState) override;
  void exit(AppContext* app, AppStateBreak* breakState) override;
  void tick(AppContext* app, AppStateBreak* breakState) override;
  void onIdleStart(AppContext* app, AppStateBreak* breakState) override;
};

// Fullscreen phase: Overlay until break is completed or exit for some reason
class BreakPhaseFullScreen : public BreakPhase {
 public:
  void enter(AppContext* app, AppStateBreak* breakState) override;
  void tick(AppContext* app, AppStateBreak* breakState) override;
  void onIdleEnd(AppContext* app, AppStateBreak* breakState) override;
  bool canExitOnActivity(AppContext* app, AppStateBreak* breakState);
  void showWindowClickableWidgets(AppContext* app, AppStateBreak* breakState);
};

// Post-break phase: Keeps window open after break completion until user activity
class BreakPhasePost : public BreakPhase {
 public:
  void enter(AppContext* app, AppStateBreak* breakState) override;
  void onIdleEnd(AppContext* app, AppStateBreak* breakState) override;
};
