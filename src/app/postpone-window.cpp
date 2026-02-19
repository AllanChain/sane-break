// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/postpone-window.h"

#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <QWidget>
#include <Qt>

#include "core/db.h"
#include "core/preferences.h"
#include "ui_postpone-window.h"

PostponeWindow::PostponeWindow(SanePreferences* preferences, BreakDatabase* db,
                               QWidget* parent)
    : QWidget(parent), ui(new Ui::PostponeUI), preferences(preferences), db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  ui->postponeMinutes->setMaximum(preferences->smallEvery->get() *
                                  preferences->postponeMaxMinutePercent->get() / 60 /
                                  100);
  onMinutesUpdate(0);
  connect(ui->postponeMinutes, &QSpinBox::valueChanged, this,
          &PostponeWindow::onMinutesUpdate);
  connect(ui->cancelButton, &QPushButton::pressed, this, &PostponeWindow::close);
  connect(ui->postponeButton, &QPushButton::pressed, this, [this]() {
    close();
    emit postponeRequested(ui->postponeMinutes->value() * 60);
  });
}

void PostponeWindow::onMinutesUpdate(int minutes) {
  ui->postponeButton->setEnabled(minutes > 0);
  ui->totalMinutesLabel->setArgs({minutes});
  int shrinkNextMinutes = minutes * preferences->postponeShrinkNextPercent->get() / 100;
  ui->nextSessionLabel->setArgs({shrinkNextMinutes});
  int breakForReference = preferences->bigBreakEnabled->get()
                              ? preferences->bigFor->get()
                              : preferences->smallFor->get();
  int breakExtensionSeconds = preferences->postponeExtendBreakPercent->get() * minutes *
                              60 * breakForReference /
                              preferences->smallEvery->get() / 100;
  ui->breakExtensionLabel->setArgs({breakExtensionSeconds});
}
