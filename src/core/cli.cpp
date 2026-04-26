// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/cli.h"

#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "core/app.h"
#include "core/flags.h"

namespace {

constexpr auto kProgramName = "sane-break";

class CliTranslation {
  Q_DECLARE_TR_FUNCTIONS(Cli)
};

bool isGlobalCliOption(const QString& argument) {
  return argument == QStringLiteral("-h") || argument == QStringLiteral("--help") ||
         argument == QStringLiteral("--version");
}

std::optional<int> parseDurationSeconds(const std::string& rawValue) {
  QString value = QString::fromStdString(rawValue).trimmed();
  if (value.isEmpty()) return std::nullopt;

  int multiplier = 60;
  QChar suffix = value.back().toLower();
  if (suffix.isLetter()) {
    value.chop(1);
    if (suffix == QLatin1Char('s')) {
      multiplier = 1;
    } else if (suffix == QLatin1Char('m')) {
      multiplier = 60;
    } else if (suffix == QLatin1Char('h')) {
      multiplier = 60 * 60;
    } else {
      return std::nullopt;
    }
  }

  bool ok = false;
  qint64 amount = value.toLongLong(&ok);
  if (!ok || amount <= 0 || amount > std::numeric_limits<int>::max() / multiplier) {
    return std::nullopt;
  }
  return static_cast<int>(amount * multiplier);
}

std::optional<int> focusCyclesForDuration(int seconds, int secondsPerCycle) {
  if (seconds <= 0 || secondsPerCycle <= 0) return std::nullopt;
  qint64 cycles =
      (static_cast<qint64>(seconds) + secondsPerCycle - 1) / secondsPerCycle;
  if (cycles <= 0 || cycles > std::numeric_limits<int>::max()) return std::nullopt;
  return static_cast<int>(cycles);
}

QString pauseReasonName(PauseReason reason) {
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
  }
  return "unknown";
}

QStringList pauseReasonNames(PauseReasons reasons) {
  QStringList names;
  for (PauseReason reason :
       {PauseReason::Idle, PauseReason::OnBattery, PauseReason::AppOpen,
        PauseReason::Sleep, PauseReason::UnknownMonitor,
        PauseReason::ExternalControl}) {
    if (reasons.testFlag(reason)) names.append(pauseReasonName(reason));
  }
  return names;
}

QJsonArray pauseReasonsToJson(PauseReasons reasons) {
  QJsonArray names;
  for (const QString& reason : pauseReasonNames(reasons)) names.append(reason);
  return names;
}

QString statusModeName(const TrayData& data) {
  if (data.isBreaking) return "break";
  if (data.isInMeeting) return "meeting";
  if (data.pauseReasons) return "paused";
  if (data.isFocusMode) return "focus";
  return "normal";
}

QString nextBreakTypeName(const TrayData& data) {
  if (data.bigBreakEnabled && data.smallBreaksBeforeBigBreak == 0) return "big";
  return "small";
}

QJsonObject statusToJson(const TrayData& data) {
  return {
      {"mode", statusModeName(data)},
      {"isBreaking", data.isBreaking},
      {"nextBreakType", nextBreakTypeName(data)},
      {"nextBreakSeconds", data.secondsToNextBreak},
      {"bigBreakEnabled", data.bigBreakEnabled},
      {"nextBigBreakSeconds", data.secondsToNextBigBreak},
      {"pauseReasons", pauseReasonsToJson(data.pauseReasons)},
      {"isPostponing", data.isPostponing},
      {"meeting",
       QJsonObject{
           {"active", data.isInMeeting},
           {"remainingSeconds", data.meetingSecondsRemaining},
           {"totalSeconds", data.meetingTotalSeconds},
       }},
      {"focus",
       QJsonObject{
           {"active", data.isFocusMode},
           {"cyclesRemaining", data.focusCyclesRemaining},
           {"totalCycles", data.focusTotalCycles},
       }},
  };
}

