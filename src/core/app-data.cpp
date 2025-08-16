// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app-data.h"

#include <QObject>
#include <memory>

#include "core/flags.h"
#include "core/preferences.h"

AppData::AppData(QObject *parent, SanePreferences *preferences)
    : QObject(parent), preferences(preferences) {
  m_secondsToNextBreak = preferences->smallEvery->get();
  breaks = std::make_unique<BreaksData>();
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
  m_secondsSinceLastBreak = 0;
  m_secondsToNextBreak = preferences->smallEvery->get();
  emit changed();
};
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
void AppData::addSecondsToNextBreak(int secs) {
  m_secondsToNextBreak += secs;
  emit changed();
};
void AppData::zeroSecondsToNextBreak() {
  m_secondsToNextBreak = 0;
  emit changed();
};
void AppData::resetSecondsToNextBreak() {
  m_secondsSinceLastBreak = 0;
  m_secondsToNextBreak = preferences->smallEvery->get();
  emit changed();
};
void AppData::refillSecondsToNextBreak() {
  m_secondsSinceLastBreak = 0;
  if (m_secondsToNextBreak < preferences->smallEvery->get()) {
    m_secondsToNextBreak = preferences->smallEvery->get();
    emit changed();
  }
};

int AppData::secondsSinceLastBreak() { return m_secondsSinceLastBreak; };
void AppData::tickSecondsSinceLastBreak() {
  m_secondsSinceLastBreak++;
  emit changed();
};
void AppData::resetSecondsSinceLastBreak() {
  m_secondsSinceLastBreak = 0;
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
