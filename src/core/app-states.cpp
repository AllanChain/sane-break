// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-states.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <memory>
#include <utility>
#include <variant>

#include "core/app-data.h"
#include "core/break-windows.h"
#include "core/flags.h"

namespace {

QString pauseReasonName(PauseReason reason) {
  switch (reason) {
    case PauseReason::Idle:
      return "idle";
    case PauseReason::OnBattery:
      return "on-battery";
    case PauseReason::AppOpen:
      return "app-open";
    case PauseReason::Sleep:
      return "sleep";
    case PauseReason::UnknownMonitor:
      return "unknown-monitor";
  }
  return "unknown";
}

QJsonArray pauseReasonsToJson(PauseReasons reasons) {
  QJsonArray reasonNames;
  for (PauseReason reason :
       {PauseReason::Idle, PauseReason::OnBattery, PauseReason::AppOpen,
        PauseReason::Sleep, PauseReason::UnknownMonitor}) {
    if (reasons.testFlag(reason)) reasonNames.append(pauseReasonName(reason));
  }
  return reasonNames;
}

QString effectivePauseSpanType(PauseReasons reasons) {
  // Sleep is tracked as its own span in AppContext::onSleepEnd().
  if (reasons.testFlag(PauseReason::Idle)) return "away";
  return "paused";
}

QJsonObject pausedSpanData(PauseReasons reasons) {
  return {{"reasons", pauseReasonsToJson(reasons)}};
}

}  // namespace

void AppContext::transitionTo(std::unique_ptr<AppState> state) {
  if (m_currentState && m_currentState->getID() == state->getID()) {
    return;  // no need to transition
  }
  if (m_currentState) m_currentState->exit(this);
  m_currentState = std::move(state);
  m_currentState->enter(this);
  emit appStateChanged();
}

void AppContext::exitCurrentState() {
  if (m_currentState) m_currentState->exit(this);
}

void AppStateBreak::transitionTo(AppContext* app, std::unique_ptr<BreakPhase> phase) {
  if (m_currentPhase) m_currentPhase->exit(app, this);
  m_currentPhase = std::move(phase);
  m_currentPhase->enter(app, this);
}

BreakCompletion AppStateBreak::completeBreak(AppContext* app) {
  int focusSpanId = app->data->focus().spanId();
  BreakCompletion completion = app->data->completeBreak();
  if (completion.focusCompleted) {
    app->db->closeSpan(focusSpanId, {{"reason", "completed"}});
  }
  return completion;
}

void AppContext::tick() { m_currentState->tick(this); }
void AppContext::onIdleStart() { m_currentState->onIdleStart(this); }
void AppContext::onIdleEnd() { m_currentState->onIdleEnd(this); }
void AppContext::onMenuAction(MenuAction action) {
  m_currentState->onMenuAction(this, action);
}
int AppContext::openCurrentSpan(const QString& type, const QJsonObject& data,
                                const QDateTime& startTime) {
  currentSpanId = db->openSpan(type, data, startTime);
  return currentSpanId;
}
void AppContext::closeCurrentSpan(const QJsonObject& extraData,
                                  const QDateTime& endTime) {
  db->closeSpan(currentSpanId, extraData, endTime);
  currentSpanId = -1;
}

void AppContext::onSleepEnd(int sleptSeconds) {
  QDateTime now = QDateTime::currentDateTimeUtc();
  QDateTime sleepStart = now.addSecs(-sleptSeconds);

  // Close the current state's span at sleep boundary
  closeCurrentSpan({}, sleepStart);

  // Create a sleep span from sleepStart to now
  int sleepId = db->openSpan("sleep", {}, sleepStart);
  db->closeSpan(sleepId);

  if (m_currentState->onSleepEnd(this, sleptSeconds)) return;
  // Default: treat sleep as a pause/resume cycle
  onPauseRequest(PauseReason::Sleep);
  data->pause().addSecondsPaused(sleptSeconds);
  onResumeRequest(PauseReason::Sleep);
}
void AppContext::onPauseRequest(PauseReasons reasons) {
  data->pause().addReasons(reasons);
  m_currentState->onPauseRequest(this, reasons);
}
void AppContext::onResumeRequest(PauseReasons reasons) {
  data->pause().removeReasons(reasons);
  m_currentState->onResumeRequest(this, reasons);
}