QString compactJson(const QJsonObject& object) {
  return QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

CliCommandResult jsonStatusResult(const TrayData& data) {
  return {
      .ok = true,
      .message = compactJson(statusToJson(data)),
  };
}

CliCommandResult jsonErrorResult(const QString& code, const QString& message,
                                 const QJsonObject& details = {}) {
  QJsonObject error{
      {"error", code},
      {"message", message},
  };
  for (auto it = details.begin(); it != details.end(); ++it) {
    error.insert(it.key(), it.value());
  }
  return {
      .ok = false,
      .message = compactJson(error),
  };
}

void addVersionFlag(CLI::App& app) {
  app.set_version_flag(
      "--version", "",
      CliTranslation::tr("Show the application version").toStdString());
}

void configureRootCli(CLI::App& cli) {
  cli.set_help_flag("-h,--help",
                    CliTranslation::tr("Show this help text").toStdString());
  addVersionFlag(cli);
  cli.failure_message(CLI::FailureMessage::help);
  cli.require_subcommand(0, 1);
}

CLI::App* addCliCommand(CLI::App& cli, const QString& name, const QString& summary) {
  CLI::App* subcommand = cli.add_subcommand(name.toStdString(), summary.toStdString());
  addVersionFlag(*subcommand);
  return subcommand;
}

std::vector<std::string> toCliArguments(const QStringList& arguments) {
  std::vector<std::string> cliArguments;
  cliArguments.reserve(arguments.size());
  for (const QString& argument : arguments) {
    cliArguments.push_back(argument.toLocal8Bit().toStdString());
  }
  std::reverse(cliArguments.begin(), cliArguments.end());
  return cliArguments;
}

CliCommandResult handleCliParseError(const CLI::App& app,
                                     const CLI::ParseError& error) {
  if (error.get_name() == "CallForVersion") {
    return {
        .ok = true,
        .message = QString("Sane Break %1").arg(QCoreApplication::applicationVersion()),
    };
  }

  std::ostringstream out;
  std::ostringstream err;
  app.exit(error, out, err);

  const bool ok = error.get_exit_code() == 0;
  if (ok) {
    return {.ok = true, .message = QString::fromStdString(out.str())};
  }
  return jsonErrorResult("cli-parse-error", QString::fromStdString(err.str()));
}

}  // namespace

bool shouldLaunchGuiForArguments(const QStringList& arguments) {
  if (arguments.isEmpty()) return true;

  const QString& firstArgument = arguments.first();
  return firstArgument.startsWith('-') && !isGlobalCliOption(firstArgument);
}

