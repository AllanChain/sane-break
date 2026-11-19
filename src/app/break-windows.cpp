// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-windows.h"

#include <qglobal.h>

#include <QApplication>
#include <QAudioOutput>
#include <QGuiApplication>
#include <QList>
#include <QLocale>
#include <QMargins>
#include <QMediaPlayer>
#include <QObject>
#include <QRandomGenerator>
#include <QScreen>
#include <QSettings>
#include <QTime>
#include <QTimer>
#include <concepts>
#include <utility>

#include "app/break-window.h"
#include "app/sound-player.h"
#include "config.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/preferences.h"
#include "heads-up-window.h"
#include "idle/factory.h"

#ifdef Q_OS_MACOS
#include "lib/macos/workspace.h"
#endif
#ifdef Q_OS_LINUX
#include <QGuiApplication>
#include <QPluginLoader>

#include "app/layer-shell/interface.h"
#include "lib/linux/system-check.h"
#endif

namespace {

template <typename T>
concept ScreenAwareWindow = requires(T* w, QScreen* s) {
  { w->screenIdentity() } -> std::same_as<QScreen*>;
  w->hide();
  w->deleteLater();
};

template <ScreenAwareWindow T, typename Pred>
void removeWindowsIf(QList<T*>& windows, Pred pred) {
  QList<T*> toDestroy;
  for (T* w : std::as_const(windows))
    if (pred(w)) toDestroy << w;
  for (T* w : std::as_const(toDestroy)) {
    windows.removeOne(w);
    // Hide synchronously before Qt migrates the native surface to another screen.
    // Destruction must remain deferred (notably during macOS screen removal).
    w->hide();
    w->deleteLater();
  }
}

}  // namespace

BreakWindows::BreakWindows(QObject* parent) : AbstractBreakWindows(parent) {
  soundPlayer = new SoundPlayer(this);
  clockUpdateTimer = new QTimer(this);
  connect(clockUpdateTimer, &QTimer::timeout, this, &BreakWindows::updateClocks);

  // A display switch can add a screen, remove another and only then settle on
  // its final resolution. Debounce the whole sequence, not only screenAdded.
  m_screenDebounce = new QTimer(this);
  m_screenDebounce->setSingleShot(true);
  m_screenDebounce->setInterval(400);
  connect(m_screenDebounce, &QTimer::timeout, this, [this] { reconcileScreens(); });
  connect(qApp, &QGuiApplication::screenRemoved, this, [this](QScreen* screen) {
    // Immediate (synchronous) removal — safe before QScreen destruction because we
    // only compare pointers, never dereference the dying screen.
    removeWindowsIf(m_windows,
                    [screen](auto* w) { return w->screenIdentity() == screen; });
    removeWindowsIf(m_headsUpWindows,
                    [screen](auto* w) { return w->screenIdentity() == screen; });
    scheduleScreenReconcile();
  });
  connect(qApp, &QGuiApplication::screenAdded, this, [this](QScreen* screen) {
    watchScreen(screen);
    scheduleScreenReconcile();
  });
  connect(qApp, &QGuiApplication::primaryScreenChanged, this,
          &BreakWindows::scheduleScreenReconcile);
  for (QScreen* screen : QGuiApplication::screens()) watchScreen(screen);

#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() == "wayland" && LinuxSystemSupport::layerShell) {
    QPluginLoader loader("libsanebreak_layer_shell");
    if (!loader.load()) {
      qWarning() << "Fail to load layer-shell plugin. Window layout may go wrong:"
                 << loader.errorString();
    } else {
      layerShell = qobject_cast<LayerShellInterface*>(loader.instance());
      if (!layerShell) {
        qWarning("Fail to instantiate layer-shell plugin. Window layout may go wrong.");
      }
    }
  }
#endif
}

QString rollMessages(const QStringList messagesToRoll) {
  if (!messagesToRoll.empty()) {
    int randomIndex = QRandomGenerator::global()->bounded(messagesToRoll.size());
    return messagesToRoll[randomIndex];
  }
  return "";
}

