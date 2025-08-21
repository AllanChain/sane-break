// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-states.h"

#include <memory>
#include <utility>

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

void AppStateBreak::transitionTo(AppContext *app, std::unique_ptr<BreakPhase> phase) {
  // Break phases does not have exit handlers. It will makes things complicated when
  // transiting to other AppState.
  m_currentPhase = std::move(phase);
  m_currentPhase->enter(app, this);
}

void AppContext::tick() { m_currentState->tick(this); }
void AppContext::onIdleStart() { m_currentState->onIdleStart(this); }
void AppContext::onIdleEnd() { m_currentState->onIdleEnd(this); }
void AppContext::onMenuAction(MenuAction action) {
  m_currentState->onMenuAction(this, action);
}
void AppContext::onPauseRequest(PauseReasons reasons) {
  data->addPauseReasons(reasons);
  m_currentState->onPauseRequest(this, reasons);
}
void AppContext::onResumeRequest(PauseReasons reasons) {
  data->removePauseReasons(reasons);
  m_currentState->OnResumeRequest(this, reasons);
}

void AppStateNormal::enter(AppContext *app) {
  // Set up idle timer to detect idle pauses. Low accuracy is fine
  app->idleTimer->setWatchAccuracy(5000);
  app->idleTimer->setMinIdleTime(app->preferences->pauseOnIdleFor->get() * 1000);
}
void AppStateNormal::tick(AppContext *app) {
  // TODO: Check
  app->data->tickSecondsSinceLastBreak();
  app->data->tickSecondsToNextBreak();
  if (app->data->secondsToNextBreak() <= 0)
    app->transitionTo(std::make_unique<AppStateBreak>());
}
void AppStateNormal::onIdleStart(AppContext *app) {
  app->data->addPauseReasons(PauseReason::Idle);
  app->transitionTo(std::make_unique<AppStatePaused>());
}
void AppStateNormal::onPauseRequest(AppContext *app, PauseReasons) {
  app->transitionTo(std::make_unique<AppStatePaused>());
}
void AppStateNormal::onMenuAction(AppContext *app, MenuAction action) {
  switch (action) {
    case (MenuAction::BreakNow):
      app->data->zeroSecondsToNextBreak();
      app->transitionTo(std::make_unique<AppStateBreak>());
      break;
    case (MenuAction::BigBreakNow):
      app->data->zeroSecondsToNextBreak();
      app->data->makeNextBreakBig();
      app->transitionTo(std::make_unique<AppStateBreak>());
      break;
    default:
      break;
  }
}

// The paused state will inherit the idle time settings from the state before:
// - For normal state to paused: low accuracy because the pause may be long
// - For break state to paused: high accuracy because we want it to be instant
// Moreover, setting idle interval will reset the event-based watchers.

/* To avoid immediate breaks after break resume, we consider the user have already
 * taken the break if there shall be breaks during the pause. Of course, if the user
 * have configured `resetAfterPause`, we just reset it.
 */