CliCommandResult executeCliCommand(AbstractApp* app, const QStringList& arguments) {
  CLI::App cli{"Sane Break", kProgramName};
  configureRootCli(cli);

  CLI::App* breakNow =
      addCliCommand(cli, QStringLiteral("break-now"),
                    CliTranslation::tr("Start the next break immediately"));
  CLI::App* bigBreakNow =
      addCliCommand(cli, QStringLiteral("big-break-now"),
                    CliTranslation::tr("Start the next big break immediately"));
  CLI::App* pause =
      addCliCommand(cli, QStringLiteral("pause"),
                    CliTranslation::tr("Pause breaks by external control"));
  CLI::App* resume =
      addCliCommand(cli, QStringLiteral("resume"),
                    CliTranslation::tr("Resume breaks paused by external control"));
  CLI::App* enableBreak =
      addCliCommand(cli, QStringLiteral("enable-break"),
                    CliTranslation::tr("Enable breaks by clearing all pause reasons"));

  bool jsonStatus = false;
  CLI::App* status = addCliCommand(cli, QStringLiteral("status"),
                                   CliTranslation::tr("Show current break status"));
  std::string meetingStartDuration;
  std::string meetingStartReason;
  std::string meetingEndBreakLater;
  std::string meetingExtendDuration;
  bool meetingEndBreakNow = false;
  CLI::App* meeting =
      addCliCommand(cli, QStringLiteral("meeting"),
                    CliTranslation::tr("Manage meeting or presentation mode"));
  meeting->require_subcommand(1, 1);

  CLI::App* meetingStart =
      addCliCommand(*meeting, QStringLiteral("start"),
                    CliTranslation::tr("Start meeting mode for a duration"));
  meetingStart
      ->add_option("--for", meetingStartDuration,
                   CliTranslation::tr("Meeting duration, for example 45m, 2700s, or 1h")
                       .toStdString())
      ->required();
  meetingStart->add_option("--reason", meetingStartReason,
                           CliTranslation::tr("Meeting reason").toStdString());

  CLI::App* meetingEnd = addCliCommand(*meeting, QStringLiteral("end"),
                                       CliTranslation::tr("End meeting mode"));
  CLI::Option* meetingEndBreakNowOption = meetingEnd->add_flag(
      "--break-now", meetingEndBreakNow,
      CliTranslation::tr("Start a break immediately").toStdString());
  CLI::Option* meetingEndBreakLaterOption = meetingEnd->add_option(
      "--break-later", meetingEndBreakLater,
      CliTranslation::tr("Schedule the next break after this duration").toStdString());
  meetingEndBreakNowOption->excludes(meetingEndBreakLaterOption);

  CLI::App* meetingExtend =
      addCliCommand(*meeting, QStringLiteral("extend"),
                    CliTranslation::tr("Extend the current meeting"));
  meetingExtend
      ->add_option(
          "time", meetingExtendDuration,
          CliTranslation::tr("Duration to extend, for example 10m, 600s, or 1h")
              .toStdString())
      ->required();

  std::string focusStartDuration;
  std::string focusStartReason;
  CLI::App* focus = addCliCommand(cli, QStringLiteral("focus"),
                                  CliTranslation::tr("Manage focus mode"));
  focus->require_subcommand(1, 1);

  CLI::App* focusStart =
      addCliCommand(*focus, QStringLiteral("start"),
                    CliTranslation::tr("Start focus mode for a duration"));
  focusStart
      ->add_option("--for", focusStartDuration,
                   CliTranslation::tr("Focus duration, for example 50m, 3000s, or 1h")
                       .toStdString())
      ->required();
  focusStart->add_option("--reason", focusStartReason,
                         CliTranslation::tr("Focus reason").toStdString());

  CLI::App* focusEnd = addCliCommand(*focus, QStringLiteral("end"),
                                     CliTranslation::tr("End focus mode"));

  std::string postponeTime;
  CLI::App* postpone = addCliCommand(cli, QStringLiteral("postpone"),
                                     CliTranslation::tr("Postpone the next break"));
  postpone
      ->add_option(
          "time", postponeTime,
          CliTranslation::tr("Duration to postpone, for example 10m, 600s, or 1h. "
                             "Bare numbers are minutes.")
              .toStdString())
      ->required();

  std::vector<std::string> cliArguments = toCliArguments(arguments);
  try {
    cli.parse(cliArguments);
  } catch (const CLI::ParseError& error) {
    return handleCliParseError(cli, error);
  }

  if (breakNow->parsed()) {
    app->breakNow();
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (bigBreakNow->parsed()) {
    app->bigBreakNow();
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (pause->parsed()) {
    app->pauseByExternalControl();
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (resume->parsed()) {
    app->resumeFromExternalControl();
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (enableBreak->parsed()) {
    app->enableBreak();
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (status->parsed()) {
    Q_UNUSED(jsonStatus);
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (meetingStart->parsed()) {
    if (app->trayDataSnapshot().isInMeeting) {
      return jsonErrorResult("meeting-already-active",
                             QStringLiteral("Meeting mode is already active."));
    }

    std::optional<int> seconds = parseDurationSeconds(meetingStartDuration);
    if (!seconds) {
      return jsonErrorResult("invalid-duration",
                             QString("Invalid duration \"%1\". Use a positive duration "
                                     "such as 10m, 600s, or 1h.")
                                 .arg(QString::fromStdString(meetingStartDuration)),
                             {{"input", QString::fromStdString(meetingStartDuration)}});
    }

    app->startMeeting(*seconds, QString::fromStdString(meetingStartReason));
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (meetingEnd->parsed()) {
    if (!app->trayDataSnapshot().isInMeeting) {
      return jsonErrorResult("meeting-not-active",
                             QStringLiteral("Meeting mode is not active."));
    }

    if (meetingEndBreakNow) {
      app->endMeetingBreakNow();
      return jsonStatusResult(app->trayDataSnapshot());
    }

    int seconds = app->preferences->smallEvery->get();
    if (!meetingEndBreakLater.empty()) {
      std::optional<int> parsedSeconds = parseDurationSeconds(meetingEndBreakLater);
      if (!parsedSeconds) {
        return jsonErrorResult(
            "invalid-duration",
            QString(
                "Invalid duration \"%1\". Use a positive duration such as 10m, 600s, "
                "or 1h.")
                .arg(QString::fromStdString(meetingEndBreakLater)),
            {{"input", QString::fromStdString(meetingEndBreakLater)}});
      }
      seconds = *parsedSeconds;
    }

    app->endMeetingBreakLater(seconds);
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (meetingExtend->parsed()) {
    if (!app->trayDataSnapshot().isInMeeting) {
      return jsonErrorResult("meeting-not-active",
                             QStringLiteral("Meeting mode is not active."));
    }

    std::optional<int> seconds = parseDurationSeconds(meetingExtendDuration);
    if (!seconds) {
      return jsonErrorResult(
          "invalid-duration",
          QString("Invalid duration \"%1\". Use a positive duration such as 10m, 600s, "
                  "or 1h.")
              .arg(QString::fromStdString(meetingExtendDuration)),
          {{"input", QString::fromStdString(meetingExtendDuration)}});
    }

    app->extendMeeting(*seconds);
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (focusStart->parsed()) {
    TrayData data = app->trayDataSnapshot();
    if (data.isFocusMode) {
      return jsonErrorResult("focus-already-active",
                             QStringLiteral("Focus mode is already active."));
    }
    if (data.isInMeeting) {
      return jsonErrorResult(
          "focus-during-meeting",
          QStringLiteral("Cannot start focus mode during meeting mode."));
    }

    std::optional<int> seconds = parseDurationSeconds(focusStartDuration);
    std::optional<int> cycles =
        seconds
            ? focusCyclesForDuration(*seconds, app->preferences->focusSmallEvery->get())
            : std::nullopt;
    if (!cycles) {
      return jsonErrorResult("invalid-duration",
                             QString("Invalid duration \"%1\". Use a positive duration "
                                     "such as 10m, 600s, or 1h.")
                                 .arg(QString::fromStdString(focusStartDuration)),
                             {{"input", QString::fromStdString(focusStartDuration)}});
    }

    app->startFocus(*cycles, QString::fromStdString(focusStartReason));
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (focusEnd->parsed()) {
    TrayData data = app->trayDataSnapshot();
    if (!data.isFocusMode) {
      return jsonErrorResult("focus-not-active",
                             QStringLiteral("Focus mode is not active."));
    }
    if (data.isBreaking) {
      return jsonErrorResult("focus-end-during-break",
                             QStringLiteral("Cannot end focus mode during a break."));
    }

    app->endFocus();
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (postpone->parsed()) {
    std::optional<int> seconds = parseDurationSeconds(postponeTime);
    if (!seconds) {
      return jsonErrorResult("invalid-duration",
                             QString("Invalid duration \"%1\". Use a positive duration "
                                     "such as 10m, 600s, or 1h.")
                                 .arg(QString::fromStdString(postponeTime)),
                             {{"input", QString::fromStdString(postponeTime)}});
    }

    app->postpone(*seconds);
    return jsonStatusResult(app->trayDataSnapshot());
  }

  return jsonErrorResult("expected-command", QStringLiteral("Expected a command."));
}
