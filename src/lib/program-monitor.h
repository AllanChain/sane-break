// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RUNNING_PROGRAMS_MONITOR_H
#define RUNNING_PROGRAMS_MONITOR_H

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

#endif  // RUNNING_PROGRAMS_MONITOR_H
