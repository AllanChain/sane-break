// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonObject>
#include <QMap>
#include <QString>

void runHookScript(const QString& command, const QString& event,
                   const QMap<QString, QString>& env = {});

QJsonObject runCustomizeHookScript(const QString& command, const QString& event,
                                   const QJsonObject& input,
                                   const QMap<QString, QString>& env = {},
                                   int timeoutMs = 1000);
