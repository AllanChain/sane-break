// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <qglobal.h>
#include <qlogging.h>

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QStyleHints>
#include <QTimer>
#include <QWindow>
#include <Qt>

#include "app/break-windows.h"
#include "app/pref-window.h"
#include "app/tray.h"
#include "app/widgets/language-select.h"
#include "core/app-states.h"
#include "core/app.h"
#include "core/db.h"
#include "core/preferences.h"
#include "idle/factory.h"
#include "lib/screen-lock.h"
#include "lib/system-monitor.h"
#include "lib/timer.h"
#include "meeting-prompt.h"
#include "focus-window.h"
#include "meeting-window.h"
#include "postpone-window.h"

#if (defined(Q_OS_LINUX) or defined(Q_OS_MACOS))
#include "app/unix/signal-handler.h"
#else
#include "app/windows/signal-handler.h"
#endif

SaneBreakApp::SaneBreakApp(const AppDependencies& deps, QObject* parent)
    : AbstractApp(deps, parent) {
  prefWindow = new PreferenceWindow(preferences);
  tray = StatusTrayWindow::createTrayOrWindow(preferences, this);

  connect(this, &SaneBreakApp::trayDataUpdated, tray, &StatusTrayWindow::update);
  connect(tray, &StatusTrayWindow::nextBreakRequested, this, &SaneBreakApp::breakNow);
  connect(tray, &StatusTrayWindow::nextBigBreakRequested, this,
          &SaneBreakApp::bigBreakNow);
  connect(tray, &StatusTrayWindow::smallBreakInsteadRequested, this,
          &SaneBreakApp::smallBreakInstead);
  connect(tray, &StatusTrayWindow::postponeRequested, this,
          &SaneBreakApp::openPostponeWindow);
  connect(tray, &StatusTrayWindow::meetingRequested, this,
          &SaneBreakApp::openMeetingWindow);
  connect(tray, &StatusTrayWindow::focusRequested, this,
          &SaneBreakApp::openFocusWindow);
  connect(tray, &StatusTrayWindow::endFocusRequested, this,
          &SaneBreakApp::endFocus);
  connect(tray, &StatusTrayWindow::endMeetingBreakNowRequested, this,
          [this]() { endMeetingBreakLater(0); });
  connect(tray, &StatusTrayWindow::extendMeetingRequested, this,
          &SaneBreakApp::extendMeeting);
  connect(tray, &StatusTrayWindow::preferenceWindowRequested, this,
          &SaneBreakApp::showPreferences);
  connect(tray, &StatusTrayWindow::enableBreakRequested, this,
          &SaneBreakApp::enableBreak);
  connect(tray, &StatusTrayWindow::quitRequested, this, &SaneBreakApp::confirmQuit);
  connect(preferences->language, &SettingWithSignal::changed, this,
          [this]() { LanguageSelect::setLanguage(preferences->language->get()); });

  connect(this, &SaneBreakApp::quit, qApp, &QApplication::quit, Qt::QueuedConnection);
  connect(qApp, &QCoreApplication::aboutToQuit, this, &SaneBreakApp::onExit);
#if (defined(Q_OS_LINUX) or defined(Q_OS_MACOS))
  auto signalHandler = new SignalHandler(this);
  signalHandler->setup();
  connect(signalHandler, &SignalHandler::exitRequested, this, &QCoreApplication::quit);
#endif
#ifdef Q_OS_WINDOWS
  qApp->installNativeEventFilter(new SignalHandler());
#endif
}

SaneBreakApp* SaneBreakApp::create(SanePreferences* preferences, QObject* parent) {
  AppDependencies deps = {
      .preferences = preferences,
      .db = new BreakDatabase(QSqlDatabase::addDatabase("QSQLITE")),
      .countDownTimer = new Timer(),
      .screenLockTimer = new Timer(),
      .idleTimer = createIdleTimer(parent),
      .systemMonitor = new SystemMonitor(preferences),
      .breakWindows = new BreakWindows(),
      .meetingPrompt = new MeetingPrompt(),
  };
  return new SaneBreakApp(deps, parent);
}

void SaneBreakApp::start() {
  AbstractApp::start();
  tray->show();
}

void SaneBreakApp::doLockScreen() { lockScreen(); }

void SaneBreakApp::showPreferences() {
  prefWindow->show();
  prefWindow->windowHandle()->raise();
  prefWindow->windowHandle()->requestActivate();
}

void SaneBreakApp::openFocusWindow() {
  if (data->isFocusMode() || m_currentState->getID() == AppState::Meeting ||
      data->isPostponing())
    return;
  FocusWindow* focusWindow = new FocusWindow(preferences);
  connect(focusWindow, &FocusWindow::focusRequested, this,
          &SaneBreakApp::startFocus);
  focusWindow->show();
}

void SaneBreakApp::openPostponeWindow() {
  if (data->isFocusMode()) {
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot postpone during focus mode."));
    msgBox.setInformativeText(
        tr("End focus mode first if you want to postpone."));
    msgBox.setIcon(QMessageBox::Icon::Warning);
    msgBox.addButton(QMessageBox::Ok)->setText(tr("OK"));
    msgBox.exec();
    return;
  }
  if (data->isPostponing()) {
    QMessageBox msgBox;
    msgBox.setText(tr("You have already postponed once in this session."));
    msgBox.setInformativeText(tr("No further postpones are allowed."));
    msgBox.setIcon(QMessageBox::Icon::Warning);
    msgBox.addButton(QMessageBox::Ok)->setText(tr("OK"));
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.exec();
    return;
  }
  PostponeWindow* postponeWindow = new PostponeWindow(preferences, db);
  connect(postponeWindow, &PostponeWindow::postponeRequested, this,
          &SaneBreakApp::postpone);
  postponeWindow->show();
}

void SaneBreakApp::openMeetingWindow() {
  if (m_currentState->getID() == AppState::Meeting || data->isPostponing()) return;
  MeetingWindow* meetingWindow = new MeetingWindow(preferences, db);
  connect(meetingWindow, &MeetingWindow::meetingRequested, this,
          [this](QTime endTime, QString reason) {
            int seconds = QTime::currentTime().secsTo(endTime);
            if (seconds > 0) startMeeting(seconds, reason);
          });
  meetingWindow->show();
}

void SaneBreakApp::confirmQuit() {
  QMessageBox msgBox;
  msgBox.setText(tr("Are you sure to quit Sane Break?"));
  msgBox.setInformativeText(tr("You can postpone the breaks instead."));
  msgBox.setIcon(QMessageBox::Icon::Question);
  msgBox.addButton(QMessageBox::Cancel)->setText(tr("Cancel"));
  msgBox.addButton(tr("Postpone"), QMessageBox::NoRole);
  msgBox.addButton(QMessageBox::Yes)->setText(tr("Yes"));

  msgBox.setDefaultButton(QMessageBox::Cancel);
  switch (msgBox.exec()) {
    case QMessageBox::Yes:
      emit quit();
      return;
    case QMessageBox::Cancel:
      return;
    default:
      openPostponeWindow();
      return;
  }
}
