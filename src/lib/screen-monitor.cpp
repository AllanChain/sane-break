// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "screen-monitor.h"

#include <QGuiApplication>
#include <QObject>
#include <QScreen>
#include <QString>
#include <QStringList>

ScreenMonitor::ScreenMonitor(QObject* parent) : QObject(parent) {}

void ScreenMonitor::startMonitoring() {
  connect(qApp, &QGuiApplication::screenAdded, this, &ScreenMonitor::check);
  connect(qApp, &QGuiApplication::screenRemoved, this, &ScreenMonitor::check);
  check();
}

void ScreenMonitor::setKnownMonitors(const QStringList& monitors) {
  m_knownMonitors = monitors;
  check();
}

QString ScreenMonitor::screenId(const QScreen* screen) {
  QString manufacturer = screen->manufacturer();
  QString model = screen->model();
  QString serial = screen->serialNumber();
  if (manufacturer.isEmpty() && model.isEmpty() && serial.isEmpty()) {
    return screen->name();
  }
  return manufacturer + ":" + model + ":" + serial;
}

QStringList ScreenMonitor::connectedScreenIds() {
  QStringList ids;
  for (const QScreen* screen : QGuiApplication::screens()) {
    ids.append(screenId(screen));
  }
  return ids;
}

void ScreenMonitor::check() {
  // When never configured (empty known list), treat all screens as known
  if (m_knownMonitors.isEmpty()) {
    if (m_previouslyHadUnknown) {
      m_previouslyHadUnknown = false;
      emit allMonitorsKnown();
    }
    return;
  }

  bool hasUnknown = false;
  for (const QString& id : connectedScreenIds()) {
    if (!m_knownMonitors.contains(id)) {
      hasUnknown = true;
      break;
    }
  }

  if (!m_previouslyHadUnknown && hasUnknown) emit unknownMonitorConnected();
  if (m_previouslyHadUnknown && !hasUnknown) emit allMonitorsKnown();
  m_previouslyHadUnknown = hasUnknown;
}