void AppContext::checkBreakReadiness() {
  if (!m_currentState || m_currentState->getID() != AppState::Normal) return;

  int secondsToNextBreak = data->schedule().secondsToNextBreak();
  int headsUpFor = preferences->headsUpFor->get();
  if (headsUpFor > 0 && secondsToNextBreak > 0 && secondsToNextBreak <= headsUpFor) {
    breakWindows->showHeadsUp(headsUpFor, data->breakType(), preferences);
    breakWindows->setHeadsUpTime(secondsToNextBreak);
  } else {
    breakWindows->hideHeadsUp();
  }

  if (secondsToNextBreak <= 0) transitionTo(std::make_unique<AppStateBreak>());
}

void AppStateNormal::enter(AppContext* app) {
  app->openCurrentSpan("normal");
  // Use low accuracy (5s) for idle detection in normal state as it can last a long time
  app->idleTimer->setWatchAccuracy(5000);
  app->idleTimer->setMinIdleTime(app->preferences->pauseOnIdleFor->get() * 1000);
}
void AppStateNormal::exit(AppContext* app) {
  app->closeCurrentSpan();
  app->breakWindows->hideHeadsUp();
}
void AppStateNormal::tick(AppContext* app) {
  app->data->schedule().tickSecondsToNextBreak();
  app->checkBreakReadiness();
}
void AppStateNormal::onIdleStart(AppContext* app) {
  // When in postpone mode, disable pausing
  if (app->data->schedule().isPostponing()) return;
  app->data->pause().addReasons(PauseReason::Idle);
  app->transitionTo(std::make_unique<AppStatePaused>());
}
void AppStateNormal::onPauseRequest(AppContext* app, PauseReasons) {
  // When in postpone mode, disable pausing
  if (app->data->schedule().isPostponing()) return;
  app->transitionTo(std::make_unique<AppStatePaused>());
}
void AppStateNormal::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::BreakNow>(&action)) {
    app->data->schedule().earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  } else if (std::get_if<Action::BigBreakNow>(&action)) {
    app->data->schedule().earlyBreak();
    if (app->data->currentBreakConfig().bigEnabled) app->data->makeNextBreakBig();
    app->transitionTo(std::make_unique<AppStateBreak>());
  } else if (std::get_if<Action::EndFocus>(&action)) {
    app->data->focus().setCyclesRemaining(1);
    app->data->schedule().earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  }
}

// The paused state will inherit the idle time settings from the state before:
// - For normal state to paused: low accuracy because the pause may be long
// - For break state to paused: high accuracy because we want it to be instant
// Moreover, setting idle interval will reset the event-based watchers.
void AppStatePaused::enter(AppContext* app) {
  m_currentSpanReasons = app->data->pause().reasons();
  m_currentSpanType = effectivePauseSpanType(m_currentSpanReasons);
  app->openCurrentSpan(m_currentSpanType, pausedSpanData(m_currentSpanReasons));
}

/* To avoid immediate breaks after break resume, we consider the user have already
 * taken the break if there shall be breaks during the pause. Of course, if the user
 * have configured `resetAfterPause`, we just reset it.
 */
