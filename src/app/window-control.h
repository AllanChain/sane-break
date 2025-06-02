// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qglobal.h>

#include <QList>
#include <QObject>
#include <QTimer>

#include "core/flags.h"
#include "core/preferences.h"
#include "core/window-control.h"
#include "gui/break-window.h"  // IWYU pragma: export
#include "gui/sound-player.h"
#ifdef Q_OS_LINUX
#include "gui/layer-shell/interface.h"
#endif

class BreakWindowControl : public AbstractWindowControl {
  Q_OBJECT

 public:
  BreakWindowControl(const WindowDependencies &deps, QObject *parent = nullptr);
  ~BreakWindowControl() = default;
  static BreakWindowControl *create(SanePreferences *preferences,
                                    QObject *parent = nullptr);

  void show(SaneBreak::BreakType type) override;
  void lockScreen() override;
  void close() override;

 protected:
  void createWindows(SaneBreak::BreakType type) override;

 private:
  SoundPlayer *soundPlayer;
#ifdef Q_OS_LINUX
  LayerShellInterface *layerShell = nullptr;
#endif
  void tick();
  void forceBreak();
  void onIdleStart();
  void onIdleEnd();
};
