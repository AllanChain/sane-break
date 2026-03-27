// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-data.h"

#include <algorithm>

#include "core/flags.h"
#include "core/preferences.h"

AppData::AppData(QObject* parent, SanePreferences* preferences)
    : QObject(parent), preferences(preferences) {
  auto notifyChanged = [this]() { emit changed(); };
  m_schedule.setOnChanged(notifyChanged);
  m_pause.setOnChanged(notifyChanged);
  m_meeting.setOnChanged(notifyChanged);
  m_focus.setOnChanged(notifyChanged);
  m_schedule.init(preferences->smallEvery->get());

  connect(preferences->bigBreakEnabled, &SettingWithSignal::changed, this,
          &AppData::changed);
  connect(preferences->bigAfter, &SettingWithSignal::changed, this, &AppData::changed);
  connect(preferences->focusSmallEvery, &SettingWithSignal::changed, this,
          &AppData::changed);
  connect(preferences->focusBigBreakEnabled, &SettingWithSignal::changed, this,
          &AppData::changed);
  connect(preferences->focusBigAfter, &SettingWithSignal::changed, this,
          &AppData::changed);
}

void BreakScheduleState::setOnChanged(std::function<void()> onChanged) {
  m_onChanged = std::move(onChanged);
}
void BreakScheduleState::init(int secondsToNextBreak) {
  m_secondsToNextBreak = secondsToNextBreak;
  m_completedSmallBreaks = 0;
  m_postponeData.reset();
}
int BreakScheduleState::secondsToNextBreak() const { return m_secondsToNextBreak; }
void BreakScheduleState::tickSecondsToNextBreak() {
  m_secondsToNextBreak--;
  notifyChanged();
}
void BreakScheduleState::resetSecondsToNextBreak(const BreakConfig& config,
                                                 bool notify) {
  m_secondsToNextBreak = config.smallEvery;
  if (notify) notifyChanged();
}
void BreakScheduleState::setSecondsToNextBreak(int secs, bool notify) {
  m_secondsToNextBreak = secs;
  if (notify) notifyChanged();
}
void BreakScheduleState::refillSecondsToNextBreak(const BreakConfig& config) {
  if (m_secondsToNextBreak < config.smallEvery) {
    m_secondsToNextBreak = config.smallEvery;
    notifyChanged();
  }
}
void BreakScheduleState::postpone(int secs) {
  m_secondsToNextBreak += secs;
  m_postponeData.plannedSecondsToPostpone = secs;
  notifyChanged();
}
bool BreakScheduleState::isPostponing() const { return m_postponeData.isPostponing(); }
int BreakScheduleState::secondsPostponed() const {
  return m_postponeData.secondsPostponed();
}
void BreakScheduleState::resetPostpone(bool notify) {
  m_postponeData.reset();
  if (notify) notifyChanged();
}
void BreakScheduleState::earlyBreak() {
  if (m_postponeData.isPostponing()) {
    m_postponeData.actualSecondsToNextBreakWhenBreak = m_secondsToNextBreak;
  }
  m_secondsToNextBreak = 0;
  notifyChanged();
}
bool BreakScheduleState::isBreakExtendedByPostpone() const {
  return m_postponeData.secondsPostponed() > 0;
}
int BreakScheduleState::smallBreaksBeforeBigBreak(const BreakConfig& config) const {
  if (!config.bigEnabled) return -1;
  return std::max(0, config.bigAfter - 1 - m_completedSmallBreaks);
}
BreakType BreakScheduleState::breakType(const BreakConfig& config) const {
  if (!config.bigEnabled) return BreakType::Small;
  return smallBreaksBeforeBigBreak(config) == 0 ? BreakType::Big : BreakType::Small;
}
int BreakScheduleState::breakDuration(const BreakConfig& config,
                                      int postponeExtendBreakPercent) const {
  int totalSeconds =
      breakType(config) == BreakType::Big ? config.bigFor : config.smallFor;
  int breakForReference = config.bigEnabled ? config.bigFor : config.smallFor;
  totalSeconds += postponeExtendBreakPercent * m_postponeData.secondsPostponed() *
                  breakForReference / config.smallEvery / 100;
  return totalSeconds;
}
void BreakScheduleState::resetCycle(bool notify) {
  m_completedSmallBreaks = 0;
  if (notify) notifyChanged();
}
void BreakScheduleState::makeNextBreakBig(const BreakConfig& config, bool notify) {
  m_completedSmallBreaks = std::max(0, config.bigAfter - 1);
  if (notify) notifyChanged();
}
void BreakScheduleState::makeNextBreakLastSmallBeforeBig(const BreakConfig& config,
                                                         bool notify) {
  m_completedSmallBreaks = std::max(0, config.bigAfter - 2);
  if (notify) notifyChanged();
}
void BreakScheduleState::recordCompletedBreak(BreakType completedBreakType,
                                              bool notify) {
  if (completedBreakType == BreakType::Big) {
    m_completedSmallBreaks = 0;
  } else {
    m_completedSmallBreaks++;
  }
  if (notify) notifyChanged();
}
const PostponeData& BreakScheduleState::postponeData() const { return m_postponeData; }
void BreakScheduleState::notifyChanged() {
  if (m_onChanged) m_onChanged();
}

