// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-data.h"

#include <QDateTime>
#include <QObject>
#include <algorithm>

#include "core/flags.h"
#include "core/preferences.h"

AppData::AppData(QObject* parent, SanePreferences* preferences)
    : QObject(parent), preferences(preferences) {
  m_secondsToNextBreak = preferences->smallEvery->get();
}

BreakType AppData::breakType() {
  if (!effectiveBigBreakEnabled()) return BreakType::Small;
  return smallBreaksBeforeBigBreak() == 0 ? BreakType::Big : BreakType::Small;
};

int AppData::smallBreaksBeforeBigBreak() {
  if (!effectiveBigBreakEnabled()) return -1;
  int breakEvery = effectiveBigAfter();
  m_breakCycleCount %= breakEvery;
  return (breakEvery - m_breakCycleCount) % breakEvery;
};
void AppData::finishAndStartNextCycle() {
  m_breakCycleCount++;
  if (m_focusData.isActive) {
    m_focusData.cyclesRemaining--;
    if (m_focusData.cyclesRemaining <= 0) m_focusData.clear();
  }
  // Must come after focus clear: if focus just ended, effectiveSmallEvery()
  // returns the normal interval instead of the focus interval.
  m_secondsToNextBreak = effectiveSmallEvery();
  m_secondsToNextBreak -= m_postponeData.secondsPostponed() *
                          preferences->postponeShrinkNextPercent->get() / 100;
  m_postponeData.reset();
  emit changed();
};
int AppData::breakDuration() {
  int totalSeconds =
      breakType() == BreakType::Big ? effectiveBigFor() : effectiveSmallFor();
  // If postponed, calculate extra seconds. Add zero otherwise.
  int breakForReference =
      effectiveBigBreakEnabled() ? effectiveBigFor() : effectiveSmallFor();
  totalSeconds += preferences->postponeExtendBreakPercent->get() *
                  m_postponeData.secondsPostponed() * breakForReference /
                  effectiveSmallEvery() / 100;
  return totalSeconds;
}
void AppData::resetBreakCycle() {
  m_breakCycleCount = 1;
  emit changed();
};
void AppData::makeNextBreakBig() {
  m_breakCycleCount = 0;
  emit changed();
};
void AppData::makeNextBreakLastSmallBeforeBig() {
  m_breakCycleCount = -1;
  emit changed();
};

int AppData::secondsToNextBreak() { return m_secondsToNextBreak; };
void AppData::tickSecondsToNextBreak() {
  m_secondsToNextBreak--;
  emit changed();
};
void AppData::resetSecondsToNextBreak() {
  m_secondsToNextBreak = effectiveSmallEvery();
  emit changed();
};
void AppData::setSecondsToNextBreak(int secs) {
  m_secondsToNextBreak = secs;
  emit changed();
};
void AppData::refillSecondsToNextBreak() {
  if (m_secondsToNextBreak < effectiveSmallEvery()) {
    m_secondsToNextBreak = effectiveSmallEvery();
    emit changed();
  }
};
void AppData::postpone(int secs) {
  m_secondsToNextBreak += secs;
  m_postponeData.plannedSecondsToPostpone = secs;
  emit changed();
};
bool AppData::isPostponing() { return m_postponeData.isPostponing(); }
void AppData::earlyBreak() {
  if (m_postponeData.isPostponing())
    m_postponeData.actualSecondsToNextBreakWhenBreak = m_secondsToNextBreak;
  m_secondsToNextBreak = 0;
  emit changed();
};

int AppData::secondsPaused() { return m_secondsPaused; };
void AppData::tickSecondsPaused() { addSecondsPaused(1); };
void AppData::addSecondsPaused(int secs) {
  m_secondsPaused += secs;
  emit changed();
};
void AppData::resetSecondsPaused() {
  m_secondsPaused = 0;
  emit changed();
};

PauseReasons AppData::pauseReasons() { return m_pauseReasons; };
void AppData::addPauseReasons(PauseReasons reason) {
  m_pauseReasons |= reason;
  emit changed();
};
void AppData::removePauseReasons(PauseReasons reason) {
  m_pauseReasons &= ~reason;
  emit changed();
};
void AppData::clearPauseReasons() {
  m_pauseReasons = PauseReasons::fromInt(0);
  emit changed();
}
void MeetingData::clear() {
  isActive = false;
  secondsRemaining = 0;
  totalSeconds = 0;
  reason.clear();
}
void FocusData::clear() {
  isActive = false;
  cyclesRemaining = 0;
  totalCycles = 0;
  spanId = -1;
}
bool AppData::isInMeeting() const { return m_meetingData.isActive; }
int AppData::meetingSecondsRemaining() const { return m_meetingData.secondsRemaining; }
int AppData::meetingTotalSeconds() const { return m_meetingData.totalSeconds; }
QString AppData::meetingReason() const { return m_meetingData.reason; }
void AppData::setMeetingData(int secondsRemaining, int totalSeconds,
                             const QString& reason) {
  m_meetingData.isActive = true;
  m_meetingData.secondsRemaining = secondsRemaining;
  m_meetingData.totalSeconds = totalSeconds;
  m_meetingData.reason = reason;
  emit changed();
}
void AppData::clearMeetingData() {
  m_meetingData.clear();
  emit changed();
}
void AppData::tickMeetingRemaining() {
  m_meetingData.secondsRemaining--;
  emit changed();
}
void AppData::subtractMeetingRemaining(int secs) {
  m_meetingData.secondsRemaining = std::max(0, m_meetingData.secondsRemaining - secs);
  emit changed();
}
void AppData::extendMeeting(int secs) {
  m_meetingData.secondsRemaining += secs;
  m_meetingData.totalSeconds += secs;
  emit changed();
}

bool AppData::isFocusMode() const { return m_focusData.isActive; }
int AppData::focusCyclesRemaining() const { return m_focusData.cyclesRemaining; }
int AppData::focusTotalCycles() const { return m_focusData.totalCycles; }
void AppData::startFocusMode(int totalCycles) {
  m_focusData.isActive = true;
  m_focusData.cyclesRemaining = totalCycles;
  m_focusData.totalCycles = totalCycles;
  emit changed();
}
void AppData::endFocusMode() {
  m_focusData.clear();
  emit changed();
}
void AppData::setFocusCyclesRemaining(int cycles) {
  m_focusData.cyclesRemaining = cycles;
  emit changed();
}
int AppData::focusSpanId() const { return m_focusData.spanId; }
void AppData::setFocusSpanId(int id) { m_focusData.spanId = id; }

int AppData::effectiveSmallEvery() {
  return m_focusData.isActive ? preferences->focusSmallEvery->get()
                              : preferences->smallEvery->get();
}
int AppData::effectiveSmallFor() {
  return m_focusData.isActive ? preferences->focusSmallFor->get()
                              : preferences->smallFor->get();
}
bool AppData::effectiveBigBreakEnabled() {
  return m_focusData.isActive ? preferences->focusBigBreakEnabled->get()
                              : preferences->bigBreakEnabled->get();
}
int AppData::effectiveBigAfter() {
  return m_focusData.isActive ? preferences->focusBigAfter->get()
                              : preferences->bigAfter->get();
}
int AppData::effectiveBigFor() {
  return m_focusData.isActive ? preferences->focusBigFor->get()
                              : preferences->bigFor->get();
}

bool PostponeData::isPostponing() {
  return plannedSecondsToPostpone - actualSecondsToNextBreakWhenBreak;
}
void PostponeData::reset() {
  plannedSecondsToPostpone = actualSecondsToNextBreakWhenBreak = 0;
}
int PostponeData::secondsPostponed() {
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