BreakWindowMessage breakMessage(BreakType type, SanePreferences* preferences) {
  if (type == BreakType::Small) {
    return {
        .prompt = preferences->smallMessages->defaultValue()[0],
        .fullScreen = rollMessages(preferences->smallMessages->get()),
    };
  } else {
    return {
        .prompt = preferences->bigMessages->defaultValue()[0],
        .fullScreen = rollMessages(preferences->bigMessages->get()),
    };
  }
}

BreakWindowTheme breakTheme(BreakType type, SanePreferences* preferences) {
  if (type == BreakType::Small) {
    return {
        .background = preferences->smallBackgroundColor->get(),
        .flash = preferences->smallFlashColor->get(),
        .messageColor = preferences->messageColor->get(),
        .countDownColor = preferences->countDownColor->get(),
        .backgroundImage = preferences->smallBackgroundImage->get(),
    };
  } else {
    return {
        .background = preferences->bigBackgroundColor->get(),
        .flash = preferences->bigFlashColor->get(),
        .messageColor = preferences->messageColor->get(),
        .countDownColor = preferences->countDownColor->get(),
        .backgroundImage = preferences->bigBackgroundImage->get(),
    };
  }
}

BreakWindowShow breakShow(BreakType type, SanePreferences* preferences) {
  if (type == BreakType::Small) {
    return {
        .prograssBar = preferences->smallBreakShowProgressBar->get(),
        .countdown = preferences->smallBreakShowCountdown->get(),
        .clock = preferences->smallBreakShowClock->get(),
        .endTime = preferences->smallBreakShowEndTime->get(),
        .buttons = preferences->smallBreakShowButtons->get(),
    };
  } else {
    return {
        .prograssBar = preferences->bigBreakShowProgressBar->get(),
        .countdown = preferences->bigBreakShowCountdown->get(),
        .clock = preferences->bigBreakShowClock->get(),
        .endTime = preferences->bigBreakShowEndTime->get(),
        .buttons = preferences->bigBreakShowButtons->get(),
    };
  }
}

BreakWindowData BreakWindows::createData(BreakType type, SanePreferences* preferences,
                                         int breakDuration, bool isPostponed) {
  const int flashSpeed = preferences->flashSpeed->get();
  return {
      .totalSeconds = breakDuration,
      .isPostponed = isPostponed,
      .flashAnimationDuration = flashSpeed > 0 ? 60000 / flashSpeed : 0,
      .message = breakMessage(type, preferences),
      .theme = breakTheme(type, preferences),
      .show = breakShow(type, preferences),
  };
}
void BreakWindows::create(BreakWindowData data) {
  m_activeBreak.emplace();
  m_activeBreak->data = data;
  // Initialize remainingTime to the full duration: a freshly created break has
  // not started counting down yet.  Without this, remainingTime defaults to 0
  // (the struct default), so createOnScreen() would call setTime(0, ...) before
  // AppStateBreak::enter() sets the real time, causing a brief visual glitch
  // where the progress bar animates to empty and then back to full.
  m_activeBreak->remainingTime = data.totalSeconds;
  // Do not rebuild newly-created windows for a previous display change.
  m_screenDebounce->stop();
  QList<QScreen*> screens = QApplication::screens();
  for (QScreen* screen : std::as_const(screens)) {
    createOnScreen(screen);
    if (!isValidScreen(screen)) scheduleScreenReconcile();
  }
  if (screens.isEmpty()) scheduleScreenReconcile();
  updateClocks();  // Set the initial clock
  clockUpdateTimer->start(3000);
}
void BreakWindows::create(BreakType type, SanePreferences* preferences,
                          int breakDuration, bool isPostponed) {
  create(createData(type, preferences, breakDuration, isPostponed));
}

void BreakWindows::destroy() {
  for (auto w : std::as_const(m_windows)) {
    w->close();
    w->deleteLater();
  }
  m_windows.clear();
  m_activeBreak.reset();
  m_screenDebounce->stop();
  clockUpdateTimer->stop();
}

void BreakWindows::playEnterSound(BreakType type, SanePreferences* preferences) {
  soundPlayer->play(type == BreakType::Small ? preferences->smallStartBell->get()
                                             : preferences->bigStartBell->get());
}

void BreakWindows::playExitSound(BreakType type, SanePreferences* preferences) {
  soundPlayer->play(type == BreakType::Small ? preferences->smallEndBell->get()
                                             : preferences->bigEndBell->get());
}

