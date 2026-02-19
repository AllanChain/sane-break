// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class AbstractMeetingPrompt : public QObject {
  Q_OBJECT
 public:
  using QObject::QObject;
  virtual void showEndPrompt() = 0;
  virtual void closeEndPrompt() = 0;
  virtual void resetTimeout() = 0;
  virtual bool isShowing() const = 0;

 signals:
  void breakNowRequested();
  void breakLaterRequested(int seconds);
  void extendRequested(int seconds);
};
