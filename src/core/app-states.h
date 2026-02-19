// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>
#include <memory>
#include <variant>

#include "core/app-data.h"
#include "core/break-windows.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/meeting-prompt.h"
#include "core/preferences.h"
#include "timer.h"

// User-initiated commands dispatched as pure pass-through to the current state via
// onMenuAction. Add new actions here when they originate from UI and need no
// framework-level handling in AppContext. For system/platform events that require
// pre/post-dispatch logic in AppContext (e.g. updating data before delegating), add a
// virtual method to AppState instead.
namespace Action {
struct BreakNow {};
struct BigBreakNow {};
struct SmallBreakInstead {};
struct EnableBreaks {};
struct ExitForceBreak {};
struct EndMeetingBreakLater {
  int seconds = 0;
};
struct ExtendMeeting {
  int seconds;
};
}  // namespace Action

using MenuAction =
    std::variant<Action::BreakNow, Action::BigBreakNow, Action::SmallBreakInstead,
                 Action::EnableBreaks, Action::ExitForceBreak,
                 Action::EndMeetingBreakLater, Action::ExtendMeeting>;

class AppContext;

// Base class for application states managing different modes of operation.
//
// Two event dispatch mechanisms:
// - onMenuAction(MenuAction): for user-initiated commands. AppContext is a pure
// pass-through;
//   states receive the variant directly. Add new user actions as structs in the Action
//   namespace.
// - Virtual methods (onIdleStart, onPauseRequest, etc.): for system/platform events
// where
//   AppContext needs framework-level logic around the dispatch (e.g. onPauseRequest
//   adds pause reasons before delegating, onSleepEnd applies a default fallback). Add
//   new system events as virtual methods here.
class AppState {
 public:
  enum StateID { Normal, Paused, Break, Meeting };
  virtual StateID getID() = 0;
  virtual void enter(AppContext*) {};
  virtual void exit(AppContext*) {};
  virtual void tick(AppContext*) {};
  virtual void onIdleStart(AppContext*) {};
  virtual void onIdleEnd(AppContext*) {};
  // Returns true if the state fully handled the sleep event, false to apply the default
  // behavior (pause → add slept seconds → resume).
  virtual bool onSleepEnd(AppContext*, int) { return false; };
  virtual void onMenuAction(AppContext*, MenuAction) {};
  virtual void onPauseRequest(AppContext*, PauseReasons) {};
  virtual void onResumeRequest(AppContext*, PauseReasons) {};
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
  AbstractMeetingPrompt* meetingPrompt;
  BreakDatabase* db;

  void transitionTo(std::unique_ptr<AppState> state);
  void exitCurrentState();
  void checkBreakReadiness();

 protected:
  std::unique_ptr<AppState> m_currentState;

  void tick();
  void onIdleStart();
  void onIdleEnd();
  void onMenuAction(MenuAction action);
  void onSleepEnd(int sleptSeconds);
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
  void onResumeRequest(AppContext* app, PauseReasons reasons) override;
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

// Meeting state: break schedule suspended during a meeting/presentation
class AppStateMeeting : public AppState {
 public:
  StateID getID() override { return Meeting; }
  void enter(AppContext* app) override;
  void exit(AppContext* app) override;
  void tick(AppContext* app) override;
  bool onSleepEnd(AppContext* app, int sleptSeconds) override;
  void onMenuAction(AppContext* app, MenuAction action) override;
};