void BreakWindows::setTime(int remainingTime) {
  QTime now = QTime::currentTime();
  QTime estimatedEndTime = now.addSecs(remainingTime);
  // Round time to seconds
  if (estimatedEndTime.msec() > 500) {
    estimatedEndTime = estimatedEndTime.addMSecs(500);
  }
  QString endTime = QLocale::system().toString(estimatedEndTime, QLocale::ShortFormat);
  // Remember so windows added mid-break (hot-plug) start already time-synced.
  if (m_activeBreak) {
    m_activeBreak->remainingTime = remainingTime;
    m_activeBreak->endTime = endTime;
  }
  for (auto w : std::as_const(m_windows)) {
    w->setTime(remainingTime, endTime);
  }
}
void BreakWindows::updateClocks() {
  QTime now = QTime::currentTime();
  QString hourMinute = QLocale::system().toString(now, QLocale::ShortFormat);
  for (auto w : std::as_const(m_windows)) {
    w->setClock(hourMinute);
  }
}
void BreakWindows::showFullScreen() {
  if (m_activeBreak) m_activeBreak->phase = Phase::FullScreen;
  for (auto w : std::as_const(m_windows)) w->showFullScreen();
}
void BreakWindows::showFlashPrompt() {
  if (m_activeBreak) m_activeBreak->phase = Phase::Prompt;
  for (auto w : std::as_const(m_windows)) w->showFlashPrompt();
}
void BreakWindows::showButtons(Buttons buttons, bool show) {
  if (m_activeBreak) {
    m_activeBreak->buttons = buttons;
    m_activeBreak->buttonsVisible = show;
  }
  for (auto w : std::as_const(m_windows)) w->showButtons(buttons, show);
}
void BreakWindows::showHeadsUp(int totalSeconds, BreakType breakType,
                               SanePreferences* preferences) {
  if (!m_headsUpWindows.isEmpty() && m_activeHeadsUp &&
      (m_activeHeadsUp->totalSeconds != totalSeconds ||
       m_activeHeadsUp->breakType != breakType)) {
    hideHeadsUp();
  }
  if (!m_headsUpWindows.isEmpty()) return;
  // The pill tints its fill with the prompt flash colour, so it shares the theme
  // selector with the full-screen windows.
  m_activeHeadsUp.emplace(ActiveHeadsUp{
      .totalSeconds = totalSeconds,
      .breakType = breakType,
      .remaining = totalSeconds,
      .theme = breakTheme(breakType, preferences),
  });
  m_screenDebounce->stop();
  QList<QScreen*> screens = QApplication::screens();
  for (QScreen* screen : std::as_const(screens)) {
    createHeadsUpOnScreen(screen);
    if (!isValidScreen(screen)) scheduleScreenReconcile();
  }
  if (screens.isEmpty()) scheduleScreenReconcile();
}
void BreakWindows::setHeadsUpTime(int remainingTime) {
  if (m_activeHeadsUp) m_activeHeadsUp->remaining = remainingTime;
  for (auto* w : std::as_const(m_headsUpWindows)) {
    w->setTime(remainingTime);
  }
}
void BreakWindows::hideHeadsUp() {
  for (auto* w : std::as_const(m_headsUpWindows)) {
    w->close();
    w->deleteLater();
  }
  m_headsUpWindows.clear();
  m_activeHeadsUp.reset();
}

bool BreakWindows::isValidScreen(QScreen* screen) {
  return screen && !screen->geometry().isEmpty() &&
         !screen->availableGeometry().isEmpty();
}

