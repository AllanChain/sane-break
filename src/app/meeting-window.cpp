// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/meeting-window.h"

#include <QButtonGroup>
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

MeetingWindow::MeetingWindow(SanePreferences* preferences, BreakDatabase* db,
                             QWidget* parent)
    : QWidget(parent), ui(new Ui::MeetingUI), preferences(preferences), db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  connect(ui->endTime, &QTimeEdit::timeChanged, this, &MeetingWindow::onInputUpdate);
  connect(ui->reasonEdit, &QLineEdit::textChanged, this, &MeetingWindow::onInputUpdate);
  connect(ui->cancelButton, &QPushButton::pressed, this, &MeetingWindow::close);
  connect(ui->confirmButton, &QPushButton::pressed, this, [this]() {
    close();
    emit meetingRequested(ui->endTime->time(), ui->reasonEdit->text());
  });
  QTime now = QTime::currentTime();
  ui->endTime->setTime(QTime(now.hour(), now.minute()));
}

void MeetingWindow::onInputUpdate() {
  QTime endTime = ui->endTime->time();
  QTime currentTime = QTime::currentTime();
  ui->confirmButton->setEnabled(endTime > currentTime &&
                                ui->reasonEdit->text().length() > 5);
  ui->dynamicLabel->setArgs({endTime.toString("HH:mm")});
}
