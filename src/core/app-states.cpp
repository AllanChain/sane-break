// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-states.h"

#include <memory>
#include <utility>
#include <variant>

#include "core/app-data.h"
#include "core/break-windows.h"
#include "core/flags.h"

void AppContext::transitionTo(std::unique_ptr<AppState> state) {
  if (m_currentState && m_currentState->getID() == state->getID()) {
    return;  // no need to transition
  }
  if (m_currentState) m_currentState->exit(this);
  m_currentState = std::move(state);
  m_currentState->enter(this);
}

void AppContext::exitCurrentState() {
  if (m_currentState) m_currentState->exit(this);
}

void AppStateBreak::transitionTo(AppContext* app, std::unique_ptr<BreakPhase> phase) {
  if (m_currentPhase) m_currentPhase->exit(app, this);
  m_currentPhase = std::move(phase);
  m_currentPhase->enter(app, this);
}

void AppContext::tick() { m_currentState->tick(this); }
void AppContext::onIdleStart() { m_currentState->onIdleStart(this); }
void AppContext::onIdleEnd() { m_currentState->onIdleEnd(this); }
void AppContext::onMenuAction(MenuAction action) {
  m_currentState->onMenuAction(this, action);
}
void AppContext::onSleepEnd(int sleptSeconds) {
  if (m_currentState->onSleepEnd(this, sleptSeconds)) return;
  // Default: treat sleep as a pause/resume cycle
  onPauseRequest(PauseReason::Sleep);
  data->addSecondsPaused(sleptSeconds);
  onResumeRequest(PauseReason::Sleep);
}
void AppContext::onPauseRequest(PauseReasons reasons) {
  data->addPauseReasons(reasons);
  m_currentState->onPauseRequest(this, reasons);
}
void AppContext::onResumeRequest(PauseReasons reasons) {
  data->removePauseReasons(reasons);
  m_currentState->onResumeRequest(this, reasons);
}

void AppStateNormal::enter(AppContext* app) {
  app->db->logEvent("normal::start");
  // Use low accuracy (5s) for idle detection in normal state as it can last a long time
  app->idleTimer->setWatchAccuracy(5000);
  app->idleTimer->setMinIdleTime(app->preferences->pauseOnIdleFor->get() * 1000);
}
void AppStateNormal::exit(AppContext* app) { app->db->logEvent("normal::end"); }
void AppStateNormal::tick(AppContext* app) {
  app->data->tickSecondsToNextBreak();
  if (app->data->secondsToNextBreak() <= 0)
    app->transitionTo(std::make_unique<AppStateBreak>());
}
void AppStateNormal::onIdleStart(AppContext* app) {
  // When in postpone mode, disable pausing
  if (app->data->isPostponing()) return;
  app->data->addPauseReasons(PauseReason::Idle);
  app->transitionTo(std::make_unique<AppStatePaused>());
}
void AppStateNormal::onPauseRequest(AppContext* app, PauseReasons) {
  // When in postpone mode, disable pausing
  if (app->data->isPostponing()) return;
  app->transitionTo(std::make_unique<AppStatePaused>());
}
void AppStateNormal::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::BreakNow>(&action)) {
    app->data->earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  } else if (std::get_if<Action::BigBreakNow>(&action)) {
    app->data->earlyBreak();
    if (app->data->effectiveBigBreakEnabled()) app->data->makeNextBreakBig();
    app->transitionTo(std::make_unique<AppStateBreak>());
  } else if (std::get_if<Action::EndFocus>(&action)) {
    app->data->setFocusCyclesRemaining(1);
    app->data->earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  }
}

// The paused state will inherit the idle time settings from the state before:
// - For normal state to paused: low accuracy because the pause may be long
// - For break state to paused: high accuracy because we want it to be instant
// Moreover, setting idle interval will reset the event-based watchers.
void AppStatePaused::enter(AppContext* app) { app->db->logEvent("pause::start"); }

/* To avoid immediate breaks after break resume, we consider the user have already
 * taken the break if there shall be breaks during the pause. Of course, if the user
 * have configured `resetAfterPause`, we just reset it.
 */