void PauseState::setOnChanged(std::function<void()> onChanged) {
  m_onChanged = std::move(onChanged);
}
int PauseState::secondsPaused() const { return m_secondsPaused; }
void PauseState::tickSecondsPaused() { addSecondsPaused(1); }
void PauseState::addSecondsPaused(int secs) {
  m_secondsPaused += secs;
  notifyChanged();
}
void PauseState::resetSecondsPaused() {
  m_secondsPaused = 0;
  notifyChanged();
}
PauseReasons PauseState::reasons() const { return m_reasons; }
void PauseState::addReasons(PauseReasons reasons) {
  m_reasons |= reasons;
  notifyChanged();
}
void PauseState::removeReasons(PauseReasons reasons) {
  m_reasons &= ~reasons;
  notifyChanged();
}
void PauseState::clearReasons() {
  m_reasons = PauseReasons::fromInt(0);
  notifyChanged();
}
void PauseState::notifyChanged() {
  if (m_onChanged) m_onChanged();
}

void MeetingState::setOnChanged(std::function<void()> onChanged) {
  m_onChanged = std::move(onChanged);
}
bool MeetingState::isActive() const { return m_isActive; }
int MeetingState::secondsRemaining() const { return m_secondsRemaining; }
int MeetingState::totalSeconds() const { return m_totalSeconds; }
QString MeetingState::reason() const { return m_reason; }
void MeetingState::set(int secondsRemaining, int totalSeconds, const QString& reason) {
  m_isActive = true;
  m_secondsRemaining = secondsRemaining;
  m_totalSeconds = totalSeconds;
  m_reason = reason;
  notifyChanged();
}
void MeetingState::clear() {
  m_isActive = false;
  m_secondsRemaining = 0;
  m_totalSeconds = 0;
  m_reason.clear();
  notifyChanged();
}
void MeetingState::tickRemaining() {
  m_secondsRemaining--;
  notifyChanged();
}
void MeetingState::subtractRemaining(int secs) {
  m_secondsRemaining = std::max(0, m_secondsRemaining - secs);
  notifyChanged();
}
void MeetingState::extend(int secs) {
  m_secondsRemaining += secs;
  m_totalSeconds += secs;
  notifyChanged();
}
void MeetingState::notifyChanged() {
  if (m_onChanged) m_onChanged();
}

void FocusState::setOnChanged(std::function<void()> onChanged) {
  m_onChanged = std::move(onChanged);
}
bool FocusState::isActive() const { return m_isActive; }
int FocusState::cyclesRemaining() const { return m_cyclesRemaining; }
int FocusState::totalCycles() const { return m_totalCycles; }
bool FocusState::entryBreakDone() const { return m_entryBreakDone; }
int FocusState::spanId() const { return m_spanId; }
void FocusState::start(int totalCycles) {
  m_isActive = true;
  m_entryBreakDone = false;
  m_cyclesRemaining = totalCycles;
  m_totalCycles = totalCycles;
  m_completedSmallBreaks = 0;
  notifyChanged();
}
void FocusState::end() {
  clear();
  notifyChanged();
}
void FocusState::setCyclesRemaining(int cycles) {
  m_cyclesRemaining = cycles;
  notifyChanged();
}
void FocusState::setSpanId(int id) { m_spanId = id; }
int FocusState::smallBreaksBeforeBigBreak(const BreakConfig& config) const {
  if (!config.bigEnabled) return -1;
  return std::max(0, config.bigAfter - 1 - m_completedSmallBreaks);
}
BreakType FocusState::breakType(const BreakConfig& config) const {
  if (!config.bigEnabled) return BreakType::Small;
  return smallBreaksBeforeBigBreak(config) == 0 ? BreakType::Big : BreakType::Small;
}
void FocusState::resetCycle(bool notify) {
  m_completedSmallBreaks = 0;
  if (notify) notifyChanged();
}
void FocusState::makeNextBreakBig(const BreakConfig& config, bool notify) {
  m_completedSmallBreaks = std::max(0, config.bigAfter - 1);
  if (notify) notifyChanged();
}
void FocusState::makeNextBreakLastSmallBeforeBig(const BreakConfig& config,
                                                 bool notify) {
  m_completedSmallBreaks = std::max(0, config.bigAfter - 2);
  if (notify) notifyChanged();
}
void FocusState::recordCompletedBreak(BreakType completedBreakType, bool notify) {
  if (completedBreakType == BreakType::Big) {
    m_completedSmallBreaks = 0;
  } else {
    m_completedSmallBreaks++;
  }
  if (notify) notifyChanged();
}
bool FocusState::advanceAfterCompletedBreak(bool notify) {
  if (!m_isActive) return false;

  bool changed = false;
  bool completed = false;
  if (m_entryBreakDone) {
    m_cyclesRemaining--;
    changed = true;
    if (m_cyclesRemaining <= 0) {
      clear();
      completed = true;
      changed = true;
    }
  } else {
    m_entryBreakDone = true;
    changed = true;
  }

  if (changed && notify) notifyChanged();
  return completed;
}
void FocusState::clear() {
  m_isActive = false;
  m_entryBreakDone = false;
  m_cyclesRemaining = 0;
  m_totalCycles = 0;
  m_spanId = -1;
  m_completedSmallBreaks = 0;
}
void FocusState::notifyChanged() {
  if (m_onChanged) m_onChanged();
}

