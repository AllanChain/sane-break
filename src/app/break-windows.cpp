// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-windows.h"

#include <qglobal.h>

#include <QApplication>
#include <QAudioOutput>
#include <QGuiApplication>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <utility>

#include "config.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/preferences.h"
#include "gui/break-window.h"
#include "gui/sound-player.h"

#ifdef WITH_LAYER_SHELL
#include <LayerShellQt/shell.h>
#endif
BreakWindows::BreakWindows(QObject *parent) : AbstractBreakWindows(parent) {
  soundPlayer = new SoundPlayer(this);

#ifdef WITH_LAYER_SHELL
  if (QGuiApplication::platformName() == "wayland")
    LayerShellQt::Shell::useLayerShell();
#endif
}

void BreakWindows::createWindows(BreakWindowData data) {
  QList<QScreen *> screens = QApplication::screens();
  for (QScreen *screen : std::as_const(screens)) {
    BreakWindow *w = new BreakWindow(data);
    m_windows.append(w);
    w->initSize(screen);
  }
}

void BreakWindows::playEnterSound(SaneBreak::BreakType type,
                                  SanePreferences *preferences) {
  soundPlayer->play(type == SaneBreak::BreakType::Small
                        ? preferences->smallStartBell->get()
                        : preferences->bigStartBell->get());
}

void BreakWindows::playExitSound(SaneBreak::BreakType type,
                                 SanePreferences *preferences) {
  soundPlayer->play(type == SaneBreak::BreakType::Small
                        ? preferences->smallEndBell->get()
                        : preferences->bigEndBell->get());
}
