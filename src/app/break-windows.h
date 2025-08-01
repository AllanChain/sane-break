// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QList>
#include <QObject>
#include <QTimer>

#include "core/break-windows.h"
#include "core/flags.h"
#include "core/preferences.h"
#include "gui/break-window.h"
#include "gui/sound-player.h"

class BreakWindows : public AbstractBreakWindows {
  Q_OBJECT

 public:
  BreakWindows(QObject *parent = nullptr);
  ~BreakWindows() = default;

  void create(SaneBreak::BreakType, SanePreferences *) override;
  void destroy() override;
  void setTime(int remainingTime) override;
  void showFullScreen() override;
  void showFlashPrompt() override;
  void showButtons(Buttons, bool show = true) override;
  void playEnterSound(SaneBreak::BreakType, SanePreferences *) override;
  void playExitSound(SaneBreak::BreakType, SanePreferences *) override;

 private:
  QList<BreakWindow *> m_windows;
  SoundPlayer *soundPlayer;
  QTimer *clockUpdateTimer;
  void updateClocks();
};
