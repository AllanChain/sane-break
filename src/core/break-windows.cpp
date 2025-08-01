// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/break-windows.h"

#include <QRandomGenerator>
#include <QString>
#include <utility>

#include "core/flags.h"
#include "core/preferences.h"

void AbstractBreakWindows::setTime(int remainingTime) {
  for (auto w : std::as_const(m_windows)) w->setTime(remainingTime);
}
void AbstractBreakWindows::showFullScreen() {
  for (auto w : std::as_const(m_windows)) w->showFullScreen();
}
void AbstractBreakWindows::showFlashPrompt() {
  for (auto w : std::as_const(m_windows)) w->showFlashPrompt();
}
void AbstractBreakWindows::showButtons(AbstractBreakWindow::Buttons buttons) {
  for (auto w : std::as_const(m_windows)) w->showButtons(buttons);
}
void AbstractBreakWindows::create(SaneBreak::BreakType type,
                                  SanePreferences *preferences) {
  QStringList messagesToRoll = type == SaneBreak::BreakType::Big
                                   ? preferences->bigMessages->get()
                                   : preferences->smallMessages->get();
  QString message = "";
  if (!messagesToRoll.empty()) {
    int randomIndex = QRandomGenerator::global()->bounded(messagesToRoll.size());
    message = messagesToRoll[randomIndex];
  }

  BreakWindowData data = {
      .totalSeconds = type == SaneBreak::BreakType::Big ? preferences->bigFor->get()
                                                        : preferences->smallFor->get(),
      .message = message,
      .theme =
          {
              .mainBackground = preferences->backgroundColor->get(),
              .highlightBackground = type == SaneBreak::BreakType::Big
                                         ? preferences->bigHighlightColor->get()
                                         : preferences->smallHighlightColor->get(),
              .messageColor = preferences->messageColor->get(),
              .countDownColor = preferences->countDownColor->get(),
              .flashAnimationDuration = 500 / preferences->flashSpeed->get() * 100,
          },
  };
  createWindows(data);
  for (auto w : std::as_const(m_windows)) {
    connect(w, &AbstractBreakWindow::lockScreenRequested, this,
            &AbstractBreakWindows::lockScreenRequested);
    connect(w, &AbstractBreakWindow::exitForceBreakRequested, this,
            &AbstractBreakWindows::exitForceBreakRequested);
  }
}
void AbstractBreakWindows::destroy() {
  for (auto w : std::as_const(m_windows)) {
    w->close();
    w->deleteLater();
  }
  m_windows.clear();
}
