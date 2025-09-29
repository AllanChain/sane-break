// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "auto-start.h"

#include <Windows.h>
#include <appmodel.h>
#include <basetsd.h>
#include <winerror.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLoggingCategory>
#include <QSettings>

#include "core/preferences.h"

// Include WinRT headers for MSIX support
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>

using namespace winrt::Windows::ApplicationModel;

// Use Windows API directly to detect packaged app to avoid WinRT ABI conflicts with Qt
bool isPackagedApp() {
  UINT32 len = 0;
  LONG result = GetCurrentPackageFamilyName(&len, nullptr);
  return result != APPMODEL_ERROR_NO_PACKAGE;
}

void AutoStart::setEnabled(bool enabled) {
  if (isPackagedApp()) {
    try {
      winrt::init_apartment(winrt::apartment_type::single_threaded);

      auto startupTask = StartupTask::GetAsync(L"SaneBreak").get();
      auto state = startupTask.State();

      if (enabled) {
        switch (state) {
          case StartupTaskState::DisabledByPolicy:
            emit operationResult(false, tr("Disallowed by policy"));
            break;
          case StartupTaskState::DisabledByUser:
            emit operationResult(false, tr("Disallowed by user"));
            break;
          case StartupTaskState::Disabled:
            startupTask.RequestEnableAsync();
            emit operationResult(true);
            break;
          default:
            emit operationResult(true);
            break;
        }
      } else {
        startupTask.Disable();
        switch (state) {
          case StartupTaskState::EnabledByPolicy:
            emit operationResult(false, tr("Disallowed by policy"));
            break;
          case StartupTaskState::Enabled:
            startupTask.Disable();
            emit operationResult(true);
            break;
          default:
            emit operationResult(true);
            break;
        }
      }
      winrt::uninit_apartment();
    } catch (const winrt::hresult_error& ex) {
      winrt::uninit_apartment();
      qWarning() << "MSIX AutoStart failed:"
                 << QString::fromWCharArray(ex.message().c_str());
      emit operationResult(false, QString::fromWCharArray(ex.message().c_str()));
    } catch (...) {
      winrt::uninit_apartment();
      qWarning() << "MSIX AutoStart failed with unknown error";
      emit operationResult(false, tr("Unknown error in MSIX autostart"));
    }
  } else {
    // Use registry-based autostart for traditional Win32 applications
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    if (enabled) {
      settings.setValue(
          QCoreApplication::applicationName(),
          QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    } else {
      settings.remove(QCoreApplication::applicationName());
    }
    switch (settings.status()) {
      case QSettings::NoError:
        emit operationResult(true);
        return;
      case QSettings::AccessError:
        //: Error message when requesting autostart got permission denied
        emit operationResult(false, tr("Permission denied"));
        return;
      default:
        emit operationResult(false, tr("Unknown error"));
        return;
    }
  }
}

bool AutoStart::isEnabled(SanePreferences*) {
  if (isPackagedApp()) {
    // For MSIX packages, use WinRT implementation directly
    try {
      winrt::init_apartment(winrt::apartment_type::single_threaded);

      auto startupTask = StartupTask::GetAsync(L"SaneBreak");
      auto result = startupTask.get();
      bool enabled = result.State() == StartupTaskState::Enabled ||
                     result.State() == StartupTaskState::EnabledByPolicy;

      winrt::uninit_apartment();
      return enabled;
    } catch (const winrt::hresult_error& ex) {
      winrt::uninit_apartment();
      qWarning() << "MSIX AutoStart check failed:"
                 << QString::fromWCharArray(ex.message().c_str());
      return false;
    } catch (...) {
      winrt::uninit_apartment();
      qWarning() << "MSIX AutoStart check failed with unknown error";
      return false;
    }
  } else {
    // Check registry-based autostart for traditional Win32 applications
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    return settings.contains(QCoreApplication::applicationName());
  }
}
