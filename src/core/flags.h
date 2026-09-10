// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QFlags>
#include <QList>
#include <QString>
#include <QStringList>

// Reasons the break schedule is currently suspended.
enum class PauseReason {
  Idle = 1 << 0,
  OnBattery = 1 << 1,
  AppOpen = 1 << 2,
  Sleep = 1 << 3,
  UnknownMonitor = 1 << 4,
  ExternalControl = 1 << 5,
  ScreenLock = 1 << 6,
};
Q_DECLARE_FLAGS(PauseReasons, PauseReason)
Q_DECLARE_OPERATORS_FOR_FLAGS(PauseReasons)

// Pause reasons come in two families.
//  - Absence: nobody is at the machine (no input, or a locked session). A break
//    detects absence on its own, through its prompt <-> full-screen phases, so these
//    never interrupt an ongoing break (see AppStateBreak::onPauseRequest), and the time
//    they cover counts as away rather than as a deliberate pause.
//  - Everything else: the user, an app, or the environment asked for a pause while the
//    user may still be present, so an ongoing break is ended for them.
constexpr PauseReasons AbsencePauseReasons =
    PauseReason::Idle | PauseReason::ScreenLock;

// The one place that maps a reason to its reported id. CLI status output and break
// spans both read through here, so the two can never disagree.
inline QString pauseReasonId(PauseReason reason) {
  switch (reason) {
    case PauseReason::Idle:
      return "idle";
    case PauseReason::OnBattery:
      return "on-battery";
    case PauseReason::AppOpen:
      return "app-open";
    case PauseReason::Sleep:
      return "sleep";
    case PauseReason::UnknownMonitor:
      return "unknown-monitor";
    case PauseReason::ExternalControl:
      return "external-control";
    case PauseReason::ScreenLock:
      return "screen-lock";
  }
  return "unknown";
}

// Emission order, also part of the schema: a reason array lists active reasons in this
// order. Append new reasons at the end and never rename an id, or consumers that read
// the arrays positionally break without anything failing here.
inline const QList<PauseReason>& pauseReasonSchemaOrder() {
  static const QList<PauseReason> kOrder = {
      PauseReason::Idle,      PauseReason::OnBattery,      PauseReason::AppOpen,
      PauseReason::Sleep,     PauseReason::UnknownMonitor, PauseReason::ExternalControl,
      PauseReason::ScreenLock};
  return kOrder;
}

inline QStringList pauseReasonIds(PauseReasons reasons) {
  QStringList ids;
  for (PauseReason reason : pauseReasonSchemaOrder()) {
    if (reasons.testFlag(reason)) ids.append(pauseReasonId(reason));
  }
  return ids;
}

enum class BreakType { Small, Big };
