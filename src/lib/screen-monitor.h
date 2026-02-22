// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QScreen>
#include <QStringList>

class ScreenMonitor : public QObject {
  Q_OBJECT

 public:
  ScreenMonitor(QObject* parent = nullptr);
  void startMonitoring();
  void setKnownMonitors(const QStringList& monitors);
  static QString screenId(const QScreen* screen);
  static QStringList connectedScreenIds();

 signals:
  void unknownMonitorConnected();
  void allMonitorsKnown();

 private:
  void check();
  QStringList m_knownMonitors;
  bool m_previouslyHadUnknown = false;
};
