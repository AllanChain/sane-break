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
  return smallBreaksBeforeBigBreak() == 0 ? BreakType::Big : BreakType::Small;
};

int AppData::smallBreaksBeforeBigBreak() {
  int breakEvery = preferences->bigAfter->get();
  m_breakCycleCount %= breakEvery;
  return (breakEvery - m_breakCycleCount) % breakEvery;
};
void AppData::finishAndStartNextCycle() {
  m_breakCycleCount++;
  m_secondsToNextBreak = preferences->smallEvery->get();
  m_secondsToNextBreak -= m_postponeData.secondsPostponed() *
                          preferences->postponeShrinkNextPercent->get() / 100;
  m_postponeData.reset();
  emit changed();
};
int AppData::breakDuration() {
  int totalSeconds = breakType() == BreakType::Big ? preferences->bigFor->get()
                                                   : preferences->smallFor->get();
  // If postponed, calculate extra seconds. Add zero otherwise.
  totalSeconds += preferences->postponeExtendBreakPercent->get() *
                  m_postponeData.secondsPostponed() * preferences->bigFor->get() /
                  preferences->smallEvery->get() / 100;
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
  m_secondsToNextBreak = preferences->smallEvery->get();
  emit changed();
};
void AppData::setSecondsToNextBreak(int secs) {
  m_secondsToNextBreak = secs;
  emit changed();
};
void AppData::refillSecondsToNextBreak() {
  if (m_secondsToNextBreak < preferences->smallEvery->get()) {
    m_secondsToNextBreak = preferences->smallEvery->get();
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
}
bool AppData::isInMeeting() const { return m_meetingData.isActive; }
int AppData::meetingSecondsRemaining() const { return m_meetingData.secondsRemaining; }
int AppData::meetingTotalSeconds() const { return m_meetingData.totalSeconds; }
void AppData::setMeetingData(int secondsRemaining, int totalSeconds) {
  m_meetingData.isActive = true;
  m_meetingData.secondsRemaining = secondsRemaining;
  m_meetingData.totalSeconds = totalSeconds;
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
