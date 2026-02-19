// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meeting-prompt.h"

#include <QDialog>
#include <QTimer>
#include <Qt>

#ifdef Q_OS_MACOS
#include "lib/macos/workspace.h"
#endif

#include "ui_meeting-prompt.h"

void MeetingPrompt::showEndPrompt() {
  if (dialog) return;

  dialog = new QDialog();
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  ui = new Ui::MeetingPrompt();
  ui->setupUi(dialog);

  timeoutRemaining = 60;
  updateBreakNowText();

  timeoutTimer = new QTimer(dialog);
  timeoutTimer->setInterval(1000);
  connect(timeoutTimer, &QTimer::timeout, this, [this]() {
    timeoutRemaining--;
    if (timeoutRemaining <= 0) {
      timeoutTimer->stop();
      closeEndPrompt();
      emit breakLaterRequested(0);
      return;
    }
    updateBreakNowText();
  });
  timeoutTimer->start();

  connect(ui->breakNow, &QPushButton::clicked, this, [this]() {
    if (dialog) dialog->done(QDialog::Accepted);
    emit breakNowRequested();
  });
  connect(ui->breakLater, &QPushButton::clicked, this, [this]() {
    if (dialog) dialog->done(QDialog::Rejected);
  });
  connect(ui->notYet, &QPushButton::clicked, this, [this]() {
    if (dialog) dialog->done(QDialog::Accepted);
    emit extendRequested(1800);
  });
  connect(dialog, &QDialog::rejected, this,
          [this]() { emit breakLaterRequested(300); });

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
#ifdef Q_OS_MACOS
  macForceActivation();
#endif
}

void MeetingPrompt::closeEndPrompt() {
  if (dialog) dialog->done(QDialog::Accepted);
}

void MeetingPrompt::resetTimeout() {
  timeoutRemaining = 60;
  if (ui) updateBreakNowText();
}

bool MeetingPrompt::isShowing() const { return dialog != nullptr; }

void MeetingPrompt::updateBreakNowText() {
  if (ui) {
    ui->breakNow->setArgs({timeoutRemaining});
  }
}
