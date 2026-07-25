// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "program-monitor.h"

#include <QString>
#include <QStringList>

#include <memory>
#include <windows.h>
#include <tlhelp32.h>

// Minimal NT types needed for ProcessCommandLineInformation.
// Defined here to avoid dependency on <winternl.h> which has
// inconsistent support across compilers and static analyzers.
using NTSTATUS = LONG;
using PROCESSINFOCLASS = ULONG;

struct UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
};

// NtQueryInformationProcess is exported from ntdll.dll
extern "C" NTSTATUS NTAPI NtQueryInformationProcess(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength);

// ProcessCommandLineInformation (value 60) retrieves the full command line
// as a UNICODE_STRING. Available since Windows 8.1.
static constexpr PROCESSINFOCLASS kProcessCommandLineInformation = 60;

static QString getProcessCommandLine(HANDLE hProcess) {
  // First call to determine required buffer size
  ULONG returnLength = 0;
  NTSTATUS status = NtQueryInformationProcess(
      hProcess, kProcessCommandLineInformation, nullptr, 0, &returnLength);

  // STATUS_INFO_LENGTH_MISMATCH (0xC0000004) means the function told
  // us the required size — that's expected on the first call.
  if (returnLength == 0) return {};

  auto buffer = std::make_unique<BYTE[]>(returnLength);
  status = NtQueryInformationProcess(hProcess, kProcessCommandLineInformation,
                                     buffer.get(), returnLength,
                                     &returnLength);
  if (status != 0) return {};  // STATUS_SUCCESS == 0

  auto *pUnicodeStr = reinterpret_cast<UNICODE_STRING *>(buffer.get());
  if (!pUnicodeStr->Buffer || pUnicodeStr->Length == 0) return {};

  return QString::fromWCharArray(pUnicodeStr->Buffer,
                                 pUnicodeStr->Length / sizeof(WCHAR));
}

static QString getProcessExePath(HANDLE hProcess) {
  WCHAR exePath[MAX_PATH];
  DWORD size = MAX_PATH;
  if (QueryFullProcessImageNameW(hProcess, 0, exePath, &size)) {
    return QString::fromWCharArray(exePath, size);
  }
  return {};
}

const QStringList RunningProgramsMonitor::runningPrograms() {
  QStringList programs;

  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE) return programs;

  PROCESSENTRY32W pe;
  pe.dwSize = sizeof(pe);

  if (Process32FirstW(hSnapshot, &pe)) {
    do {
      // Skip the System Idle Process (PID 0)
      if (pe.th32ProcessID == 0) continue;

      QString commandLine;

      HANDLE hProcess = OpenProcess(
          PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE,
          pe.th32ProcessID);

      if (hProcess) {
        // Best: full command line via NtQueryInformationProcess (Win 8.1+)
        commandLine = getProcessCommandLine(hProcess);

        // Fallback: full executable path
        if (commandLine.isEmpty()) {
          commandLine = getProcessExePath(hProcess);
        }

        CloseHandle(hProcess);
      }

      // Last resort: short process name from the snapshot
      if (commandLine.isEmpty()) {
        commandLine = QString::fromWCharArray(pe.szExeFile);
      }

      if (!commandLine.isEmpty()) {
        programs.append(commandLine);
      }

    } while (Process32NextW(hSnapshot, &pe));
  }

  CloseHandle(hSnapshot);
  return programs;
}
