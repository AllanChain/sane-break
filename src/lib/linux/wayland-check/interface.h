// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QObject>

class WaylandCheckInterface : public QObject {
 public:
  virtual ~WaylandCheckInterface() = default;
  virtual bool supportsIdleNotify() = 0;
  virtual bool supportsLayerShell() = 0;
};

#define WaylandCheckInterface_iid \
  "io.github.AllanChain.sane-break.WaylandCheckInterface/1.0"
Q_DECLARE_INTERFACE(WaylandCheckInterface, WaylandCheckInterface_iid)