void AppStatePaused::exit(AppContext* app) {
  app->closeCurrentSpan(pausedSpanData(m_currentSpanReasons));
  BreakConfig config = app->data->currentBreakConfig();
  // Calculate the total duration of the next break to determine if breaks would have
  // occurred during pause
  int nextBreakDuration =
      (app->data->breakType() == BreakType::Big ? config.bigFor : config.smallFor);
  int secondsToNextBreakEnd =
      app->data->schedule().secondsToNextBreak() + nextBreakDuration;
  // If user was paused longer than the break duration or longer than resetAfterPause
  // setting, refill the break timer to avoid immediate breaks
  if (app->data->pause().secondsPaused() > secondsToNextBreakEnd ||
      app->data->pause().secondsPaused() > app->preferences->resetAfterPause->get()) {
    app->data->schedule().refillSecondsToNextBreak(config);
  }
  // If user was paused longer than resetCycleAfterPause setting, reset the entire break
  // cycle
  if (app->data->pause().secondsPaused() >
      app->preferences->resetCycleAfterPause->get()) {
    app->data->resetBreakCycle();
  }
  app->data->pause().resetSecondsPaused();
  // Ensure pause reasons are cleared on exit
  app->data->pause().clearReasons();
  m_currentSpanType.clear();
  m_currentSpanReasons = {};
}
void AppStatePaused::tick(AppContext* app) { app->data->pause().tickSecondsPaused(); }
void AppStatePaused::onIdleStart(AppContext* app) {
  app->data->pause().addReasons(PauseReason::Idle);
  m_currentSpanReasons |= app->data->pause().reasons();
  QString nextSpanType = effectivePauseSpanType(app->data->pause().reasons());
  if (nextSpanType != m_currentSpanType) {
    app->closeCurrentSpan(pausedSpanData(m_currentSpanReasons));
    m_currentSpanType = nextSpanType;
    m_currentSpanReasons = app->data->pause().reasons();
    app->openCurrentSpan(m_currentSpanType, pausedSpanData(m_currentSpanReasons));
  }
}
void AppStatePaused::onIdleEnd(AppContext* app) {
  // We need to clear screenLockTimer when going through break -> paused -> idleEnd
  app->screenLockTimer->stop();
  app->data->pause().removeReasons(PauseReason::Idle);
  if (app->data->pause().reasons()) {
    QString nextSpanType = effectivePauseSpanType(app->data->pause().reasons());
    if (nextSpanType != m_currentSpanType) {
      app->closeCurrentSpan(pausedSpanData(m_currentSpanReasons));
      m_currentSpanType = nextSpanType;
      m_currentSpanReasons = app->data->pause().reasons();
      app->openCurrentSpan(m_currentSpanType, pausedSpanData(m_currentSpanReasons));
    } else {
      m_currentSpanReasons |= app->data->pause().reasons();
    }
  }
  if (!app->data->pause().reasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}
void AppStatePaused::onPauseRequest(AppContext* app, PauseReasons) {
  m_currentSpanReasons |= app->data->pause().reasons();
  QString nextSpanType = effectivePauseSpanType(app->data->pause().reasons());
  if (nextSpanType == m_currentSpanType) return;

  app->closeCurrentSpan(pausedSpanData(m_currentSpanReasons));
  m_currentSpanType = nextSpanType;
  m_currentSpanReasons = app->data->pause().reasons();
  app->openCurrentSpan(m_currentSpanType, pausedSpanData(m_currentSpanReasons));
}
void AppStatePaused::onResumeRequest(AppContext* app, PauseReasons) {
  if (app->data->pause().reasons()) {
    QString nextSpanType = effectivePauseSpanType(app->data->pause().reasons());
    if (nextSpanType != m_currentSpanType) {
      app->closeCurrentSpan(pausedSpanData(m_currentSpanReasons));
      m_currentSpanType = nextSpanType;
      m_currentSpanReasons = app->data->pause().reasons();
      app->openCurrentSpan(m_currentSpanType, pausedSpanData(m_currentSpanReasons));
    } else {
      m_currentSpanReasons |= app->data->pause().reasons();
    }
  }
  if (!app->data->pause().reasons()) {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}
void AppStatePaused::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::EnableBreaks>(&action)) {
    app->data->pause().clearReasons();
    app->transitionTo(std::make_unique<AppStateNormal>());
  } else if (std::get_if<Action::EndFocus>(&action)) {
    app->data->pause().clearReasons();
    app->data->focus().setCyclesRemaining(1);
    app->data->schedule().earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  }
}

