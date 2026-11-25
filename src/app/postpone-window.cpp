// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/postpone-window.h"

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <Qt>

#include "core/app-data.h"
#include "core/db.h"
#include "core/preferences.h"
#include "ui_postpone-window.h"

PostponeWindow::PostponeWindow(SanePreferences* preferences, BreakDatabase* db,
                               QWidget* parent)
    : QDialog(parent), ui(new Ui::PostponeUI), preferences(preferences), db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  ui->postponeMinutes->setMaximum(PostponeRules::maxPostponeMinutes(preferences));
  onMinutesUpdate(0);
  connect(ui->postponeMinutes, &QSpinBox::valueChanged, this,
          &PostponeWindow::onMinutesUpdate);
  connect(ui->postponeButton, &QPushButton::pressed, this, [this]() {
    emit postponeRequested(ui->postponeMinutes->value() * 60);
    accept();
  });
}

void PostponeWindow::setMinutes(int minutes) { ui->postponeMinutes->setValue(minutes); }

void PostponeWindow::onMinutesUpdate(int minutes) {
  ui->postponeButton->setEnabled(minutes > 0);
  ui->totalMinutesLabel->setArgs({minutes});
  PostponeImpact impact = PostponeRules::calculateImpact(preferences, minutes * 60);
  ui->nextSessionLabel->setArgs({impact.nextSessionShrinkSeconds / 60});
  ui->breakExtensionLabel->setArgs({impact.breakExtensionSeconds});
}
