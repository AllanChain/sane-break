// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "config.h"
#include "core/preferences.h"

class AutoStart : public QObject {
  Q_OBJECT

 public:
  AutoStart(QObject* parent = nullptr) : QObject(parent) {};
  void setEnabled(bool enabled);
  bool isEnabled(SanePreferences* preferences);
#ifdef LINUX_DIST_FLATPAK
 public slots:
  void flatpakCallback(uint response, const QVariantMap& results);
#endif

 signals:
  void operationResult(bool succeeded, QString error = QString());
};
