// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>
#include <QWindow>

#include "interface.h"

class LayerShell : public LayerShellInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID LayerShellInterface_iid)
  Q_INTERFACES(LayerShellInterface)
 public:
  LayerShell() {};
  ~LayerShell() override = default;
  void init() override;
  void layout(QWindow* window) override;
};