void AppStatePaused::exit(AppContext* app) {
  app->db->logEvent("pause::end");
  // Calculate the total duration of the next break to determine if breaks would have
  // occurred during pause
  int nextBreakDuration =
      (app->data->breakType() == BreakType::Big ? app->data->effectiveBigFor()
                                                : app->data->effectiveSmallFor());
  int secondsToNextBreakEnd = app->data->secondsToNextBreak() + nextBreakDuration;
  // If user was paused longer than the break duration or longer than resetAfterPause
  // setting, refill the break timer to avoid immediate breaks
  if (app->data->secondsPaused() > secondsToNextBreakEnd ||
      app->data->secondsPaused() > app->preferences->resetAfterPause->get()) {
    app->data->refillSecondsToNextBreak();
  }
  // If user was paused longer than resetCycleAfterPause setting, reset the entire break
  // cycle
  if (app->data->secondsPaused() > app->preferences->resetCycleAfterPause->get()) {
    app->data->resetBreakCycle();
  }
  app->data->resetSecondsPaused();
  // Ensure pause reasons are cleared on exit
  app->data->clearPauseReasons();
}
void AppStatePaused::tick(AppContext* app) { app->data->tickSecondsPaused(); }
void AppStatePaused::onIdleStart(AppContext* app) {
  app->data->addPauseReasons(PauseReason::Idle);
}
void AppStatePaused::onIdleEnd(AppContext* app) {
  // We need to clear screenLockTimer when going through break -> paused -> idleEnd
  app->screenLockTimer->stop();
  app->data->removePauseReasons(PauseReason::Idle);
  if (!app->data->pauseReasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}
void AppStatePaused::onResumeRequest(AppContext* app, PauseReasons) {
  if (!app->data->pauseReasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}
void AppStatePaused::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::EnableBreaks>(&action)) {
    app->data->clearPauseReasons();
    app->transitionTo(std::make_unique<AppStateNormal>());
  } else if (std::get_if<Action::EndFocus>(&action)) {
    app->data->clearPauseReasons();
    app->data->setFocusCyclesRemaining(1);
    app->data->earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  }
}

void AppStateBreak::enter(AppContext* app) {
  app->db->logEvent(
      "break::start",
      {{"type", app->data->breakType() == BreakType::Big ? "big" : "small"}});
  data = std::make_unique<BreaksData>(dataInit(app));
  // On focus entry break, exhaust force break exits so the exit button is hidden
  if (app->data->isFocusMode() &&
      app->data->focusCyclesRemaining() == app->data->focusTotalCycles()) {
    for (int i = 0; i < app->preferences->maxForceBreakExits->get(); i++)
      data->recordForceBreakExit();
  }
  // Use high accuracy (500ms) for idle detection during breaks for responsive
  // transitions
  app->idleTimer->setWatchAccuracy(500);
  app->idleTimer->setMinIdleTime(2000);
  app->breakWindows->create(app->data->breakType(), app->preferences);
  // Ensure we set the time at least once to initialize the UI in case user is idle and
  // no tick occurs
  app->breakWindows->setTime(data->remainingSeconds());
  app->breakWindows->playEnterSound(app->data->breakType(), app->preferences);
  if (app->idleTimer->isIdle()) {
    this->transitionTo(app, std::make_unique<BreakPhaseFullScreen>());
  } else {
    this->transitionTo(app, std::make_unique<BreakPhasePrompt>());
  }
}
void AppStateBreak::exit(AppContext* app) {
  if (m_currentPhase) m_currentPhase->exit(app, this);
  app->db->logEvent("break::end", {{"normal-exit", (data->remainingSeconds() <= 0)}});
  app->breakWindows->destroy();
}
void AppStateBreak::tick(AppContext* app) { m_currentPhase->tick(app, this); }
void AppStateBreak::onIdleStart(AppContext* app) {
  m_currentPhase->onIdleStart(app, this);
}
void AppStateBreak::onIdleEnd(AppContext* app) { m_currentPhase->onIdleEnd(app, this); }
void AppStateBreak::onPauseRequest(AppContext* app, PauseReasons reasons) {
  // We don't exit break if request pause on idle - continue with break instead
  if (reasons != PauseReason::Idle) {
    // For non-idle pause requests, finish current break and transition to paused state
    app->data->finishAndStartNextCycle();
    app->transitionTo(std::make_unique<AppStatePaused>());
  }
}
BreaksDataInit AppStateBreak::dataInit(AppContext* app) {
  int flashFor = app->preferences->flashFor->get();
  // On focus entry break (first break when entering focus mode), force immediate
  // full-screen by setting flashFor to 0
  if (app->data->isFocusMode() &&
      app->data->focusCyclesRemaining() == app->data->focusTotalCycles()) {
    flashFor = 0;
  }
  return {
      .totalSeconds = app->data->breakDuration(),
      .flashFor = flashFor,
  };
}
void AppStateBreak::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::SmallBreakInstead>(&action)) {
    this->exit(app);
    app->data->makeNextBreakLastSmallBeforeBig();
    app->db->logEvent("break::small-instead");
    this->enter(app);
  } else if (std::get_if<Action::ExitForceBreak>(&action)) {
    data->init(dataInit(app));
    data->recordForceBreakExit();
    app->db->logEvent("break::exit-force");
    this->transitionTo(app, std::make_unique<BreakPhasePrompt>());
  }
}

