// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "core/preferences.h"

class AbstractMeetingPrompt : public QObject {
  Q_OBJECT
 public:
  AbstractMeetingPrompt(QObject* parent, SanePreferences* preferences)
      : QObject(parent), preferences(preferences) {};
  virtual void showEndPrompt() = 0;
  virtual void closeEndPrompt() = 0;
  virtual void setTime(int seconds) = 0;

 signals:
  void breakNowRequested();
  void breakLaterRequested(int seconds);
  void extendRequested(int seconds);

 protected:
  SanePreferences* preferences;
};
