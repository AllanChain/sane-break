// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>
#include <QPointer>
#include <QTimer>

#include "core/meeting-prompt.h"

namespace Ui {
class MeetingPrompt;
}

class MeetingPrompt : public AbstractMeetingPrompt {
  Q_OBJECT
 public:
  using AbstractMeetingPrompt::AbstractMeetingPrompt;
  void showEndPrompt() override;
  void closeEndPrompt() override;
  void resetTimeout() override;
  bool isShowing() const override;

 private:
  QPointer<QDialog> dialog;
  Ui::MeetingPrompt* ui = nullptr;
  QTimer* timeoutTimer = nullptr;
  int timeoutRemaining = 60;

  void updateBreakNowText();
};
