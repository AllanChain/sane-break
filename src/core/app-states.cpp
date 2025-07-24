// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-states.h"

#include <memory>
#include <utility>

#include "core/flags.h"

void AppContext::transitionTo(std::unique_ptr<AppState> state) {
  if (m_currentState) m_currentState->exit(this);
  m_currentState = std::move(state);
  m_currentState->enter(this);
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
  app->data->addPauseReasons(SaneBreak::PauseReason::Idle);
  app->transitionTo(std::make_unique<AppStatePaused>());
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
  int nextBreakDuration = (app->data->breakType() == SaneBreak::BreakType::Big
                               ? app->preferences->bigFor->get()
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
}
void AppStatePaused::tick(AppContext *app) { app->data->tickSecondsPaused(); }
void AppStatePaused::onIdleEnd(AppContext *app) {
  app->data->removePauseReasons(SaneBreak::PauseReason::Idle);
  if (!app->data->pauseReasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}

void AppStateBreak::enter(AppContext *app) {
  // Finer accuracy for idle detection
  app->idleTimer->setWatchAccuracy(500);
  app->idleTimer->setMinIdleTime(2000);
  app->data->breaks->init({
      .totalSeconds = app->data->breakType() == SaneBreak::BreakType::Big
                          ? app->preferences->bigFor->get()
                          : app->preferences->smallFor->get(),
      .flashFor = app->preferences->flashFor->get(),
  });
  app->windowControl->createWindows(app->data->breakType());
  app->transitionTo(std::make_unique<AppStateBreakPrompt>());
}

void AppStateBreakNormalExit::enter(AppContext *app) {
  app->windowControl->deleteWindows();
  app->data->finishAndStartNextCycle();
  if (app->idleTimer->isIdle()) {
    // We don't count down immediately after break. We wait for user activities.
    app->data->addPauseReasons(SaneBreak::PauseReason::Idle);
    app->transitionTo(std::make_unique<AppStatePaused>());
  } else {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}

void AppStateBreakPrompt::enter(AppContext *app) {
  app->windowControl->showFlashPrompt();
}
void AppStateBreakPrompt::tick(AppContext *app) {
  app->data->breaks->tickSecondsToForceBreak();
  if (app->data->breaks->isForceBreak()) {
    app->transitionTo(std::make_unique<AppStateBreakFullScreen>());
  }
}
void AppStateBreakPrompt::onIdleStart(AppContext *app) {
  app->transitionTo(std::make_unique<AppStateBreakFullScreen>());
}

void AppStateBreakFullScreen::enter(AppContext *app) {
  app->windowControl->showFullScreen();
}
void AppStateBreakFullScreen::tick(AppContext *app) {
  app->data->breaks->tickRemainingTime();
}
void AppStateBreakFullScreen::onIdleEnd(AppContext *app) {
  if (!app->data->breaks->isForceBreak()) {
    app->transitionTo(std::make_unique<AppStateBreakPrompt>());
  }
}
