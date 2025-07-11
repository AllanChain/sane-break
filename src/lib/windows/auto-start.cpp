// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "auto-start.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

#include "core/preferences.h"

void AutoStart::setEnabled(bool enabled) {
  QSettings settings(
      "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
      QSettings::NativeFormat);
  if (enabled) {
    settings.setValue(
        QCoreApplication::applicationName(),
        QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
  } else {
    settings.remove(QCoreApplication::applicationName());
  }
  switch (settings.status()) {
    case QSettings::NoError:
      emit operationResult(true);
      return;
    case QSettings::AccessError:
      //: Error message when requesting autostart got permission denied
      emit operationResult(false, tr("Permission denied"));
      return;
    default:
      emit operationResult(false, tr("Unknown error"));
      return;
  }
}

bool AutoStart::isEnabled(SanePreferences*) {
  QSettings settings(
      "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
      QSettings::NativeFormat);
  return settings.contains(QCoreApplication::applicationName());
}
