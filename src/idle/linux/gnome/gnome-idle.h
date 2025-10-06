// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusConnection>
#include <QDBusInterface>

#include "idle/idle-interface.h"

class IdleTimeGNOME : public IdleTimeReaderInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID IdleTimeReaderInterface_iid)
  Q_INTERFACES(IdleTimeReaderInterface)
 public:
  IdleTimeGNOME();
  ~IdleTimeGNOME();
  int read();

 private:
  QDBusInterface* iface = nullptr;
};