void BreakPhasePrompt::enter(AppContext* app, AppStateBreak*) {
  app->db->logEvent("break::flash::start");
  app->breakWindows->showFlashPrompt();
  app->breakWindows->showButtons(AbstractBreakWindows::Button::ExitForceBreak |
                                     AbstractBreakWindows::Button::LockScreen,
                                 false);
  // screenLockTimer should only be active in BreakPhaseFullScreen and AppStatePaused
  app->screenLockTimer->stop();
}
void BreakPhasePrompt::exit(AppContext* app, AppStateBreak*) {
  app->db->logEvent("break::flash::end");
}
void BreakPhasePrompt::tick(AppContext* app, AppStateBreak* breakState) {
  breakState->data->tickSecondsToForceBreak();
  app->breakWindows->setTime(breakState->data->remainingSeconds());
  if (breakState->data->isForceBreak()) {
    breakState->transitionTo(app, std::make_unique<BreakPhaseFullScreen>());
  }
}
void BreakPhasePrompt::onIdleStart(AppContext* app, AppStateBreak* breakState) {
  breakState->transitionTo(app, std::make_unique<BreakPhaseFullScreen>());
}

void BreakPhaseFullScreen::enter(AppContext* app, AppStateBreak* breakState) {
  app->breakWindows->showFullScreen();
  if (!canExitOnActivity(app, breakState)) {
    showWindowClickableWidgets(app, breakState);
  }
  if (int secs = app->preferences->autoScreenLock->get(); secs > 0) {
    app->screenLockTimer->start(secs * 1000);
  }
}
void BreakPhaseFullScreen::tick(AppContext* app, AppStateBreak* breakState) {
  breakState->data->tickRemainingTime();
  app->breakWindows->setTime(breakState->data->remainingSeconds());
  if (!canExitOnActivity(app, breakState)) {
    showWindowClickableWidgets(app, breakState);
  }
  if (breakState->data->remainingSeconds() <= 0) {
    app->breakWindows->playExitSound(app->data->breakType(), app->preferences);
    // record break type before we start next cycle
    auto breakType = app->data->breakType();
    app->data->finishAndStartNextCycle();
    if (app->idleTimer->isIdle()) {
      // Check if window should auto-close based on preferences and break type
      bool shouldCloseWindow =
          (breakType == BreakType::Small &&
           app->preferences->autoCloseWindowAfterSmallBreak->get()) ||
          (breakType == BreakType::Big &&
           app->preferences->autoCloseWindowAfterBigBreak->get());
      if (!shouldCloseWindow) {
        // Leave break window open until user activities
        breakState->transitionTo(app, std::make_unique<BreakPhasePost>());
      } else {
        // We don't count down immediately after break. We wait for user activities.
        app->data->addPauseReasons(PauseReason::Idle);
        app->transitionTo(std::make_unique<AppStatePaused>());
      }
    } else {
      app->screenLockTimer->stop();
      app->transitionTo(std::make_unique<AppStateNormal>());
    }
  }
}
void BreakPhaseFullScreen::onIdleEnd(AppContext* app, AppStateBreak* breakState) {
  if (canExitOnActivity(app, breakState)) {
    breakState->data->resetRemainingTime();
    breakState->transitionTo(app, std::make_unique<BreakPhasePrompt>());
  }
}
bool BreakPhaseFullScreen::canExitOnActivity(AppContext* app,
                                             AppStateBreak* breakState) {
  // Allow exit full screen on user activity if: not in force break AND time elapsed is
  // less than confirmAfter setting.
  return !breakState->data->isForceBreak() &&
         (breakState->data->totalSeconds() - breakState->data->remainingSeconds() <
          app->preferences->confirmAfter->get());
}
void BreakPhaseFullScreen::showWindowClickableWidgets(AppContext* app,
                                                      AppStateBreak* breakState) {
  AbstractBreakWindows::Buttons buttons = AbstractBreakWindows::Button::LockScreen;
  if (breakState->data->numberForceBreakExits() <
      app->preferences->maxForceBreakExits->get()) {
    buttons |= AbstractBreakWindows::Button::ExitForceBreak;
  }
  app->breakWindows->showButtons(buttons);
}

