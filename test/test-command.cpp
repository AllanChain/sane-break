// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QObject>
#include <QSqlDatabase>
#include <QTest>
#include <future>

#include "core/cli.h"
#include "core/command-ipc.h"
#include "core/flags.h"
#include "dummy.h"
#include "gmock/gmock.h"

using namespace std::chrono_literals;
using testing::NiceMock;

class TestCommand : public QObject {
  Q_OBJECT
 private:
  QObject* depsParent = nullptr;
  DummyAppDependencies deps;

  template <typename T>
  T waitForFuture(std::future<T>& future) {
    while (future.wait_for(10ms) != std::future_status::ready) {
      QCoreApplication::processEvents();
    }
    return future.get();
  }

 private slots:
  void init() {
    QTest::failOnWarning();
    QLocalServer::removeServer(externalCommandServerName());
    QCoreApplication::setOrganizationName("SaneBreak");
    QCoreApplication::setApplicationName("SaneBreak");
    QCoreApplication::setApplicationVersion("test-version");
    depsParent = new QObject();
    deps = DummyApp::makeDeps(depsParent);
  }

  void cleanup() { delete depsParent; }

  void cli_router_launches_gui_without_arguments() {
    QVERIFY(shouldLaunchGuiForArguments({}));
  }

  void cli_router_launches_gui_for_qt_options() {
    QVERIFY(shouldLaunchGuiForArguments({"-platform", "wayland"}));
  }

  void cli_router_sends_help_and_version_to_running_instance() {
    QVERIFY(!shouldLaunchGuiForArguments({"--help"}));
    QVERIFY(!shouldLaunchGuiForArguments({"--version"}));
  }

  void cli_router_sends_commands_to_running_instance() {
    QVERIFY(!shouldLaunchGuiForArguments({"pause"}));
  }

