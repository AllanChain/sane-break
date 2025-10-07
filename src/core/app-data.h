// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <memory>

#include "core/flags.h"
#include "core/preferences.h"

struct BreaksDataInit {
  int totalSeconds;
  int flashFor;
};

class BreaksData {
 public:
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

class AppData : public QObject {
  Q_OBJECT
 public:
  AppData(QObject* parent, SanePreferences* preferences);
  std::unique_ptr<BreaksData> breaks;

  BreakType breakType();
  int smallBreaksBeforeBigBreak();
  void finishAndStartNextCycle();
  void resetBreakCycle();
  void makeNextBreakBig();
  void makeNextBreakLastSmallBeforeBig();

  int secondsToNextBreak();
  void tickSecondsToNextBreak();
  void addSecondsToNextBreak(int secs);
  void zeroSecondsToNextBreak();
  void resetSecondsToNextBreak();
  // reset secondsToNextBreak only if it's smaller than the interval
  void refillSecondsToNextBreak();

  int secondsSinceLastBreak();
  void tickSecondsSinceLastBreak();
  void resetSecondsSinceLastBreak();

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
  int m_secondsSinceLastBreak = 0;
  int m_secondsPaused = 0;
  PauseReasons m_pauseReasons = {};

  SanePreferences* preferences;
};
