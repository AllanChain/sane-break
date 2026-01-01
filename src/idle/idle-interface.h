// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "core/idle-time.h"

class IdleTimeInterface : public SystemIdleTime {
 public:
  virtual ~IdleTimeInterface() = default;
};

#define IdleTimeInterface_iid "io.github.AllanChain.sane-break.IdleTimeInterface/1.0"
Q_DECLARE_INTERFACE(IdleTimeInterface, IdleTimeInterface_iid)

class IdleTimeReaderInterface : public QObject {
 public:
  virtual int read() = 0;
};

#define IdleTimeReaderInterface_iid \
  "io.github.AllanChain.sane-break.IdleTimeReaderInterface/1.0"
Q_DECLARE_INTERFACE(IdleTimeReaderInterface, IdleTimeReaderInterface_iid)
