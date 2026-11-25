// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "core/preferences.h"

namespace Ui {
class PostponeUI;
}

class PostponeWindow : public QDialog {
  Q_OBJECT

 public:
  explicit PostponeWindow(SanePreferences* preferences, QWidget* parent = nullptr);
  ~PostponeWindow() = default;

  void setMinutes(int minutes);

 signals:
  void postponeRequested(int seconds);

 private:
  Ui::PostponeUI* ui;
  SanePreferences* preferences;

  void onMinutesUpdate(int minutes);
};
