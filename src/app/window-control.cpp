// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "window-control.h"

#include <qglobal.h>

#include <QApplication>
#include <QAudioOutput>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <utility>

#include "config.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/window-control.h"
#include "idle/factory.h"
#include "lib/screen-lock.h"
#ifdef Q_OS_LINUX
#include <QGuiApplication>
#include <QPluginLoader>

#include "gui/layer-shell/interface.h"
#include "lib/linux/system-check.h"
#endif

#include "core/preferences.h"
#include "gui/break-window.h"
#include "gui/sound-player.h"

BreakWindowControl::BreakWindowControl(const WindowDependencies &deps, QObject *parent)
    : AbstractWindowControl(deps, parent) {
  soundPlayer = new SoundPlayer(this);
#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() == "wayland" && LinuxSystemSupport::layerShell) {
    QPluginLoader loader("libsanebreak_layer_shell");
    if (!loader.load()) {
      qWarning("Fail to find layer-shell plugin. Window layout may go wrong.");
    } else {
      layerShell = qobject_cast<LayerShellInterface *>(loader.instance());
      if (!layerShell) {
        qWarning("Fail to load layer-shell plugin. Window layout may go wrong.");
      } else {
      }
    }
  }
#endif
}

BreakWindowControl *BreakWindowControl::create(SanePreferences *preferences,
                                               QObject *parent) {
  WindowDependencies deps = {
      .preferences = preferences,
      .idleTimer = createIdleTimer(parent),
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
#ifdef Q_OS_LINUX
    if (layerShell) layerShell->layout(w->windowHandle());
#endif
    // GNOME mutter will make the window black if show full screen
    // See https://gitlab.gnome.org/GNOME/mutter/-/issues/2520
    // GNOME mutter will also refuse to make a window always on top if maximized.
    // Therefore, we use the same `show()` with and without Wayland workaround.
    w->show();
  }
}

void BreakWindowControl::show(SaneBreak::BreakType type) {
  AbstractWindowControl::show(type);
  soundPlayer->play(type == SaneBreak::BreakType::Small
                        ? preferences->smallStartBell->get()
                        : preferences->bigStartBell->get());
}

void BreakWindowControl::close() {
  AbstractWindowControl::close();
  if (m_remainingSeconds <= 0) {
    soundPlayer->play(m_currentType == SaneBreak::BreakType::Small
                          ? preferences->smallEndBell->get()
                          : preferences->bigEndBell->get());
  }
}

void BreakWindowControl::lockScreen() { ::lockScreen(); }
