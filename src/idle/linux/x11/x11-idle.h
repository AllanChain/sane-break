// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>

#include "idle/idle-interface.h"

class IdleTimeX11 : public IdleTimeReaderInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID IdleTimeReaderInterface_iid)
  Q_INTERFACES(IdleTimeReaderInterface)
 public:
  IdleTimeX11() = default;
  ~IdleTimeX11() = default;
  int read() override;
};
