// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QStringList>

class AbstractApp;

struct CliCommandResult {
  bool ok = false;
  QString message;
};

bool shouldLaunchGuiForArguments(const QStringList& arguments);
CliCommandResult executeCliCommand(AbstractApp* app, const QStringList& arguments);
