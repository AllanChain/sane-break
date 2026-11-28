// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QFlags>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "core/flags.h"
#include "core/preferences.h"

struct BreakWindowTheme {
  QColor background;
  QColor flash;
  QColor messageColor;
  QColor countDownColor;
  QString backgroundImage;
};
struct BreakWindowShow {
  bool prograssBar;
  bool countdown;
  bool clock;
  bool endTime;
  bool buttons;
};
struct BreakWindowMessage {
  QString prompt;
  QString fullScreen;
};

struct BreakWindowData {
  BreakType type = BreakType::Small;
  int totalSeconds = 0;
  bool isPostponed = false;
  int flashAnimationDuration = 0;
  BreakWindowMessage message;
  BreakWindowTheme theme;
  BreakWindowShow show;

  static BreakWindowData create(BreakType type, SanePreferences* preferences,
                                int breakDuration, bool isPostponed = false);
  QJsonObject toCustomizationJson() const;
  void applyCustomizationJson(const QJsonObject& json);
};

class AbstractBreakWindows : public QObject {
  Q_OBJECT
 public:
  enum class Button {
    LockScreen = 1 << 0,
    ExitForceBreak = 1 << 1,
    PauseMedia = 1 << 2,
  };
  Q_DECLARE_FLAGS(Buttons, Button)
  using QObject::QObject;
  ~AbstractBreakWindows() = default;
  virtual void create(BreakWindowData data) = 0;
  void create(BreakType type, SanePreferences* preferences, int breakDuration,
              bool isPostponed = false) {
    create(BreakWindowData::create(type, preferences, breakDuration, isPostponed));
  }
  virtual void destroy() = 0;
  virtual void setTime(int remainingTime) = 0;
  virtual void showFullScreen() = 0;
  virtual void showFlashPrompt() = 0;
  virtual void showButtons(Buttons, bool show = true) = 0;
  virtual void playEnterSound(BreakType, SanePreferences*) = 0;
  virtual void playExitSound(BreakType, SanePreferences*) = 0;
  virtual void showHeadsUp(int totalSeconds, BreakType breakType,
                           SanePreferences* preferences) = 0;
  virtual void setHeadsUpTime(int remainingTime) = 0;
  virtual void hideHeadsUp() = 0;

 signals:
  void lockScreenRequested();
  void exitForceBreakRequested();
  void pauseMediaRequested();
  void startBreakRequested();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractBreakWindows::Buttons)
