// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/pwr_mgt/IOPMLib.h>

#include <QObject>
#include <QTimer>

// Detects active display-idle power assertions on macOS.
//
// HIDIdleTime (read by macosIdleTime()) only tracks HID input, so a video player
// holding a PreventUserIdleDisplaySleep assertion does not pause it. In InhibitorAware
// mode we read the system-wide assertion status and zero the reported idle while one
// is active.
//
// The aggregate status is polled on a slow timer (~2 s) and cached; the per-tick
// reader only reads the cached flag. No entitlement changes.
class MacosInhibitor : public QObject {
  Q_OBJECT
 public:
  explicit MacosInhibitor(QObject* parent = nullptr);
  // Cached flag: is there any active assertion of type PreventUserIdleDisplaySleep?
  bool isDisplayInhibited() const { return m_inhibited; }

 private slots:
  void refresh();

 private:
  QTimer* m_timer;
  bool m_inhibited = false;
};