void AppStatePaused::exit(AppContext *app) {
  int nextBreakDuration =
      (app->data->breakType() == BreakType::Big ? app->preferences->bigFor->get()
                                                : app->preferences->smallFor->get());
  int secondsToNextBreakEnd = app->data->secondsToNextBreak() + nextBreakDuration;
  if (app->data->secondsPaused() > secondsToNextBreakEnd ||
      app->data->secondsPaused() > app->preferences->resetAfterPause->get()) {
    app->data->refillSecondsToNextBreak();
  }
  if (app->data->secondsPaused() > app->preferences->resetCycleAfterPause->get()) {
    app->data->resetBreakCycle();
  }
  app->data->resetSecondsPaused();
  // Ensure pause reasons are cleared on exit
  app->data->clearPauseReasons();
}
void AppStatePaused::tick(AppContext *app) { app->data->tickSecondsPaused(); }
void AppStatePaused::onIdleStart(AppContext *app) {
  app->data->addPauseReasons(PauseReason::Idle);
}
void AppStatePaused::onIdleEnd(AppContext *app) {
  // We need to clear screenLockTimer when going through break -> paused -> idleEnd
  app->screenLockTimer->stop();
  app->data->removePauseReasons(PauseReason::Idle);
  if (!app->data->pauseReasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}
void AppStatePaused::OnResumeRequest(AppContext *app, PauseReasons) {
  if (!app->data->pauseReasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}
void AppStatePaused::onMenuAction(AppContext *app, MenuAction action) {
  switch (action) {
    case (MenuAction::EnableBreaks):
      app->data->clearPauseReasons();
      app->transitionTo(std::make_unique<AppStateNormal>());
      break;
    default:
      break;
  }
}

void AppStateBreak::enter(AppContext *app) {
  // Finer accuracy for idle detection
  app->idleTimer->setWatchAccuracy(500);
  app->idleTimer->setMinIdleTime(2000);
  initBreakData(app);
  app->breakWindows->create(app->data->breakType(), app->preferences);
  // Ensure we set the time at least once. Sometimes there is no tick called to set the
  // time if the user is idle.
  app->breakWindows->setTime(app->data->breaks->remainingSeconds());
  app->breakWindows->playEnterSound(app->data->breakType(), app->preferences);
  if (app->idleTimer->isIdle()) {
    this->transitionTo(app, std::make_unique<BreakPhaseFullScreen>());
  } else {
    this->transitionTo(app, std::make_unique<BreakPhasePrompt>());
  }
}
void AppStateBreak::exit(AppContext *app) { app->breakWindows->destroy(); }
void AppStateBreak::tick(AppContext *app) { m_currentPhase->tick(app, this); }
void AppStateBreak::onIdleStart(AppContext *app) {
  m_currentPhase->onIdleStart(app, this);
}
void AppStateBreak::onIdleEnd(AppContext *app) { m_currentPhase->onIdleEnd(app, this); }
void AppStateBreak::onPauseRequest(AppContext *app, PauseReasons reasons) {
  // We don't exit break if request pause on idle
  if (reasons != PauseReason::Idle) {
    app->data->finishAndStartNextCycle();
    app->transitionTo(std::make_unique<AppStatePaused>());
  }
}
void AppStateBreak::initBreakData(AppContext *app) {
  app->data->breaks->init({
      .totalSeconds = app->data->breakType() == BreakType::Big
                          ? app->preferences->bigFor->get()
                          : app->preferences->smallFor->get(),
      .flashFor = app->preferences->flashFor->get(),
  });
}
void AppStateBreak::onMenuAction(AppContext *app, MenuAction action) {
  switch (action) {
    case (MenuAction::SmallBreakInstead):
      this->exit(app);
      app->data->makeNextBreakLastSmallBeforeBig();
      this->enter(app);
      break;
    case (MenuAction::ExitForceBreak):
      initBreakData(app);
      app->data->breaks->recordForceBreakExit();
      this->transitionTo(app, std::make_unique<BreakPhasePrompt>());
      break;
    default:
      break;
  }
}

void BreakPhasePrompt::enter(AppContext *app, AppStateBreak *) {
  app->breakWindows->showFlashPrompt();
  // screenLockTimer should only be active in BreakPhaseFullScreen and AppStatePaused
  app->screenLockTimer->stop();
}
void BreakPhasePrompt::tick(AppContext *app, AppStateBreak *breakState) {
  app->data->breaks->tickSecondsToForceBreak();
  app->breakWindows->setTime(app->data->breaks->remainingSeconds());
  if (app->data->breaks->isForceBreak()) {
    breakState->transitionTo(app, std::make_unique<BreakPhaseFullScreen>());
  }
}
void BreakPhasePrompt::onIdleStart(AppContext *app, AppStateBreak *breakState) {
  breakState->transitionTo(app, std::make_unique<BreakPhaseFullScreen>());
}

void BreakPhaseFullScreen::enter(AppContext *app, AppStateBreak *) {
  app->breakWindows->showFullScreen();
  if (!canExitOnActivity(app)) {
    showWindowClickableWidgets(app);
  }
  if (int secs = app->preferences->autoScreenLock->get(); secs > 0) {
    app->screenLockTimer->start(secs * 1000);
  }
}
void BreakPhaseFullScreen::tick(AppContext *app, AppStateBreak *breakState) {
  app->data->breaks->tickRemainingTime();
  app->breakWindows->setTime(app->data->breaks->remainingSeconds());
  if (!canExitOnActivity(app)) {
    showWindowClickableWidgets(app);
  }
  if (app->data->breaks->remainingSeconds() <= 0) {
    app->breakWindows->playExitSound(app->data->breakType(), app->preferences);
    // record break type before we start next cycle
    auto breakType = app->data->breakType();
    app->data->finishAndStartNextCycle();
    if (app->idleTimer->isIdle()) {
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
void BreakPhaseFullScreen::onIdleEnd(AppContext *app, AppStateBreak *breakState) {
  if (canExitOnActivity(app)) {
    app->data->breaks->resetRemainingTime();
    breakState->transitionTo(app, std::make_unique<BreakPhasePrompt>());
  }
}
bool BreakPhaseFullScreen::canExitOnActivity(AppContext *app) {
  // Not in force break and not confirmed
  return !app->data->breaks->isForceBreak() &&
         (app->data->breaks->totalSeconds() - app->data->breaks->remainingSeconds() <
          app->preferences->confirmAfter->get());
}
void BreakPhaseFullScreen::showWindowClickableWidgets(AppContext *app) {
  AbstractBreakWindows::Buttons buttons = AbstractBreakWindows::Button::LockScreen;
  if (app->data->breaks->numberForceBreakExits() <
      app->preferences->maxForceBreakExits->get()) {
    buttons |= AbstractBreakWindows::Button::ExitForceBreak;
  }
  app->breakWindows->showButtons(buttons);
}

void BreakPhasePost::enter(AppContext *app, AppStateBreak *) {
  app->breakWindows->showButtons(AbstractBreakWindows::Button::ExitForceBreak, false);
}
void BreakPhasePost::onIdleEnd(AppContext *app, AppStateBreak *) {
  app->screenLockTimer->stop();
  app->transitionTo(std::make_unique<AppStateNormal>());
}