  void cli_rejects_command_arguments() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"pause", "10m"});
    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("10m"), qPrintable(result.message));
    QVERIFY(!app.trayData.pauseReasons);
  }

  void cli_accepts_subcommand_without_arguments() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"pause"});
    QVERIFY2(result.ok, qPrintable(result.message));
    QCOMPARE(app.trayData.pauseReasons, PauseReason::ExternalControl);
  }

  void cli_accepts_postpone_time_argument_data() {
    QTest::addColumn<QString>("timeArgument");

    QTest::newRow("minutes-suffix") << QString("10m");
    QTest::newRow("bare-minutes") << QString("10");
  }

  void cli_accepts_postpone_time_argument() {
    QFETCH(QString, timeArgument);

    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    CliCommandResult result = executeCliCommand(&app, {"postpone", timeArgument});

    QVERIFY2(result.ok, qPrintable(result.message));
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak + 10 * 60);
  }

  void cli_rejects_invalid_postpone_time() {
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    CliCommandResult result = executeCliCommand(&app, {"postpone", "soon"});

    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("soon"), qPrintable(result.message));
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
  }

  void cli_shows_status() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"status"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QCOMPARE(result.message, QString("Mode: normal\n"
                                     "Next break: small in 20m 0s\n"
                                     "Next big break: 1h 0m 0s"));
    QVERIFY(!app.trayData.pauseReasons);
  }

  void cli_shows_json_status() {
    NiceMock<DummyApp> app(deps);
    app.start();
    app.pauseByExternalControl();

    CliCommandResult result = executeCliCommand(&app, {"status", "--json"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QJsonDocument document = QJsonDocument::fromJson(result.message.toUtf8());
    QVERIFY2(document.isObject(), qPrintable(result.message));
    QJsonObject status = document.object();
    QCOMPARE(status["mode"].toString(), QString("paused"));
    QCOMPARE(status["isBreaking"].toBool(), false);
    QCOMPARE(status["nextBreakType"].toString(), QString("small"));
    QCOMPARE(status["pauseReasons"].toArray().first().toString(),
             QString("external-control"));
    QCOMPARE(status["meeting"].toObject()["active"].toBool(), false);
    QCOMPARE(status["focus"].toObject()["active"].toBool(), false);
    QCOMPARE(app.trayData.pauseReasons, PauseReason::ExternalControl);
  }

  void cli_starts_meeting() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(
        &app, {"meeting", "start", "--for", "45m", "--reason", "client call"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY(app.trayData.isInMeeting);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 45 * 60);
    QCOMPARE(app.trayData.meetingTotalSeconds, 45 * 60);
  }

  void cli_extends_meeting() {
    NiceMock<DummyApp> app(deps);
    app.start();
    app.startMeeting(10 * 60, "client call");

    CliCommandResult result = executeCliCommand(&app, {"meeting", "extend", "5m"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QCOMPARE(app.trayData.meetingSecondsRemaining, 15 * 60);
    QCOMPARE(app.trayData.meetingTotalSeconds, 15 * 60);
  }

  void cli_ends_meeting_with_break_later() {
    NiceMock<DummyApp> app(deps);
    app.start();
    app.startMeeting(10 * 60, "client call");

    CliCommandResult result =
        executeCliCommand(&app, {"meeting", "end", "--break-later", "2m"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY(!app.trayData.isInMeeting);
    QCOMPARE(app.trayData.secondsToNextBreak, 2 * 60);
  }

  void cli_ends_meeting_with_break_now() {
    NiceMock<DummyApp> app(deps);
    app.start();
    app.startMeeting(10 * 60, "client call");

    CliCommandResult result =
        executeCliCommand(&app, {"meeting", "end", "--break-now"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY(!app.trayData.isInMeeting);
    QVERIFY(app.trayData.isBreaking);
  }

  void cli_rejects_meeting_commands_when_inactive_data() {
    QTest::addColumn<QStringList>("arguments");

    QTest::newRow("end") << QStringList({"meeting", "end"});
    QTest::newRow("extend") << QStringList({"meeting", "extend", "5m"});
  }

  void cli_rejects_meeting_commands_when_inactive() {
    QFETCH(QStringList, arguments);

    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, arguments);

    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("not active"), qPrintable(result.message));
  }

  void cli_rejects_invalid_meeting_duration() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result =
        executeCliCommand(&app, {"meeting", "start", "--for", "soon"});

    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("soon"), qPrintable(result.message));
    QVERIFY(!app.trayData.isInMeeting);
  }

  void cli_starts_focus() {
    deps.preferences->focusSmallEvery->set(10 * 60);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(
        &app, {"focus", "start", "--for", "25m", "--reason", "deep work"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY(app.trayData.isFocusMode);
    QVERIFY(app.trayData.isBreaking);
    QCOMPARE(app.trayData.focusCyclesRemaining, 3);
    QCOMPARE(app.trayData.focusTotalCycles, 3);
  }

  void cli_ends_focus() {
    deps.preferences->focusSmallEvery->set(10 * 60);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();
    app.startFocus(3, "deep work");
    app.advanceToBreakEnd();

    CliCommandResult result = executeCliCommand(&app, {"focus", "end"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY(app.trayData.isBreaking);
    app.advanceToBreakEnd();
    QVERIFY(!app.trayData.isFocusMode);
  }

  void cli_rejects_focus_end_during_break() {
    deps.preferences->focusSmallEvery->set(10 * 60);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();
    app.startFocus(3, "deep work");

    CliCommandResult result = executeCliCommand(&app, {"focus", "end"});

    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("during a break"), qPrintable(result.message));
    QVERIFY(app.trayData.isFocusMode);
  }

  void cli_rejects_focus_start_during_meeting() {
    NiceMock<DummyApp> app(deps);
    app.start();
    app.startMeeting(10 * 60, "client call");

    CliCommandResult result =
        executeCliCommand(&app, {"focus", "start", "--for", "25m"});

    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("meeting"), qPrintable(result.message));
    QVERIFY(!app.trayData.isFocusMode);
    QVERIFY(app.trayData.isInMeeting);
  }

  void cli_shows_command_help() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"pause", "--help"});
    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY2(result.message.contains("pause"), qPrintable(result.message));
    QVERIFY(!app.trayData.pauseReasons);
  }

  void cli_shows_global_help() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"--help"});
    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY2(result.message.contains("break-now"), qPrintable(result.message));
  }

  void cli_shows_version() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"--version"});
    QVERIFY2(result.ok, qPrintable(result.message));
    QCOMPARE(result.message, QString("Sane Break test-version"));
  }

  void cli_shows_command_version() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"pause", "--version"});
    QVERIFY2(result.ok, qPrintable(result.message));
    QCOMPARE(result.message, QString("Sane Break test-version"));
    QVERIFY(!app.trayData.pauseReasons);
  }

  void cli_rejects_unknown_command() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandResult result = executeCliCommand(&app, {"unknown"});
    QVERIFY(!result.ok);
    QVERIFY2(result.message.contains("unknown"), qPrintable(result.message));
    QVERIFY2(result.message.contains("break-now"), qPrintable(result.message));
  }

  void external_control_pause_round_trip() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.pauseByExternalControl();
    QCOMPARE(app.trayData.pauseReasons, PauseReason::ExternalControl);

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    app.resumeFromExternalControl();
    QVERIFY(!app.trayData.pauseReasons);
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }

  void resume_external_control_preserves_other_pause_reasons() {
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->pauseRequested(PauseReason::OnBattery);
    app.pauseByExternalControl();
    QVERIFY(app.trayData.pauseReasons.testFlag(PauseReason::OnBattery));
    QVERIFY(app.trayData.pauseReasons.testFlag(PauseReason::ExternalControl));

    app.resumeFromExternalControl();
    QVERIFY(app.trayData.pauseReasons.testFlag(PauseReason::OnBattery));
    QVERIFY(!app.trayData.pauseReasons.testFlag(PauseReason::ExternalControl));
  }

  void cli_enable_break_clears_all_pause_reasons() {
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->pauseRequested(PauseReason::OnBattery);
    app.pauseByExternalControl();
    QVERIFY(app.trayData.pauseReasons.testFlag(PauseReason::OnBattery));
    QVERIFY(app.trayData.pauseReasons.testFlag(PauseReason::ExternalControl));

    CliCommandResult result = executeCliCommand(&app, {"enable-break"});

    QVERIFY2(result.ok, qPrintable(result.message));
    QVERIFY(!app.trayData.pauseReasons);
  }

  void command_server_dispatches_requests() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandServer server(&app);
    QString errorMessage;
    if (!server.start(&errorMessage)) {
      QSKIP(qPrintable(QString("QLocalServer unavailable in this environment: %1")
                           .arg(errorMessage)));
    }

    auto pauseFuture =
        std::async(std::launch::async, []() { return sendCliCommand({"pause"}); });
    CliCommandResult pauseResult = waitForFuture(pauseFuture);
    QVERIFY2(pauseResult.ok, qPrintable(pauseResult.message));
    QCOMPARE(app.trayData.pauseReasons, PauseReason::ExternalControl);

    auto resumeFuture =
        std::async(std::launch::async, []() { return sendCliCommand({"resume"}); });
    CliCommandResult resumeResult = waitForFuture(resumeFuture);
    QVERIFY2(resumeResult.ok, qPrintable(resumeResult.message));
    QVERIFY(!app.trayData.pauseReasons);
  }

  void command_server_handles_help_and_version() {
    NiceMock<DummyApp> app(deps);
    app.start();

    CliCommandServer server(&app);
    QString errorMessage;
    if (!server.start(&errorMessage)) {
      QSKIP(qPrintable(QString("QLocalServer unavailable in this environment: %1")
                           .arg(errorMessage)));
    }

    auto helpFuture =
        std::async(std::launch::async, []() { return sendCliCommand({"--help"}); });
    CliCommandResult helpResult = waitForFuture(helpFuture);
    QVERIFY2(helpResult.ok, qPrintable(helpResult.message));
    QVERIFY2(helpResult.message.contains("break-now"), qPrintable(helpResult.message));

    auto versionFuture =
        std::async(std::launch::async, []() { return sendCliCommand({"--version"}); });
    CliCommandResult versionResult = waitForFuture(versionFuture);
    QVERIFY2(versionResult.ok, qPrintable(versionResult.message));
    QCOMPARE(versionResult.message, QString("Sane Break test-version"));
  }

  void command_client_fails_without_server() {
    CliCommandResult result = sendCliCommand({"break-now"}, 100);
    QVERIFY(!result.ok);
    QCOMPARE(result.message, QString("No running Sane Break instance is available."));
  }

  void command_server_name_matches_platform() {
#ifdef Q_OS_WINDOWS
    QVERIFY(
        externalCommandServerName().startsWith(QStringLiteral("\\\\.\\pipe\\LOCAL\\")));
    QVERIFY(externalCommandServerName().contains(
        QStringLiteral("SaneBreak.CommandServer")));
    QVERIFY(!externalCommandServerName().contains(QLatin1Char('/')));
#else
    QVERIFY(externalCommandServerName().endsWith(QStringLiteral(".sock")));
    QVERIFY(
        !externalCommandServerName().contains(QStringLiteral("\\\\.\\pipe\\LOCAL\\")));
#endif
  }
};

QTEST_MAIN(TestCommand)
#include "test-command.moc"
