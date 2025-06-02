// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>

#include "interface.h"

class WaylandCheck : public WaylandCheckInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID WaylandCheckInterface_iid)
  Q_INTERFACES(WaylandCheckInterface)
 public:
  WaylandCheck();
  ~WaylandCheck() override = default;
  bool supportsIdleNotify() override { return m_idleNotify; }
  bool supportsLayerShell() override { return m_layerShell; }

  bool m_idleNotify = false;
  bool m_layerShell = false;
};
