// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "window-manager.h"

#include <qglobal.h>

#include <utility>

#include "config.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/window-control.h"
#include "lib/timer.h"

#ifdef WITH_LAYER_SHELL
#include <LayerShellQt/shell.h>
#endif
#include <QApplication>
#include <QAudioOutput>
#include <QGuiApplication>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "core/preferences.h"
#include "gui/break-window.h"
#include "gui/sound-player.h"

BreakWindowControl::BreakWindowControl(const WindowDependencies &deps, QObject *parent)
    : AbstractWindowControl(deps, parent) {
  soundPlayer = new SoundPlayer(this);

#ifdef WITH_LAYER_SHELL
  if (QGuiApplication::platformName() == "wayland")
    LayerShellQt::Shell::useLayerShell();
#endif
}

BreakWindowControl *BreakWindowControl::create(SanePreferences *preferences,
                                               QObject *parent) {
  auto countDownTimer = new Timer();
  auto forceBreakTimer = new Timer();
  auto idleTimer = SystemIdleTime::createIdleTimer();
  WindowDependencies deps = {
      .preferences = preferences,
      .countDownTimer = countDownTimer,
      .idleTimer = idleTimer,
      .forceBreakTimer = forceBreakTimer,
  };
  return new BreakWindowControl(deps, parent);
}

void BreakWindowControl::createWindows(SaneBreak::BreakType type) {
  QList<QScreen *> screens = QApplication::screens();

  BreakData data = breakData(type);
  for (QScreen *screen : std::as_const(screens)) {
    BreakWindow *w = new BreakWindow(data);
    m_windows.append(w);
    w->initSize(screen);
  }
}

void BreakWindowControl::show(SaneBreak::BreakType type) {
  AbstractWindowControl::show(type);
  soundPlayer->play(type == SaneBreak::BreakType::Small
                        ? preferences->smallStartBell->get()
                        : preferences->bigStartBell->get());
}

void BreakWindowControl::close() {
  if (m_remainingTime <= 0) {
    soundPlayer->play(m_currentType == SaneBreak::BreakType::Small
                          ? preferences->smallEndBell->get()
                          : preferences->bigEndBell->get());
  }
}
void BreakWindowControl::deleteWindows() {
  for (auto w : std::as_const(m_windows)) {
    w->close();
    w->deleteLater();
  }
  m_windows.clear();
}
