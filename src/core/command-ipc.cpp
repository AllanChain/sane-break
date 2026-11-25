// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/command-ipc.h"

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
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
#include <QTimer>
#include <optional>

#include "core/cli.h"

namespace {

constexpr int kProtocolVersion = 1;
constexpr int kServerTimeoutMs = 1000;
constexpr qsizetype kMaxMessageBytes = 64 * 1024;

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

std::optional<QJsonObject> parseJsonObject(const QByteArray& payload,
                                           QString* errorMessage) {
  QJsonParseError parseError;
  QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
  if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
    if (errorMessage) {
      *errorMessage =
          CommandIpcTranslation::tr("Received an invalid response from Sane Break.");
    }
    return std::nullopt;
  }
  return document.object();
}

std::optional<QJsonObject> readClientJsonObject(QLocalSocket* socket, int timeoutMs,
                                                QString* errorMessage) {
  QElapsedTimer timer;
  timer.start();
  QByteArray payload;
  bool timedOut = false;

  while (!payload.contains('\n') && payload.size() <= kMaxMessageBytes) {
    if (socket->bytesAvailable() > 0) {
      payload += socket->read(kMaxMessageBytes + 1 - payload.size());
      continue;
    }
    if (socket->state() != QLocalSocket::ConnectedState) break;
    int remainingMs = timeoutMs - static_cast<int>(timer.elapsed());
    if (remainingMs <= 0) {
      timedOut = true;
      break;
    }
    if (!socket->waitForReadyRead(remainingMs)) {
      if (socket->error() == QLocalSocket::SocketTimeoutError) timedOut = true;
      break;
    }
  }

  if (payload.size() > kMaxMessageBytes) {
    if (errorMessage) {
      *errorMessage =
          CommandIpcTranslation::tr("Received an invalid response from Sane Break.");
    }
    return std::nullopt;
  }

  qsizetype newlineIndex = payload.indexOf('\n');
  if (newlineIndex >= 0) payload.truncate(newlineIndex);

  if (timedOut && newlineIndex < 0) {
    if (errorMessage) {
      *errorMessage =
          CommandIpcTranslation::tr("Timed out while waiting for a response.");
    }
    return std::nullopt;
  }

  return parseJsonObject(payload, errorMessage);
}

QJsonObject commandToJson(const QStringList& arguments) {
  return {
      {"version", kProtocolVersion},
      {"arguments", argumentsToJson(arguments)},
  };
}

class PendingCommandConnection : public QObject {
 public:
  PendingCommandConnection(QLocalSocket* socket, AbstractApp* app, QObject* parent)
      : QObject(parent), m_socket(socket), m_app(app), m_timeout(new QTimer(this)) {
    m_socket->setParent(this);
    m_timeout->setSingleShot(true);

    connect(m_socket, &QLocalSocket::readyRead, this,
            &PendingCommandConnection::onReadyRead);
    connect(m_socket, &QLocalSocket::bytesWritten, this,
            &PendingCommandConnection::onBytesWritten);
    connect(m_socket, &QLocalSocket::disconnected, this,
            &PendingCommandConnection::finish);
    connect(m_socket, &QLocalSocket::errorOccurred, this,
            [this](QLocalSocket::LocalSocketError) {
              if (m_socket->state() == QLocalSocket::UnconnectedState) finish();
            });
    connect(m_timeout, &QTimer::timeout, this, &PendingCommandConnection::onTimeout);

    m_timeout->start(kServerTimeoutMs);
    if (m_socket->bytesAvailable() > 0) onReadyRead();
    if (m_state == State::Reading &&
        m_socket->state() == QLocalSocket::UnconnectedState) {
      finish();
    }
  }

 private:
  enum class State { Reading, Writing, Closing };

  void onReadyRead() {
    if (m_state != State::Reading) return;

    m_buffer += m_socket->read(kMaxMessageBytes + 1 - m_buffer.size());
    if (m_buffer.size() > kMaxMessageBytes) {
      // Reuse existing CommandIpc translation strings to avoid churning .ts catalogs.
      sendResponse({.ok = false,
                    .message = CommandIpcTranslation::tr(
                        "Received an invalid response from Sane Break.")});
      return;
    }

    qsizetype newlineIndex = m_buffer.indexOf('\n');
    if (newlineIndex >= 0) {
      handlePayload(m_buffer.left(newlineIndex));
      return;
    }

    // Fallback for unframed clients: dispatch as soon as a complete JSON object
    // arrives.
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(m_buffer, &parseError);
    if (parseError.error == QJsonParseError::NoError && document.isObject()) {
      handleRequestObject(document.object());
    }
  }

  void handlePayload(const QByteArray& payload) {
    QString errorMessage;
    std::optional<QJsonObject> request = parseJsonObject(payload, &errorMessage);
    if (!request) {
      sendResponse({.ok = false, .message = errorMessage});
      return;
    }
    handleRequestObject(*request);
  }

  void handleRequestObject(const QJsonObject& object) {
    if (object["version"].toInt() != kProtocolVersion) {
      sendResponse(
          {.ok = false,
           .message = CommandIpcTranslation::tr("Unsupported command version.")});
      return;
    }

    sendResponse(
        executeCliCommand(m_app, argumentsFromJson(object["arguments"].toArray())));
  }

  void sendResponse(const CliCommandResult& result) {
    if (m_state != State::Reading) return;
    if (m_socket->state() != QLocalSocket::ConnectedState) {
      finish();
      return;
    }
    m_state = State::Writing;

    QByteArray response =
        QJsonDocument(resultToJson(result)).toJson(QJsonDocument::Compact);
    response.append('\n');

    m_timeout->start(kServerTimeoutMs);
    if (m_socket->write(response) < 0) {
      finish();
      return;
    }
    if (m_state == State::Writing && m_socket->bytesToWrite() == 0) {
      m_socket->disconnectFromServer();
    }
    if (m_state != State::Closing &&
        m_socket->state() == QLocalSocket::UnconnectedState) {
      finish();
    }
  }

  void onBytesWritten(qint64) {
    if (m_state == State::Writing && m_socket->bytesToWrite() == 0) {
      m_socket->disconnectFromServer();
    }
  }

  void onTimeout() {
    if (m_state == State::Reading) {
      sendResponse({.ok = false,
                    .message = CommandIpcTranslation::tr(
                        "Timed out while waiting for a response.")});
      return;
    }
    m_socket->abort();
    finish();
  }

  void finish() {
    if (m_state == State::Closing) return;
    m_state = State::Closing;
    m_timeout->stop();
    deleteLater();
  }

  QLocalSocket* m_socket;
  AbstractApp* m_app;
  QTimer* m_timeout;
  QByteArray m_buffer;
  State m_state = State::Reading;
};

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

  QByteArray request =
      QJsonDocument(commandToJson(arguments)).toJson(QJsonDocument::Compact);
  request.append('\n');
  socket.write(request);
  if (!socket.waitForBytesWritten(timeoutMs)) {
    return {
        .ok = false,
        .message = CommandIpcTranslation::tr("Timed out while sending the command."),
    };
  }

  QString errorMessage;
  std::optional<QJsonObject> response =
      readClientJsonObject(&socket, timeoutMs, &errorMessage);
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
  new PendingCommandConnection(socket, m_app, this);
}
