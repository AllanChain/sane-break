// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_AUTOSTART_H
#define SANE_BREAK_AUTOSTART_H

#include <QObject>

class AutoStart : public QObject {
  Q_OBJECT

 public:
  AutoStart(QObject *parent = nullptr) : QObject(parent) {};
  void setEnabled(bool enabled);
  bool isEnabled();

 signals:
  void operationResult(bool succeeded, QString error = QString());
};

#endif