void AppStateBreak::enter(AppContext* app) {
  app->openCurrentSpan(
      "break", {{"type", app->data->breakType() == BreakType::Big ? "big" : "small"}});
  data = std::make_unique<BreaksData>(dataInit(app));
  // On focus entry break, exhaust force break exits so the exit button is hidden
  if (app->data->focus().isActive() && !app->data->focus().entryBreakDone()) {
    for (int i = 0; i < app->preferences->maxForceBreakExits->get(); i++)
      data->recordForceBreakExit();
  }
  // Use high accuracy (500ms) for idle detection during breaks for responsive
  // transitions
  app->idleTimer->setWatchAccuracy(500);
  app->idleTimer->setMinIdleTime(2000);
  app->breakWindows->create(app->data->breakType(), app->preferences,
                            data->totalSeconds(),
                            app->data->schedule().isBreakExtendedByPostpone());
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
  app->closeCurrentSpan({{"normal-exit", (data->remainingSeconds() <= 0)}});
  if (!m_preserveBreakWindowsOnExit) app->breakWindows->destroy();
  m_preserveBreakWindowsOnExit = false;
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
    this->completeBreak(app);
    app->transitionTo(std::make_unique<AppStatePaused>());
  }
}
BreaksDataInit AppStateBreak::dataInit(AppContext* app) {
  int flashFor = app->preferences->flashFor->get();
  // On focus entry break (first break when entering focus mode), force immediate
  // full-screen by setting flashFor to 0
  if (app->data->focus().isActive() && !app->data->focus().entryBreakDone()) {
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
  } else if (std::get_if<Action::ReenterBreak>(&action)) {
    this->exit(app);
    this->enter(app);
  }
}

