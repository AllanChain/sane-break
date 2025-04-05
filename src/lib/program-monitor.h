// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QRegularExpression>
#include <QStringList>
#include <QTimer>
#include <QtContainerFwd>

class RunningProgramsMonitor : public QObject {
  Q_OBJECT

 public:
  RunningProgramsMonitor(QObject *parent = nullptr);
  void startMonitoring();
  void stopMonitoring();
  void setPrograms(const QStringList &programs);

 signals:
  void programStarted();
  void programStopped();

 private slots:
  void tick();

 private:
  const QRegularExpression validProgramFilter = QRegularExpression(".");
  QTimer *monitorTimer;
  QStringList programsToMonitor;
  bool previouslySeen = false;
};
