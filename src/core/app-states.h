// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>
#include <memory>

#include "core/app-data.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "timer.h"

enum class MenuAction {
  BreakNow,
  BigBreakNow,
  SmallBreakInstead,
  EnableBreaks,
  ExitForceBreak,
};

class AppContext;

class AppState {
 public:
  enum StateID { Normal, Paused, Break };
  virtual StateID getID() = 0;
  virtual void enter(AppContext *) {};
  virtual void exit(AppContext *) {};
  virtual void tick(AppContext *) {};
  virtual void onIdleStart(AppContext *) {};
  virtual void onIdleEnd(AppContext *) {};
  virtual void onMenuAction(AppContext *, MenuAction) {};
  virtual void onPauseRequest(AppContext *, PauseReasons) {};
  virtual void OnResumeRequest(AppContext *, PauseReasons) {};
  virtual ~AppState() = default;
};

// Minimal interface for app used by states
class AppContext : public QObject {
  Q_OBJECT
 public:
  using QObject::QObject;
  AppData *data;
  SanePreferences *preferences;
  SystemIdleTime *idleTimer;
  AbstractTimer *screenLockTimer;
  AbstractBreakWindows *breakWindows;

  void transitionTo(std::unique_ptr<AppState> state);

 protected:
  std::unique_ptr<AppState> m_currentState;

  void tick();
  void onIdleStart();
  void onIdleEnd();
  void onMenuAction(MenuAction action);
  void onPauseRequest(PauseReasons reasons);
  void onResumeRequest(PauseReasons reasons);
};

class AppStateNormal : public AppState {
 public:
  StateID getID() override { return Normal; };
  void enter(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleStart(AppContext *app) override;
  void onMenuAction(AppContext *app, MenuAction action) override;
  void onPauseRequest(AppContext *app, PauseReasons reasons) override;
};

class AppStatePaused : public AppState {
 public:
  StateID getID() override { return Paused; };
  void exit(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleStart(AppContext *app) override;
  void onIdleEnd(AppContext *app) override;
  void onMenuAction(AppContext *app, MenuAction action) override;
  void OnResumeRequest(AppContext *app, PauseReasons reasons) override;
};

class BreakPhase;

class AppStateBreak : public AppState {
 public:
  void transitionTo(AppContext *app, std::unique_ptr<BreakPhase> phase);

  StateID getID() override { return Break; };
  void enter(AppContext *app) override;
  void exit(AppContext *app) override;
  void tick(AppContext *app) override;
  void onIdleStart(AppContext *app) override;
  void onIdleEnd(AppContext *app) override;
  void onMenuAction(AppContext *app, MenuAction action) override;
  void onPauseRequest(AppContext *app, PauseReasons reasons) override;

  void initBreakData(AppContext *);

 protected:
  std::unique_ptr<BreakPhase> m_currentPhase;
};

class BreakPhase {
 public:
  virtual void enter(AppContext *, AppStateBreak *) {};
  // Break phases should not have exit handlers. It will makes things complicated when
  // transiting to other AppState.
  virtual void tick(AppContext *, AppStateBreak *) {};
  virtual void onIdleStart(AppContext *, AppStateBreak *) {};
  virtual void onIdleEnd(AppContext *, AppStateBreak *) {};
  virtual ~BreakPhase() = default;
};

class BreakPhasePrompt : public BreakPhase {
 public:
  void enter(AppContext *app, AppStateBreak *breakState) override;
  void tick(AppContext *app, AppStateBreak *breakState) override;
  void onIdleStart(AppContext *app, AppStateBreak *breakState) override;
};

class BreakPhaseFullScreen : public BreakPhase {
 public:
  void enter(AppContext *app, AppStateBreak *breakState) override;
  void tick(AppContext *app, AppStateBreak *breakState) override;
  void onIdleEnd(AppContext *app, AppStateBreak *breakState) override;
  bool canExitOnActivity(AppContext *app);
  void showWindowClickableWidgets(AppContext *app);
};

class BreakPhasePost : public BreakPhase {
 public:
  void enter(AppContext *app, AppStateBreak *breakState) override;
  void onIdleEnd(AppContext *app, AppStateBreak *breakState) override;
};