void BreakPhasePrompt::enter(AppContext* app, AppStateBreak*) {
  m_spanId = app->db->openSpan("flash");
  app->breakWindows->showFlashPrompt();
  app->breakWindows->showButtons(AbstractBreakWindows::Button::ExitForceBreak |
                                     AbstractBreakWindows::Button::LockScreen,
                                 false);
  // screenLockTimer should only be active in BreakPhaseFullScreen and AppStatePaused
  app->screenLockTimer->stop();
}
void BreakPhasePrompt::exit(AppContext* app, AppStateBreak*) {
  app->db->closeSpan(m_spanId);
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
    // record break type before break completion mutates the cycle
    auto breakType = app->data->breakType();
    BreakCompletion completion = breakState->completeBreak(app);
    if (app->idleTimer->isIdle()) {
      // Check if window should auto-close based on preferences and break type
      bool keepWindowOpen =
          (breakType == BreakType::Small &&
           !app->preferences->autoCloseWindowAfterSmallBreak->get()) ||
          (breakType == BreakType::Big &&
           !app->preferences->autoCloseWindowAfterBigBreak->get());
      app->data->postBreak().setPending(completion);
      breakState->preserveBreakWindowsOnExit(keepWindowOpen);
      app->transitionTo(std::make_unique<AppStatePostBreakIdle>(keepWindowOpen));
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

void AppStatePostBreakIdle::enter(AppContext* app) {
  app->openCurrentSpan("away",
                       {{"reasons", QJsonArray{pauseReasonName(PauseReason::Idle)}}});
  app->screenLockTimer->stop();
  app->data->pause().addReasons(PauseReason::Idle);
  if (m_keepWindowOpen) {
    app->breakWindows->showButtons(AbstractBreakWindows::Button::ExitForceBreak |
                                       AbstractBreakWindows::Button::LockScreen,
                                   false);
  }
}

void AppStatePostBreakIdle::exit(AppContext* app) {
  app->closeCurrentSpan();
  app->data->pause().removeReasons(PauseReason::Idle);
  app->data->postBreak().clear();
  if (m_keepWindowOpen) app->breakWindows->destroy();
}

void AppStatePostBreakIdle::tick(AppContext* app) {
  app->data->postBreak().tickIdleSeconds();
}

void AppStatePostBreakIdle::finalize(AppContext* app) {
  int idleSeconds = app->data->postBreak().idleSeconds();
  int cycleResetThreshold = app->data->postBreak().cycleResetThresholdSeconds();
  int undoPostponeShrinkThreshold = app->preferences->resetCycleAfterPause->get();
  bool resetCycle = app->data->postBreak().breakType() == BreakType::Small &&
                    idleSeconds > cycleResetThreshold;
  bool undoPostponeShrink = app->data->postBreak().wasPostponed() &&
                            idleSeconds > undoPostponeShrinkThreshold;

  app->data->finalizePendingPostBreak(resetCycle, undoPostponeShrink);

  PauseReasons remainingPauseReasons = app->data->pause().reasons();
  remainingPauseReasons &= ~PauseReasons(PauseReason::Idle);
  if (remainingPauseReasons) {
    app->transitionTo(std::make_unique<AppStatePaused>());
  } else {
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}

void AppStatePostBreakIdle::onIdleEnd(AppContext* app) {
  app->screenLockTimer->stop();
  finalize(app);
}

bool AppStatePostBreakIdle::onSleepEnd(AppContext* app, int sleptSeconds) {
  // Sleep contributes to post-break inactivity, but this state should remain in the
  // same deferred-finalization mode after wake.
  app->openCurrentSpan("away",
                       {{"reasons", QJsonArray{pauseReasonName(PauseReason::Idle)}}});
  app->data->postBreak().addIdleSeconds(sleptSeconds);
  return true;
}

void AppStateMeeting::enter(AppContext* app) {
  app->openCurrentSpan("meeting",
                       {{"scheduledSeconds", app->data->meeting().totalSeconds()},
                        {"reason", app->data->meeting().reason()}});
  app->data->schedule().resetSecondsToNextBreak(app->data->currentBreakConfig());
  app->idleTimer->setWatchAccuracy(5000);
  app->idleTimer->setMinIdleTime(app->preferences->pauseOnIdleFor->get() * 1000);
}

void AppStateMeeting::exit(AppContext* app) {
  app->closeCurrentSpan();
  app->data->meeting().clear();
  app->meetingPrompt->closeEndPrompt();
}

void AppStateMeeting::tick(AppContext* app) {
  if (app->data->meeting().secondsRemaining() > 0) app->data->meeting().tickRemaining();
  int remaining = app->data->meeting().secondsRemaining();
  if (remaining > 0 && remaining <= 60) {
    app->meetingPrompt->showEndPrompt();
    app->meetingPrompt->setTime(remaining);
  } else if (remaining <= 0) {
    app->db->logEvent("meeting::end", {{"next-break", 0}});
    if (app->data->currentBreakConfig().bigEnabled) app->data->makeNextBreakBig();
    app->transitionTo(std::make_unique<AppStateNormal>());
  }
}

void AppStateMeeting::onMenuAction(AppContext* app, MenuAction action) {
  if (std::get_if<Action::EndMeetingBreakNow>(&action)) {
    app->db->logEvent("meeting::end", {{"next-break", 0}});
    if (app->data->currentBreakConfig().bigEnabled) app->data->makeNextBreakBig();
    app->data->schedule().earlyBreak();
    app->transitionTo(std::make_unique<AppStateBreak>());
  } else if (auto* a = std::get_if<Action::EndMeetingBreakLater>(&action)) {
    app->db->logEvent("meeting::end", {{"next-break", a->seconds}});
    if (app->data->currentBreakConfig().bigEnabled) app->data->makeNextBreakBig();
    app->data->schedule().setSecondsToNextBreak(a->seconds);
    app->transitionTo(std::make_unique<AppStateNormal>());
  } else if (auto* a = std::get_if<Action::ExtendMeeting>(&action)) {
    app->db->logEvent("meeting::extend", {{"seconds", a->seconds}});
    app->data->meeting().extend(a->seconds);
    app->meetingPrompt->closeEndPrompt();
  }
}
bool AppStateMeeting::onSleepEnd(AppContext* app, int sleptSeconds) {
  BreakConfig config = app->data->currentBreakConfig();
  int breakDuration = config.bigEnabled ? config.bigFor : config.smallFor;
  int skipIfSleptFor = app->data->meeting().secondsRemaining() + breakDuration;
  if (sleptSeconds >= skipIfSleptFor) {
    app->db->logEvent("meeting::end", {{"next-break", -1}});
    app->data->resetBreakCycle();
    app->data->schedule().resetSecondsToNextBreak(app->data->currentBreakConfig());
    app->transitionTo(std::make_unique<AppStateNormal>());
    return true;
  }
  // Short sleep: reopen meeting span at current time
  app->openCurrentSpan("meeting",
                       {{"scheduledSeconds", app->data->meeting().totalSeconds()},
                        {"reason", app->data->meeting().reason()}});
  app->data->meeting().subtractRemaining(sleptSeconds);
  return true;
}
