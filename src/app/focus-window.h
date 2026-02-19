// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include "core/preferences.h"

namespace Ui {
class FocusUI;
}

class FocusWindow : public QDialog {
  Q_OBJECT

 public:
  FocusWindow(SanePreferences* preferences, QWidget* parent = nullptr);
  ~FocusWindow() = default;

 signals:
  void focusRequested(int totalCycles, QString reason);

 private:
  Ui::FocusUI* ui;
  SanePreferences* preferences;

  void onInputUpdate();
  void updateTextFromPreferences();
};