void BreakWindows::createOnScreen(QScreen* screen) {
  if (!isValidScreen(screen)) return;
  BreakWindow* w = new BreakWindow(m_activeBreak->data);
  m_windows.append(w);
  w->initSize(screen);  // records screen identity + pins QWindow::screen()
#ifdef Q_OS_LINUX
  if (layerShell) layerShell->layout(w->windowHandle());
#endif
  // GNOME mutter will make the window black if show full screen
  // See https://gitlab.gnome.org/GNOME/mutter/-/issues/2520
  // GNOME mutter will also refuse to make a window always on top if maximized.
  // Therefore, we use the same `show()` with and without Wayland workaround.
  w->show();
  // Create the window directly into the current phase so a hot-plugged display
  // doesn't pop in as a small prompt and then animate to fullscreen.
  if (m_activeBreak->phase == Phase::FullScreen)
    w->showFullScreen();
  else
    w->showFlashPrompt();
  w->showButtons(m_activeBreak->buttons, m_activeBreak->buttonsVisible);
  w->setTime(m_activeBreak->remainingTime, m_activeBreak->endTime);
  connect(w, &BreakWindow::lockScreenRequested, this,
          &BreakWindows::lockScreenRequested);
  connect(w, &BreakWindow::exitForceBreakRequested, this,
          &BreakWindows::exitForceBreakRequested);
  connect(w, &BreakWindow::pauseMediaRequested, this,
          &BreakWindows::pauseMediaRequested);
  connect(w, &QObject::destroyed, this, [this](QObject* obj) {
    // destroyed fires from ~QObject; only use the pointer for identity (removeOne),
    // never dereference it as a BreakWindow.
    m_windows.removeOne(static_cast<BreakWindow*>(obj));
  });
}

void BreakWindows::createHeadsUpOnScreen(QScreen* screen) {
  if (!isValidScreen(screen)) return;
  auto* w = new HeadsUpWindow(
      m_activeHeadsUp->totalSeconds, m_activeHeadsUp->theme.background,
      m_activeHeadsUp->theme.flash, m_activeHeadsUp->theme.messageColor);
  m_headsUpWindows.append(w);
  w->initSize(screen);
#ifdef Q_OS_LINUX
  if (layerShell) layerShell->layout(w->windowHandle(), QMargins(0, 16, 0, 0));
#endif
  w->show();
#ifdef Q_OS_MACOS
  macSetAllWorkspaces(w->windowHandle());
#endif
  w->setTime(m_activeHeadsUp->remaining);
  connect(w, &HeadsUpWindow::clicked, this, &BreakWindows::startBreakRequested);
  connect(w, &QObject::destroyed, this, [this](QObject* obj) {
    m_headsUpWindows.removeOne(static_cast<HeadsUpWindow*>(obj));
  });
}

void BreakWindows::reconcileScreens() {
  if (!m_activeBreak && !m_activeHeadsUp) return;
  // Recreating also refreshes translucent native backing surfaces and background
  // image scaling after a monitor/DPI switch. Do not call destroy(): it clears
  // the remembered phase, time and button state.
  removeWindowsIf(m_windows, [](auto*) { return true; });
  removeWindowsIf(m_headsUpWindows, [](auto*) { return true; });
  QList<QScreen*> current = QGuiApplication::screens();
  bool pendingGeometry = current.isEmpty();
  for (QScreen* screen : current)
    if (!isValidScreen(screen)) pendingGeometry = true;
  if (m_activeBreak) {
    for (QScreen* screen : current) {
      if (!isValidScreen(screen)) continue;
      createOnScreen(screen);
    }
  }
  if (m_activeHeadsUp) {
    for (QScreen* screen : current) {
      if (!isValidScreen(screen)) continue;
      createHeadsUpOnScreen(screen);
    }
  }
  updateClocks();
  // Empty geometry during display negotiation must not permanently drop an
  // overlay. Retry briefly; later screen signals start a fresh retry budget.
  if (pendingGeometry && m_screenRetries > 0) {
    --m_screenRetries;
    m_screenDebounce->start();
  }
}

void BreakWindows::watchScreen(QScreen* screen) {
  connect(screen, &QScreen::geometryChanged, this,
          &BreakWindows::scheduleScreenReconcile);
  connect(screen, &QScreen::availableGeometryChanged, this,
          &BreakWindows::scheduleScreenReconcile);
  connect(screen, &QScreen::logicalDotsPerInchChanged, this,
          &BreakWindows::scheduleScreenReconcile);
  connect(screen, &QScreen::physicalDotsPerInchChanged, this,
          &BreakWindows::scheduleScreenReconcile);
}

void BreakWindows::scheduleScreenReconcile() {
  if (!m_activeBreak && !m_activeHeadsUp) return;
  m_screenRetries = 5;
  m_screenDebounce->start();
}
