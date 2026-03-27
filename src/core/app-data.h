// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <functional>

#include "core/flags.h"
#include "core/preferences.h"

struct BreakConfig {
  int smallEvery = 0;
  int smallFor = 0;
  bool bigEnabled = false;
  int bigAfter = 0;
  int bigFor = 0;
};

struct PostponeData {
  int plannedSecondsToPostpone = 0;
  // Seconds to next break when the break happens.
  // Typically 0 but may be larger than 0 if having the break before the planned time.
  int actualSecondsToNextBreakWhenBreak = 0;
  bool isPostponing() const;
  int secondsPostponed() const;
  void reset();
};

class BreakScheduleState {
 public:
  void setOnChanged(std::function<void()> onChanged);
  void init(int secondsToNextBreak);

  int secondsToNextBreak() const;
  void tickSecondsToNextBreak();
  void resetSecondsToNextBreak(const BreakConfig& config, bool notify = true);
  void setSecondsToNextBreak(int secs, bool notify = true);
  void refillSecondsToNextBreak(const BreakConfig& config);

  void postpone(int secs);
  bool isPostponing() const;
  int secondsPostponed() const;
  void resetPostpone(bool notify = true);
  void earlyBreak();
  bool isBreakExtendedByPostpone() const;

  int smallBreaksBeforeBigBreak(const BreakConfig& config) const;
  BreakType breakType(const BreakConfig& config) const;
  int breakDuration(const BreakConfig& config, int postponeExtendBreakPercent) const;

  void resetCycle(bool notify = true);
  void makeNextBreakBig(const BreakConfig& config, bool notify = true);
  void makeNextBreakLastSmallBeforeBig(const BreakConfig& config, bool notify = true);
  void recordCompletedBreak(BreakType completedBreakType, bool notify = true);

  const PostponeData& postponeData() const;

 protected:
  void notifyChanged();

  std::function<void()> m_onChanged;
  int m_completedSmallBreaks = 0;
  int m_secondsToNextBreak = 0;
  PostponeData m_postponeData;
};

class PauseState {
 public:
  void setOnChanged(std::function<void()> onChanged);

  int secondsPaused() const;
  void tickSecondsPaused();
  void addSecondsPaused(int secs);
  void resetSecondsPaused();

  PauseReasons reasons() const;
  void addReasons(PauseReasons reasons);
  void removeReasons(PauseReasons reasons);
  void clearReasons();

 protected:
  void notifyChanged();

  std::function<void()> m_onChanged;
  int m_secondsPaused = 0;
  PauseReasons m_reasons = {};
};

class MeetingState {
 public:
  void setOnChanged(std::function<void()> onChanged);

  bool isActive() const;
  int secondsRemaining() const;
  int totalSeconds() const;
  QString reason() const;

  void set(int secondsRemaining, int totalSeconds, const QString& reason);
  void clear();
  void tickRemaining();
  void subtractRemaining(int secs);
  void extend(int secs);

 protected:
  void notifyChanged();

  std::function<void()> m_onChanged;
  bool m_isActive = false;
  int m_secondsRemaining = 0;
  int m_totalSeconds = 0;
  QString m_reason;
};

class FocusState {
 public:
  void setOnChanged(std::function<void()> onChanged);

  bool isActive() const;
  int cyclesRemaining() const;
  int totalCycles() const;
  bool entryBreakDone() const;
  int spanId() const;

  void start(int totalCycles);
  void end();
  void setCyclesRemaining(int cycles);
  void setSpanId(int id);

  int smallBreaksBeforeBigBreak(const BreakConfig& config) const;
  BreakType breakType(const BreakConfig& config) const;
  void resetCycle(bool notify = true);
  void makeNextBreakBig(const BreakConfig& config, bool notify = true);
  void makeNextBreakLastSmallBeforeBig(const BreakConfig& config, bool notify = true);
  void recordCompletedBreak(BreakType completedBreakType, bool notify = true);
  bool advanceAfterCompletedBreak(bool notify = true);

 protected:
  void clear();
  void notifyChanged();

  std::function<void()> m_onChanged;
  bool m_isActive = false;
  bool m_entryBreakDone = false;
  int m_cyclesRemaining = 0;
  int m_totalCycles = 0;
  int m_spanId = -1;
  int m_completedSmallBreaks = 0;
};

struct BreakCompletion {
  BreakType completedBreakType;
  bool focusCompleted = false;
  bool wasPostponed = false;
  int cycleResetThresholdSeconds = 0;
  int nextSessionBaseSeconds = 0;
  int nextSessionAdjustedSeconds = 0;
};

class PostBreakState {
 public:
  void setPending(const BreakCompletion& completion);
  bool isActive() const;
  BreakType breakType() const;
  bool wasPostponed() const;
  int cycleResetThresholdSeconds() const;
  int nextSessionBaseSeconds() const;
  int nextSessionAdjustedSeconds() const;
  int idleSeconds() const;
  void tickIdleSeconds();
  void addIdleSeconds(int secs);
  void clear();

 protected:
  bool m_isActive = false;
  BreakType m_breakType = BreakType::Small;
  bool m_wasPostponed = false;
  int m_cycleResetThresholdSeconds = 0;
  int m_nextSessionBaseSeconds = 0;
  int m_nextSessionAdjustedSeconds = 0;
  int m_idleSeconds = 0;
};

// Manages application state data related to break scheduling and timing
class AppData : public QObject {
  Q_OBJECT
 public:
  AppData(QObject* parent, SanePreferences* preferences);

  BreakScheduleState& schedule();
  const BreakScheduleState& schedule() const;
  PauseState& pause();
  const PauseState& pause() const;
  MeetingState& meeting();
  const MeetingState& meeting() const;
  FocusState& focus();
  const FocusState& focus() const;
  PostBreakState& postBreak();
  const PostBreakState& postBreak() const;

  BreakConfig currentBreakConfig() const;
  BreakType breakType() const;
  int smallBreaksBeforeBigBreak() const;
  BreakCompletion completeBreak();
  int breakDuration() const;
  void resetBreakCycle();
  void makeNextBreakBig();
  void makeNextBreakLastSmallBeforeBig();
  void finalizePendingPostBreak(bool resetCycle, bool undoPostponeShrink);

 signals:
  void changed();

 protected:
  BreakScheduleState m_schedule;
  PauseState m_pause;
  MeetingState m_meeting;
  FocusState m_focus;
  PostBreakState m_postBreak;
  SanePreferences* preferences;
};

struct BreaksDataInit {
  int totalSeconds;
  int flashFor;
};

class BreaksData {
 public:
  BreaksData(BreaksDataInit data);
  void init(BreaksDataInit data);

  int totalSeconds();

  bool isForceBreak();
  void tickSecondsToForceBreak();
  void resetSecondsToForceBreak();

  int remainingSeconds();
  void tickRemainingTime();
  void resetRemainingTime();

  int numberForceBreakExits();
  void recordForceBreakExit();

 protected:
  int m_totalSeconds;
  int m_secondsToForceBreak;
  int m_remainingSeconds;
  int m_numberForceBreakExits = 0;
};
