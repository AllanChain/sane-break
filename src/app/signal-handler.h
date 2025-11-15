// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

// See https://doc.qt.io/qt-6/unix-signals.html

#pragma once

#include <QObject>
#include <QSocketNotifier>

class SignalHandler : public QObject {
  Q_OBJECT
 public:
  SignalHandler(QObject* parent = 0);
  ~SignalHandler() = default;
  static int setup();
  static void signalHandler(int unused);  // UNIX signal handler
  void handleSignal();                    // Qt signal handler

 signals:
  void exitRequested();

 private:
  static int fd[2];
  QSocketNotifier* notifier;
};
