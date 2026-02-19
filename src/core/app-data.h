// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>
#include <QObject>

#include "core/flags.h"
#include "core/preferences.h"

struct PostponeData {
  int plannedSecondsToPostpone = 0;
  // Seconds to next break when the break happens.
  // Typically 0 but may be larger than 0 if having the break before the planned time.
  int actualSecondsToNextBreakWhenBreak = 0;
  bool isPostponing();
  int secondsPostponed();
  void reset();
};

struct MeetingData {
  bool isActive = false;
  int secondsRemaining = 0;
  int totalSeconds = 0;
  void clear();
};

// Manages application state data related to break scheduling and timing
class AppData : public QObject {
  Q_OBJECT
 public:
  AppData(QObject* parent, SanePreferences* preferences);

  BreakType breakType();
  int smallBreaksBeforeBigBreak();
  void finishAndStartNextCycle();
  int breakDuration();
  void resetBreakCycle();
  void makeNextBreakBig();
  void makeNextBreakLastSmallBeforeBig();

  int secondsToNextBreak();
  void tickSecondsToNextBreak();
  void resetSecondsToNextBreak();
  // reset secondsToNextBreak only if it's smaller than the interval
  void refillSecondsToNextBreak();

  void postpone(int secs);
  bool isPostponing();
  // set secondsToNextBreak to zero and record actual postponed seconds
  void earlyBreak();

  bool isInMeeting() const;
  int meetingSecondsRemaining() const;
  int meetingTotalSeconds() const;
  void setMeetingData(int secondsRemaining, int totalSeconds);
  void clearMeetingData();
  void tickMeetingRemaining();
  void subtractMeetingRemaining(int secs);
  void extendMeeting(int secs);

  void setSecondsToNextBreak(int secs);

  int secondsPaused();
  void tickSecondsPaused();
  void addSecondsPaused(int secs);
  void resetSecondsPaused();

  PauseReasons pauseReasons();
  void addPauseReasons(PauseReasons);
  void removePauseReasons(PauseReasons);
  void clearPauseReasons();

 signals:
  void changed();

 protected:
  int m_breakCycleCount = 1;
  int m_secondsToNextBreak;
  int m_secondsPaused = 0;
  PauseReasons m_pauseReasons = {};
  PostponeData m_postponeData;
  MeetingData m_meetingData;

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
