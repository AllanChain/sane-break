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
#include <QTime>
#include <QTimer>

#include "core/db.h"
#include "core/preferences.h"

namespace Ui {
class MeetingUI;
}

class MeetingWindow : public QDialog {
  Q_OBJECT

 public:
  MeetingWindow(SanePreferences* preferences, BreakDatabase* db,
                QWidget* parent = nullptr);
  ~MeetingWindow() = default;

 signals:
  void meetingRequested(QTime endTime, QString reason);

 private:
  Ui::MeetingUI* ui;
  SanePreferences* preferences;
  BreakDatabase* db;

  void onInputUpdate();
};
