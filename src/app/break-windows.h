// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qglobal.h>

#include <QList>
#include <QObject>
#include <QScreen>
#include <QTimer>
#include <optional>

#include "app/break-window.h"
#include "app/heads-up-window.h"
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
  BreakWindows(QObject* parent = nullptr);
  ~BreakWindows() = default;

  void create(BreakType, SanePreferences*, int breakDuration,
              bool isPostponed = false) override;
  void create(BreakWindowData);
  static BreakWindowData createData(BreakType, SanePreferences*, int breakDuration,
                                    bool isPostponed = false);
  void destroy() override;
  void setTime(int remainingTime) override;
  void showFullScreen() override;
  void showFlashPrompt() override;
  void showButtons(Buttons, bool show = true) override;
  void playEnterSound(BreakType, SanePreferences*) override;
  void playExitSound(BreakType, SanePreferences*) override;
  void showHeadsUp(int totalSeconds, BreakType breakType,
                   SanePreferences* preferences) override;
  void setHeadsUpTime(int remainingTime) override;
  void hideHeadsUp() override;

 private:
  // Current break phase, remembered so windows created mid-break (e.g. after a
  // display hot-plug) are created directly into the running phase instead of
  // popping in as a prompt and animating to fullscreen.
  enum class Phase { Prompt, FullScreen };

  // Runtime state of the active break, remembered so windows created mid-break
  // (e.g. after a display hot-plug) can be created directly into the current
  // phase/time without resetting the countdown or replaying the enter sound.
  // Engaged while a break is active; disengaged by destroy().
  struct ActiveBreak {
    BreakWindowData data;
    Phase phase = Phase::Prompt;
    int remainingTime = 0;
    QString endTime;
    AbstractBreakWindows::Buttons buttons{};
    bool buttonsVisible = false;
  };
  std::optional<ActiveBreak> m_activeBreak;

  // HeadsUpWindow constructor arguments + remaining time, remembered so
  // hot-plugged screens get a correctly-stated heads-up pill.
  struct ActiveHeadsUp {
    int totalSeconds = 0;
    BreakType breakType = BreakType::Small;
    int remaining = 0;
    QColor bgColor;
    QColor highlightColor;
    QColor textColor;
  };
  std::optional<ActiveHeadsUp> m_activeHeadsUp;

  void createOnScreen(QScreen* screen);
  void createHeadsUpOnScreen(QScreen* screen);
  // Creates windows on newly-attached screens for the active break/heads-up.
  // Debounced via m_screenDebounce; removal is handled elsewhere (screenRemoved
  // handler + per-window destroyed signal).
  void reconcileScreens();
  static bool isValidScreen(QScreen* screen);

  QList<BreakWindow*> m_windows;
  QList<HeadsUpWindow*> m_headsUpWindows;
  SoundPlayer* soundPlayer;
  QTimer* clockUpdateTimer;
  // Debounces screenAdded so we don't read transient/invalid geometry during macOS
  // mid-EDID negotiation or non-transactional multi-screen bursts.
  QTimer* m_screenDebounce = nullptr;
  void updateClocks();

#ifdef Q_OS_LINUX
  LayerShellInterface* layerShell = nullptr;
#endif
};
