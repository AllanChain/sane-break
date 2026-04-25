// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/meeting-window.h"

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <QWidget>
#include <Qt>

#include "core/db.h"
#include "core/preferences.h"
#include "ui_meeting-window.h"

namespace {
constexpr int kMinReasonLength = 6;
constexpr auto kHelperTextStyle = "color: #6b7280;";
constexpr auto kErrorTextStyle = "color: #ef4444;";
}  // namespace

MeetingWindow::MeetingWindow(SanePreferences* preferences, BreakDatabase* db,
                             QWidget* parent)
    : QDialog(parent), ui(new Ui::MeetingUI), preferences(preferences), db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  connect(ui->endTime, &QTimeEdit::timeChanged, this, &MeetingWindow::onInputUpdate);
  connect(ui->reasonEdit, &QLineEdit::textChanged, this, &MeetingWindow::onInputUpdate);
  connect(ui->confirmButton, &QPushButton::pressed, this, [this]() {
    emit meetingRequested(ui->endTime->time(), ui->reasonEdit->text().trimmed());
    accept();
  });
  QTime now = QTime::currentTime();
  ui->endTime->setTime(QTime(now.hour(), now.minute()).addSecs(1800));
  onInputUpdate();
}

void MeetingWindow::onInputUpdate() {
  QTime endTime = ui->endTime->time();
  QTime currentTime = QTime::currentTime();
  QString reason = ui->reasonEdit->text().trimmed();
  bool hasFutureEndTime = endTime > currentTime;
  int remainingChars = kMinReasonLength - reason.length();
  bool hasLongEnoughReason = remainingChars <= 0;

  ui->confirmButton->setEnabled(hasFutureEndTime && hasLongEnoughReason);
  ui->dynamicLabel->setArgs({endTime.toString("HH:mm")});

  if (!hasFutureEndTime && !hasLongEnoughReason) {
    ui->reasonHelpLabel->setStyleSheet(kErrorTextStyle);
    ui->reasonHelpLabel->setText(
        tr("Choose a future end time and enter at least %1 characters.")
            .arg(kMinReasonLength));
    return;
  }

  if (!hasFutureEndTime) {
    ui->reasonHelpLabel->setStyleSheet(kErrorTextStyle);
    ui->reasonHelpLabel->setText(tr("Choose an end time later than now."));
    return;
  }

  if (reason.isEmpty()) {
    ui->reasonHelpLabel->setStyleSheet(kHelperTextStyle);
    ui->reasonHelpLabel->setText(
        tr("Enter at least %1 characters.").arg(kMinReasonLength));
    return;
  }

  if (!hasLongEnoughReason) {
    ui->reasonHelpLabel->setStyleSheet(kErrorTextStyle);
    ui->reasonHelpLabel->setText(tr("Add a little more detail."));
    return;
  }

  ui->reasonHelpLabel->setStyleSheet(kHelperTextStyle);
  ui->reasonHelpLabel->setText(" ");
}
