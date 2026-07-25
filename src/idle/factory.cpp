// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "factory.h"

#include <qglobal.h>

#include <QObject>
#include <QTimer>

#include "config.h"
#include "core/idle-time.h"

#ifdef Q_OS_LINUX
#include "linux/factory.h"
#elif defined Q_OS_MACOS
#include "macos-idle.h"
#include "macos-inhibitor.h"
#include "read-based-idle.h"
#elif defined Q_OS_WIN
#include "read-based-idle.h"
#include "windows-idle.h"
#include "windows-inhibitor.h"
#endif

SystemIdleTime* createIdleTimer(QObject* parent) {
#ifdef Q_OS_LINUX
  return createLinuxIdleTimer(parent);
#elif defined Q_OS_MACOS
  auto* rbt = new ReadBasedIdleTime(parent, macosIdleTime);
  auto* monitor = new MacosInhibitor(rbt);
  rbt->setInhibitor([monitor]() { return monitor->isDisplayInhibited(); });
  return rbt;
#elif defined Q_OS_WIN
  auto* rbt = new ReadBasedIdleTime(parent, windowsIdleTime);
  rbt->setInhibitor(windowsDisplayInhibited);
  return rbt;
#endif
}
