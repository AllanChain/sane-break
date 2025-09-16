// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qglobal.h>

#include <QList>
#include <QObject>
#include <QTimer>

#include "app/break-window.h"
#include "app/sound-player.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/preferences.h"
#ifdef Q_OS_LINUX
#include "app/layer-shell/interface.h"
#endif

class BreakWindows : public AbstractBreakWindows {
  Q_OBJECT

 public:
  BreakWindows(QObject *parent = nullptr);
  ~BreakWindows() = default;

  void create(BreakType, SanePreferences *) override;
  void destroy() override;
  void setTime(int remainingTime) override;
  void showFullScreen() override;
  void showFlashPrompt() override;
  void showButtons(Buttons, bool show = true) override;
  void playEnterSound(BreakType, SanePreferences *) override;
  void playExitSound(BreakType, SanePreferences *) override;

 private:
  QList<BreakWindow *> m_windows;
  SoundPlayer *soundPlayer;
  QTimer *clockUpdateTimer;
  void updateClocks();

#ifdef Q_OS_LINUX
  LayerShellInterface *layerShell = nullptr;
#endif
};
