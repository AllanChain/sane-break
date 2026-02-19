// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

// See https://doc.qt.io/qt-6/unix-signals.html

#include "app/unix/signal-handler.h"

#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <QObject>
#include <QSocketNotifier>

int SignalHandler::fd[2] = {0, 0};
SignalHandler::SignalHandler(QObject* parent) : QObject(parent) {
  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fd)) {
    qFatal("Couldn't create TERM socketpair");
  }
  notifier = new QSocketNotifier(fd[1], QSocketNotifier::Read, this);
  connect(notifier, &QSocketNotifier::activated, this, &SignalHandler::handleSignal);
}

void SignalHandler::signalHandler(int) {
  char a = 1;
  ::write(fd[0], &a, sizeof(a));
}

int SignalHandler::setup() {
  struct sigaction term;

  term.sa_handler = SignalHandler::signalHandler;
  sigemptyset(&term.sa_mask);
  term.sa_flags = 0;
  term.sa_flags |= SA_RESTART;

  if (sigaction(SIGTERM, &term, 0)) return 2;
  if (sigaction(SIGINT, &term, 0)) return 3;

  return 0;
}

void SignalHandler::handleSignal() {
  notifier->setEnabled(false);
  char tmp;
  ::read(fd[1], &tmp, sizeof(tmp));

  emit exitRequested();

  notifier->setEnabled(true);
}
