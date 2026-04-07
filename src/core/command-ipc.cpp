// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/command-ipc.h"

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDir>
#include <QIODeviceBase>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <optional>

#include "core/cli.h"

namespace {

constexpr int kProtocolVersion = 1;

class CommandIpcTranslation {
  Q_DECLARE_TR_FUNCTIONS(CommandIpc)
};

QJsonArray argumentsToJson(const QStringList& arguments) {
  QJsonArray array;
  for (const QString& argument : arguments) array.append(argument);
  return array;
}

QStringList argumentsFromJson(const QJsonArray& array) {
  QStringList arguments;
  for (const QJsonValue& argument : array) arguments.append(argument.toString());
  return arguments;
}

QJsonObject resultToJson(const CliCommandResult& result) {
  return {
      {"ok", result.ok},
      {"message", result.message},
  };
}

CliCommandResult resultFromJson(const QJsonObject& object) {
  return {
      .ok = object["ok"].toBool(),
      .message = object["message"].toString(),
  };
}

std::optional<QJsonObject> readJsonObject(QLocalSocket* socket, int timeoutMs,
                                          QString* errorMessage) {
  if (!socket->waitForReadyRead(timeoutMs)) {
    if (errorMessage)
      *errorMessage =
          CommandIpcTranslation::tr("Timed out while waiting for a response.");
    return std::nullopt;
  }

  QByteArray payload = socket->readAll();
  while (socket->waitForReadyRead(10)) payload += socket->readAll();

  QJsonParseError parseError;
  QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
  if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
    if (errorMessage)
      *errorMessage =
          CommandIpcTranslation::tr("Received an invalid response from Sane Break.");
    return std::nullopt;
  }

  return document.object();
}

void writeJsonMessage(QLocalSocket* socket, const CliCommandResult& result) {
  QJsonDocument response(resultToJson(result));
  socket->write(response.toJson(QJsonDocument::Compact));
  socket->waitForBytesWritten(1000);
  socket->disconnectFromServer();
}

QJsonObject commandToJson(const QStringList& arguments) {
  return {
      {"version", kProtocolVersion},
      {"arguments", argumentsToJson(arguments)},
  };
}

}  // namespace

QString externalCommandServerName() {
#ifdef Q_OS_WINDOWS
  // Packaged Windows apps must use the LOCAL named-pipe namespace.
#ifndef NDEBUG
  return QStringLiteral("\\\\.\\pipe\\LOCAL\\SaneBreak.CommandServer.Debug");
#else
  return QStringLiteral("\\\\.\\pipe\\LOCAL\\SaneBreak.CommandServer");
#endif
#else
  QDir runtimeDir(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation));
#ifndef NDEBUG
  return runtimeDir.filePath(QStringLiteral("sane-break-debug.sock"));
#else
  return runtimeDir.filePath(QStringLiteral("sane-break.sock"));
#endif
#endif
}

CliCommandResult sendCliCommand(const QStringList& arguments, int timeoutMs) {
  QLocalSocket socket;
  socket.connectToServer(externalCommandServerName(), QIODeviceBase::ReadWrite);
  if (!socket.waitForConnected(timeoutMs)) {
    return {
        .ok = false,
        .message =
            CommandIpcTranslation::tr("No running Sane Break instance is available."),
    };
  }

  QJsonDocument request(commandToJson(arguments));
  socket.write(request.toJson(QJsonDocument::Compact));
  if (!socket.waitForBytesWritten(timeoutMs)) {
    return {
        .ok = false,
        .message = CommandIpcTranslation::tr("Timed out while sending the command."),
    };
  }

  QString errorMessage;
  std::optional<QJsonObject> response =
      readJsonObject(&socket, timeoutMs, &errorMessage);
  if (!response) {
    return {.ok = false, .message = errorMessage};
  }
  return resultFromJson(*response);
}

CliCommandServer::CliCommandServer(AbstractApp* app, QObject* parent)
    : QObject(parent), m_app(app), m_server(new QLocalServer(this)) {}

bool CliCommandServer::start(QString* errorMessage) {
  connect(m_server, &QLocalServer::newConnection, this,
          &CliCommandServer::handleNewConnection);

  if (m_server->listen(externalCommandServerName())) return true;

  if (m_server->serverError() == QAbstractSocket::AddressInUseError &&
      QLocalServer::removeServer(externalCommandServerName()) &&
      m_server->listen(externalCommandServerName())) {
    return true;
  }

  if (errorMessage) *errorMessage = m_server->errorString();
  return false;
}

void CliCommandServer::handleNewConnection() {
  while (QLocalSocket* socket = m_server->nextPendingConnection()) {
    handleConnection(socket);
  }
}

void CliCommandServer::handleConnection(QLocalSocket* socket) {
  connect(socket, &QLocalSocket::disconnected, socket, &QObject::deleteLater);

  QString errorMessage;
  std::optional<QJsonObject> request = readJsonObject(socket, 1000, &errorMessage);
  if (!request) {
    writeJsonMessage(socket, {.ok = false, .message = errorMessage});
    return;
  }

  QJsonObject object = *request;
  if (object["version"].toInt() != kProtocolVersion) {
    writeJsonMessage(
        socket, {.ok = false,
                 .message = CommandIpcTranslation::tr("Unsupported command version.")});
    return;
  }

  writeJsonMessage(
      socket,
      executeCliCommand(m_app, argumentsFromJson(object["arguments"].toArray())));
}
