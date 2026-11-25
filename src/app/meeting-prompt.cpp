// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meeting-prompt.h"

#include <QDialog>
#include <Qt>
#include <memory>

#ifdef Q_OS_MACOS
#include "lib/macos/workspace.h"
#endif

#include "ui_meeting-prompt.h"

MeetingPrompt::MeetingPrompt(QObject* parent, SanePreferences* preferences)
    : AbstractMeetingPrompt(parent, preferences) {}

MeetingPrompt::~MeetingPrompt() {
  if (dialog) {
    dialog->disconnect(this);
    delete dialog;
  }
}

void MeetingPrompt::showEndPrompt() {
  if (dialog) return;

  dialog = new QDialog();
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  ui = std::make_unique<Ui::MeetingPrompt>();
  ui->setupUi(dialog);
  connect(dialog, &QObject::destroyed, this, [this]() { ui.reset(); });

  ui->breakLater->setArgs({preferences->smallEvery->get() / 60});
  ui->notYet->setArgs({30});

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
  connect(dialog, &QDialog::rejected, this, [this]() { emit breakLaterRequested(); });

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

void MeetingPrompt::setTime(int seconds) {
  if (dialog && ui) ui->breakNow->setArgs({seconds});
}
