// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QStringList>

#include "core/cli.h"

class AbstractApp;

class QLocalServer;
class QLocalSocket;

QString externalCommandServerName();
CliCommandResult sendCliCommand(const QStringList& arguments, int timeoutMs = 1500);

class CliCommandServer : public QObject {
  Q_OBJECT
 public:
  explicit CliCommandServer(AbstractApp* app, QObject* parent = nullptr);

  bool start(QString* errorMessage = nullptr);

 private:
  void handleNewConnection();
  void handleConnection(QLocalSocket* socket);

  AbstractApp* m_app;
  QLocalServer* m_server;
};
