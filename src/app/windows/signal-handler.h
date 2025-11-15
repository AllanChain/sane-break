// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qtypes.h>

#include <QAbstractNativeEventFilter>
#include <QByteArray>

class SignalHandler : public QAbstractNativeEventFilter {
  bool nativeEventFilter(const QByteArray& eventType, void* message,
                         qintptr* result) override;
};
