// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hook-runner.h"

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

#include "config.h"

void runHookScript(const QString& command, const QString& event,
                   const QMap<QString, QString>& env) {
#if defined(LINUX_DIST_FLATPAK) || defined(SANE_APP_STORE)
  Q_UNUSED(command);
  Q_UNUSED(event);
  Q_UNUSED(env);
  return;
#else
  QString trimmed = command.trimmed();
  if (trimmed.isEmpty()) return;

  QStringList parts = QProcess::splitCommand(trimmed);
  if (parts.isEmpty()) return;

  QString program = parts.takeFirst();
  parts.append(event);

  auto* process = new QProcess();
  process->setProgram(program);
  process->setArguments(parts);

  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  procEnv.insert("SANE_BREAK_EVENT", event);
  for (auto it = env.cbegin(); it != env.cend(); ++it) {
    procEnv.insert(it.key(), it.value());
  }
  process->setProcessEnvironment(procEnv);
  process->setProcessChannelMode(QProcess::ForwardedErrorChannel);
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
