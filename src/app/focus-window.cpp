// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/focus-window.h"

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>
#include <Qt>

#include "core/preferences.h"
#include "ui_focus-window.h"

namespace {
constexpr int kMinReasonLength = 6;
constexpr auto kHelperTextStyle = "color: #6b7280;";
constexpr auto kErrorTextStyle = "color: #ef4444;";
}  // namespace

FocusWindow::FocusWindow(SanePreferences* preferences, QWidget* parent)
    : QDialog(parent), ui(new Ui::FocusUI), preferences(preferences) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  updateTextFromPreferences();

  connect(preferences->focusSmallEvery, &Setting<int>::changed, this,
          &FocusWindow::updateTextFromPreferences);
  connect(preferences->focusSmallFor, &Setting<int>::changed, this,
          &FocusWindow::updateTextFromPreferences);
  connect(preferences->focusBigBreakEnabled, &Setting<bool>::changed, this,
          &FocusWindow::updateTextFromPreferences);
  connect(preferences->focusBigFor, &Setting<int>::changed, this,
          &FocusWindow::updateTextFromPreferences);
  connect(preferences->focusBigAfter, &Setting<int>::changed, this,
          &FocusWindow::updateTextFromPreferences);
  connect(ui->reasonEdit, &QLineEdit::textChanged, this, &FocusWindow::onInputUpdate);
  connect(this, &QDialog::accepted, this, [this]() {
    int focusSmallEvery = this->preferences->focusSmallEvery->get();
    int totalCycles = ui->spinBox->value() * 60 / focusSmallEvery;
    emit focusRequested(totalCycles, ui->reasonEdit->text().trimmed());
  });
  onInputUpdate();
}

void FocusWindow::updateTextFromPreferences() {
  int focusSmallEvery = preferences->focusSmallEvery->get();
  int stepMinutes = focusSmallEvery / 60;
  ui->spinBox->setMinimum(stepMinutes);
  ui->spinBox->setSingleStep(stepMinutes);
  ui->spinBox->setValue(stepMinutes * 2);

  int focusSmallFor = preferences->focusSmallFor->get();
  ui->smallScheduleLabel->setArgs({focusSmallFor, focusSmallEvery / 60});

  if (preferences->focusBigBreakEnabled->get()) {
    int focusBigFor = preferences->focusBigFor->get();
    int focusBigAfter = preferences->focusBigAfter->get();
    ui->bigScheduleLabel->setVisible(true);
    ui->bigScheduleLabel->setArgs({focusBigFor, focusBigAfter});
  } else {
    ui->bigScheduleLabel->setVisible(false);
  }

  ui->confirmButton->setArgs({preferences->focusSmallFor->get()});
}

void FocusWindow::onInputUpdate() {
  QString reason = ui->reasonEdit->text().trimmed();
  int remainingChars = kMinReasonLength - reason.length();
  bool hasLongEnoughReason = remainingChars <= 0;

  ui->confirmButton->setEnabled(hasLongEnoughReason);

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