void PostBreakState::setPending(const BreakCompletion& completion) {
  m_isActive = true;
  m_breakType = completion.completedBreakType;
  m_wasPostponed = completion.wasPostponed;
  m_cycleResetThresholdSeconds = completion.cycleResetThresholdSeconds;
  m_nextSessionBaseSeconds = completion.nextSessionBaseSeconds;
  m_nextSessionAdjustedSeconds = completion.nextSessionAdjustedSeconds;
  m_idleSeconds = 0;
}
bool PostBreakState::isActive() const { return m_isActive; }
BreakType PostBreakState::breakType() const { return m_breakType; }
bool PostBreakState::wasPostponed() const { return m_wasPostponed; }
int PostBreakState::cycleResetThresholdSeconds() const {
  return m_cycleResetThresholdSeconds;
}
int PostBreakState::nextSessionBaseSeconds() const { return m_nextSessionBaseSeconds; }
int PostBreakState::nextSessionAdjustedSeconds() const {
  return m_nextSessionAdjustedSeconds;
}
int PostBreakState::idleSeconds() const { return m_idleSeconds; }
void PostBreakState::tickIdleSeconds() { addIdleSeconds(1); }
void PostBreakState::addIdleSeconds(int secs) {
  if (!m_isActive) return;
  m_idleSeconds += secs;
}
void PostBreakState::clear() {
  m_isActive = false;
  m_breakType = BreakType::Small;
  m_wasPostponed = false;
  m_cycleResetThresholdSeconds = 0;
  m_nextSessionBaseSeconds = 0;
  m_nextSessionAdjustedSeconds = 0;
  m_idleSeconds = 0;
}

BreakScheduleState& AppData::schedule() { return m_schedule; }
const BreakScheduleState& AppData::schedule() const { return m_schedule; }
PauseState& AppData::pause() { return m_pause; }
const PauseState& AppData::pause() const { return m_pause; }
MeetingState& AppData::meeting() { return m_meeting; }
const MeetingState& AppData::meeting() const { return m_meeting; }
FocusState& AppData::focus() { return m_focus; }
const FocusState& AppData::focus() const { return m_focus; }
PostBreakState& AppData::postBreak() { return m_postBreak; }
const PostBreakState& AppData::postBreak() const { return m_postBreak; }

