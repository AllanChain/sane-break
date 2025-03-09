// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "auto-start.h"

#include <QCoreApplication>
#include <QSettings>

void AutoStart::setEnabled(bool enabled) {
  QSettings settings(
      "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
      QSettings::NativeFormat);
  if (enabled) {
    settings.setValue(QCoreApplication::applicationName(),
                      QCoreApplication::applicationFilePath());
  } else {
    settings.remove(QCoreApplication::applicationName());
  }
  switch (settings.status()) {
    case QSettings::NoError:
      emit operationResult(true);
      return;
    case QSettings::AccessError:
      emit operationResult(false, tr("Permission denied"));
      return;
    default:
      emit operationResult(false, tr("Unknown error"));
      return;
  }
}

bool AutoStart::isEnabled() {
  QSettings settings(
      "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
      QSettings::NativeFormat);
  return settings.contains(QCoreApplication::applicationName());
}
