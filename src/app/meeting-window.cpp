// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/meeting-window.h"

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QString>
#include <QTime>
#include <QTimeEdit>
#include <QTimeZone>
#include <QTimer>
#include <QWidget>
#include <Qt>
#include <optional>

#include "core/db.h"
#include "core/preferences.h"
#include "ui_meeting-window.h"

namespace {
constexpr int kMinReasonLength = 6;
constexpr auto kHelperTextStyle = "color: #6b7280;";
constexpr auto kErrorTextStyle = "color: #ef4444;";
}  // namespace

namespace MeetingTime {

std::optional<QDateTime> resolveEndDateTime(const QDateTime& now,
                                            const QTime& endTime) {
  if (!endTime.isValid()) return std::nullopt;

  QDateTime todayCandidate(now.date(), endTime, now.timeZone());
  if (todayCandidate > now) return todayCandidate;

  QDateTime tomorrowCandidate = todayCandidate.addDays(1);
  if (now.secsTo(tomorrowCandidate) <= kMidnightRolloverThresholdSeconds) {
    return tomorrowCandidate;
  }

  return std::nullopt;
}

}  // namespace MeetingTime

MeetingWindow::MeetingWindow(SanePreferences* preferences, BreakDatabase* db,
                             QWidget* parent)
    : QDialog(parent), ui(new Ui::MeetingUI), preferences(preferences), db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  QLocale timeLocale = QLocale::system();
  ui->endTime->setLocale(timeLocale);
  ui->endTime->setDisplayFormat(timeLocale.timeFormat(QLocale::ShortFormat));
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
  QDateTime now = QDateTime::currentDateTime();
  QString reason = ui->reasonEdit->text().trimmed();
  auto resolvedEndTime = MeetingTime::resolveEndDateTime(now, endTime);
  bool isValidEndTime = resolvedEndTime.has_value();
  int remainingChars = kMinReasonLength - reason.length();
  bool hasLongEnoughReason = remainingChars <= 0;

  ui->confirmButton->setEnabled(isValidEndTime && hasLongEnoughReason);
  QString endTimeLabel = QLocale::system().toString(endTime, QLocale::ShortFormat);
  if (resolvedEndTime && resolvedEndTime->date() > now.date()) {
    endTimeLabel = tr("tomorrow at %1").arg(endTimeLabel);
  }
  ui->dynamicLabel->setArgs({endTimeLabel});

  if (!isValidEndTime && !hasLongEnoughReason) {
    ui->reasonHelpLabel->setStyleSheet(kErrorTextStyle);
    ui->reasonHelpLabel->setText(
        tr("Choose a future end time and enter at least %1 characters.")
            .arg(kMinReasonLength));
    return;
  }

  if (!isValidEndTime) {
    ui->reasonHelpLabel->setStyleSheet(kErrorTextStyle);
    QDateTime maybeTomorrowCutoff =
        now.addSecs(MeetingTime::kMidnightRolloverThresholdSeconds);
    if (maybeTomorrowCutoff.date() == now.date()) {
      ui->reasonHelpLabel->setText(tr("Choose an end time later than now."));
    } else {
      ui->reasonHelpLabel->setText(
          tr("Choose a later time today, or a tomorrow time before %1.")
              .arg(QLocale::system().toString(maybeTomorrowCutoff.time(),
                                              QLocale::ShortFormat)));
    }
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