BreakConfig AppData::currentBreakConfig() const {
  if (m_focus.isActive()) {
    return {
        .smallEvery = preferences->focusSmallEvery->get(),
        .smallFor = preferences->focusSmallFor->get(),
        .bigEnabled = preferences->focusBigBreakEnabled->get(),
        .bigAfter = preferences->focusBigAfter->get(),
        .bigFor = preferences->focusBigFor->get(),
    };
  }
  return {
      .smallEvery = preferences->smallEvery->get(),
      .smallFor = preferences->smallFor->get(),
      .bigEnabled = preferences->bigBreakEnabled->get(),
      .bigAfter = preferences->bigAfter->get(),
      .bigFor = preferences->bigFor->get(),
  };
}
BreakType AppData::breakType() const {
  BreakConfig config = currentBreakConfig();
  if (m_focus.isActive()) return m_focus.breakType(config);
  return m_schedule.breakType(config);
}
int AppData::smallBreaksBeforeBigBreak() const {
  BreakConfig config = currentBreakConfig();
  if (m_focus.isActive()) return m_focus.smallBreaksBeforeBigBreak(config);
  return m_schedule.smallBreaksBeforeBigBreak(config);
}
BreakCompletion AppData::completeBreak() {
  m_postBreak.clear();
  BreakConfig config = currentBreakConfig();
  int cycleResetThresholdSeconds = std::max(0, config.bigFor - config.smallFor);
  BreakCompletion completion = {
      .completedBreakType = breakType(),
      .wasPostponed = m_schedule.secondsPostponed() > 0,
      .cycleResetThresholdSeconds = cycleResetThresholdSeconds,
  };

  if (m_focus.isActive()) {
    m_focus.recordCompletedBreak(completion.completedBreakType, false);
  } else {
    m_schedule.recordCompletedBreak(completion.completedBreakType, false);
  }
  if (m_focus.isActive()) {
    completion.focusCompleted = m_focus.advanceAfterCompletedBreak(false);
  }
  // Must come after focus clear: if focus just ended, effectiveSmallEvery()
  // returns the normal interval instead of the focus interval.
  completion.nextSessionBaseSeconds = currentBreakConfig().smallEvery;
  completion.nextSessionAdjustedSeconds =
      completion.nextSessionBaseSeconds -
      m_schedule.secondsPostponed() * preferences->postponeShrinkNextPercent->get() /
          100;
  m_schedule.setSecondsToNextBreak(completion.nextSessionAdjustedSeconds, false);
  m_schedule.resetPostpone(false);
  emit changed();
  return completion;
}
int AppData::breakDuration() const {
  return m_schedule.breakDuration(currentBreakConfig(),
                                  preferences->postponeExtendBreakPercent->get());
}
void AppData::resetBreakCycle() {
  if (m_focus.isActive()) {
    m_focus.resetCycle();
  } else {
    m_schedule.resetCycle();
  }
}
void AppData::makeNextBreakBig() {
  BreakConfig config = currentBreakConfig();
  if (m_focus.isActive()) {
    m_focus.makeNextBreakBig(config);
  } else {
    m_schedule.makeNextBreakBig(config);
  }
}
void AppData::makeNextBreakLastSmallBeforeBig() {
  BreakConfig config = currentBreakConfig();
  if (m_focus.isActive()) {
    m_focus.makeNextBreakLastSmallBeforeBig(config);
  } else {
    m_schedule.makeNextBreakLastSmallBeforeBig(config);
  }
}
void AppData::finalizePendingPostBreak(bool resetCycle, bool undoPostponeShrink) {
  if (!m_postBreak.isActive()) return;

  bool changedState = false;

  if (resetCycle) {
    if (m_focus.isActive()) {
      m_focus.resetCycle(false);
      changedState = true;
    } else {
      int previousSmallBreaks =
          m_schedule.smallBreaksBeforeBigBreak(currentBreakConfig());
      m_schedule.resetCycle(false);
      changedState = changedState ||
                     previousSmallBreaks !=
                         m_schedule.smallBreaksBeforeBigBreak(currentBreakConfig());
    }
  }

  int nextSeconds = undoPostponeShrink ? m_postBreak.nextSessionBaseSeconds()
                                       : m_postBreak.nextSessionAdjustedSeconds();
  if (m_schedule.secondsToNextBreak() != nextSeconds) {
    m_schedule.setSecondsToNextBreak(nextSeconds, false);
    changedState = true;
  }
  m_postBreak.clear();

  if (changedState) emit changed();
}

bool PostponeData::isPostponing() const { return secondsPostponed() > 0; }
void PostponeData::reset() {
  plannedSecondsToPostpone = actualSecondsToNextBreakWhenBreak = 0;
}
int PostponeData::secondsPostponed() const {
  return std::max(0, plannedSecondsToPostpone - actualSecondsToNextBreakWhenBreak);
}

BreaksData::BreaksData(BreaksDataInit data) { init(data); }
void BreaksData::init(BreaksDataInit data) {
  m_remainingSeconds = m_totalSeconds = data.totalSeconds;
  m_secondsToForceBreak = data.flashFor;
  m_numberForceBreakExits = 0;
}
int BreaksData::totalSeconds() { return m_totalSeconds; };

bool BreaksData::isForceBreak() { return m_secondsToForceBreak <= 0; };
void BreaksData::tickSecondsToForceBreak() { m_secondsToForceBreak--; };
void BreaksData::resetSecondsToForceBreak() { m_secondsToForceBreak = 0; };

int BreaksData::remainingSeconds() { return m_remainingSeconds; };
void BreaksData::tickRemainingTime() { m_remainingSeconds--; };
void BreaksData::resetRemainingTime() { m_remainingSeconds = m_totalSeconds; };

int BreaksData::numberForceBreakExits() { return m_numberForceBreakExits; };
void BreaksData::recordForceBreakExit() { m_numberForceBreakExits++; };
