// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QWindow>

class LayerShellInterface : public QObject {
 public:
  virtual ~LayerShellInterface() = default;
  virtual void init() = 0;
  virtual void layout(QWindow* window) = 0;
};

#define LayerShellInterface_iid \
  "io.github.AllanChain.sane-break.LayerShellInterface/1.0"
Q_DECLARE_INTERFACE(LayerShellInterface, LayerShellInterface_iid)
