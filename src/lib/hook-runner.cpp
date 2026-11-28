// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hook-runner.h"

#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

#include "config.h"

namespace {
constexpr int kMaxCustomizeOutputBytes = 64 * 1024;

#if !defined(LINUX_DIST_FLATPAK) && !defined(SANE_APP_STORE)
bool configureHookProcess(QProcess* process, const QString& command,
                          const QString& event, const QMap<QString, QString>& env,
                          QString* programOut) {
  QString trimmed = command.trimmed();
  if (trimmed.isEmpty()) return false;

  QStringList parts = QProcess::splitCommand(trimmed);
  if (parts.isEmpty()) return false;

  *programOut = parts.takeFirst();
  parts.append(event);

  process->setProgram(*programOut);
  process->setArguments(parts);

  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  procEnv.insert("SANE_BREAK_EVENT", event);
  for (auto it = env.cbegin(); it != env.cend(); ++it) {
    procEnv.insert(it.key(), it.value());
  }
  process->setProcessEnvironment(procEnv);
  process->setProcessChannelMode(QProcess::ForwardedErrorChannel);
  return true;
}
#endif
}  // namespace

void runHookScript(const QString& command, const QString& event,
                   const QMap<QString, QString>& env) {
#if defined(LINUX_DIST_FLATPAK) || defined(SANE_APP_STORE)
  Q_UNUSED(command);
  Q_UNUSED(event);
  Q_UNUSED(env);
  return;
#else
  auto* process = new QProcess();
  QString program;
  if (!configureHookProcess(process, command, event, env, &program)) {
    delete process;
    return;
  }
  process->setStandardOutputFile(QProcess::nullDevice());

  QObject::connect(process, &QProcess::finished, process, &QObject::deleteLater);
  QObject::connect(process, &QProcess::errorOccurred, process,
                   [process, program](QProcess::ProcessError error) {
                     qWarning() << "Hook script failed:" << program << error
                                << process->errorString();
                     if (process->state() == QProcess::NotRunning) {
                       process->deleteLater();
                     }
                   });
  process->start();
#endif
}

QJsonObject runCustomizeHookScript(const QString& command, const QString& event,
                                   const QJsonObject& input,
                                   const QMap<QString, QString>& env, int timeoutMs) {
#if defined(LINUX_DIST_FLATPAK) || defined(SANE_APP_STORE)
  Q_UNUSED(command);
  Q_UNUSED(event);
  Q_UNUSED(input);
  Q_UNUSED(env);
  Q_UNUSED(timeoutMs);
  return {};
#else
  QProcess process;
  QString program;
  if (!configureHookProcess(&process, command, event, env, &program)) return {};

  process.start();
  process.write(QJsonDocument(input).toJson(QJsonDocument::Compact));
  process.closeWriteChannel();

  if (!process.waitForFinished(timeoutMs)) {
    QProcess::ProcessError err = process.error();
    QString errStr = process.errorString();
    if (process.state() != QProcess::NotRunning) {
      process.kill();
      process.waitForFinished(100);
    }
    qWarning() << "Customize hook script failed or timed out:" << program << err
               << errStr;
    return {};
  }

  if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
    qWarning() << "Customize hook script exited with error:" << program
               << process.exitStatus() << process.exitCode();
    return {};
  }

  QByteArray output = process.readAllStandardOutput();
  if (output.size() > kMaxCustomizeOutputBytes) {
    qWarning() << "Customize hook script output exceeded 64 KB:" << program
               << output.size();
    return {};
  }

  QByteArray trimmedOutput = output.trimmed();
  if (trimmedOutput.isEmpty()) return {};

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(trimmedOutput, &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    qWarning() << "Customize hook script returned invalid JSON object:" << program
               << parseError.errorString();
    return {};
  }
  return doc.object();
#endif
}