void BreakPhasePost::enter(AppContext* app, AppStateBreak*) {
  app->breakWindows->showButtons(AbstractBreakWindows::Button::ExitForceBreak |
                                     AbstractBreakWindows::Button::LockScreen,
                                 false);
}
void BreakPhasePost::onIdleEnd(AppContext* app, AppStateBreak*) {
  app->screenLockTimer->stop();
  app->transitionTo(std::make_unique<AppStateNormal>());
}

void AppStateMeeting::enter(AppContext* app) {
  app->db->logEvent("meeting::enter");
  app->data->resetSecondsToNextBreak();
  app->idleTimer->setWatchAccuracy(5000);
  app->idleTimer->setMinIdleTime(app->preferences->pauseOnIdleFor->get() * 1000);
}

void AppStateMeeting::exit(AppContext* app) {
  app->db->logEvent("meeting::exit");
  app->data->clearMeetingData();
  app->meetingPrompt->closeEndPrompt();
}

void AppStateMeeting::tick(AppContext* app) {
  if (app->data->meetingSecondsRemaining() > 0) {
    app->data->tickMeetingRemaining();
    if (app->data->meetingSecondsRemaining() <= 0) {
      app->meetingPrompt->showEndPrompt();
      return;
    }
  }
}

void AppStateMeeting::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::EndMeetingBreakNow>(&action)) {
    app->db->logEvent("meeting::end", {{"next-break", 0}});
    if (app->data->effectiveBigBreakEnabled()) app->data->makeNextBreakBig();
    app->data->earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  } else if (auto* a = std::get_if<Action::EndMeetingBreakLater>(&action)) {
    app->db->logEvent("meeting::end", {{"next-break", a->seconds}});
    if (app->data->effectiveBigBreakEnabled()) app->data->makeNextBreakBig();
    app->data->setSecondsToNextBreak(a->seconds);
    app->transitionTo(std::make_unique<AppStateNormal>());
  } else if (auto* a = std::get_if<Action::ExtendMeeting>(&action)) {
    app->db->logEvent("meeting::extend", {{"seconds", a->seconds}});
    app->data->extendMeeting(a->seconds);
  }
}
bool AppStateMeeting::onSleepEnd(AppContext* app, int sleptSeconds) {
  int breakDuration = app->data->effectiveBigBreakEnabled()
                          ? app->data->effectiveBigFor()
                          : app->data->effectiveSmallFor();
  int skipIfSleptFor = app->data->meetingSecondsRemaining() + breakDuration;
  if (sleptSeconds >= skipIfSleptFor) {
    app->db->logEvent("meeting::end", {{"next-break", -1}});
    app->data->resetBreakCycle();
    app->data->resetSecondsToNextBreak();
    app->meetingPrompt->closeEndPrompt();
    app->transitionTo(std::make_unique<AppStateNormal>());
    return true;
  }
  if (!app->meetingPrompt->isShowing()) {
    app->data->subtractMeetingRemaining(sleptSeconds);
  } else {
    app->meetingPrompt->resetTimeout();
  }
  return true;
}
