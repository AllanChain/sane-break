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
      {"pauseReasons", QJsonArray::fromStringList(pauseReasonIds(data.pauseReasons))},
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

CliCommandResult invalidDurationResult(const std::string& rawInput) {
  QString input = QString::fromStdString(rawInput);
  return jsonErrorResult("invalid-duration",
                         QString("Invalid duration \"%1\". Use a positive duration "
                                 "such as 10m, 600s, or 1h.")
                             .arg(input),
                         {{"input", input}});
}

CliCommandResult commandResult(AbstractApp* app, CommandError error) {
  switch (error) {
    case CommandError::None:
      return jsonStatusResult(app->trayDataSnapshot());
    case CommandError::MeetingAlreadyActive:
      return jsonErrorResult("meeting-already-active",
                             QStringLiteral("Meeting mode is already active."));
    case CommandError::MeetingNotActive:
      return jsonErrorResult("meeting-not-active",
                             QStringLiteral("Meeting mode is not active."));
    case CommandError::FocusAlreadyActive:
      return jsonErrorResult("focus-already-active",
                             QStringLiteral("Focus mode is already active."));
    case CommandError::FocusNotActive:
      return jsonErrorResult("focus-not-active",
                             QStringLiteral("Focus mode is not active."));
    case CommandError::FocusDuringMeeting:
      return jsonErrorResult(
          "focus-during-meeting",
          QStringLiteral("Cannot start focus mode during meeting mode."));
    case CommandError::FocusEndDuringBreak:
      return jsonErrorResult("focus-end-during-break",
                             QStringLiteral("Cannot end focus mode during a break."));
    case CommandError::AlreadyPostponing:
      return jsonErrorResult("already-postponing",
                             QStringLiteral("Break has already been postponed."));
    case CommandError::PostponeDuringFocus:
      return jsonErrorResult("postpone-during-focus",
                             QStringLiteral("Cannot postpone during focus mode."));
    case CommandError::PostponeDuringMeeting:
      return jsonErrorResult("postpone-during-meeting",
                             QStringLiteral("Cannot postpone during meeting mode."));
  }
  return jsonErrorResult("command-error", QStringLiteral("Command failed."));
}

struct CliCommands {
  CLI::App* breakNow = nullptr;
  CLI::App* bigBreakNow = nullptr;
  CLI::App* pause = nullptr;
  CLI::App* resume = nullptr;
  CLI::App* enableBreak = nullptr;
  CLI::App* status = nullptr;
  CLI::App* meetingStart = nullptr;
  CLI::App* meetingEnd = nullptr;
  CLI::App* meetingExtend = nullptr;
  CLI::App* focusStart = nullptr;
  CLI::App* focusEnd = nullptr;
  CLI::App* postpone = nullptr;

  std::string meetingStartDuration;
  std::string meetingStartReason;
  std::string meetingEndBreakLater;
  std::string meetingExtendDuration;
  bool meetingEndBreakNow = false;
  std::string focusStartDuration;
  std::string focusStartReason;
  std::string postponeTime;
};

void registerCliCommands(CLI::App& cli, CliCommands& commands) {
  commands.breakNow =
      addCliCommand(cli, QStringLiteral("break-now"),
                    CliTranslation::tr("Start the next break immediately"));
  commands.bigBreakNow =
      addCliCommand(cli, QStringLiteral("big-break-now"),
                    CliTranslation::tr("Start the next big break immediately"));
  commands.pause =
      addCliCommand(cli, QStringLiteral("pause"),
                    CliTranslation::tr("Pause breaks by external control"));
  commands.resume =
      addCliCommand(cli, QStringLiteral("resume"),
                    CliTranslation::tr("Resume breaks paused by external control"));
  commands.enableBreak =
      addCliCommand(cli, QStringLiteral("enable-break"),
                    CliTranslation::tr("Enable breaks by clearing all pause reasons"));
  commands.status = addCliCommand(cli, QStringLiteral("status"),
                                  CliTranslation::tr("Show current break status"));

  CLI::App* meeting =
      addCliCommand(cli, QStringLiteral("meeting"),
                    CliTranslation::tr("Manage meeting or presentation mode"));
  meeting->require_subcommand(1, 1);

  commands.meetingStart =
      addCliCommand(*meeting, QStringLiteral("start"),
                    CliTranslation::tr("Start meeting mode for a duration"));
  commands.meetingStart
      ->add_option("--for", commands.meetingStartDuration,
                   CliTranslation::tr("Meeting duration, for example 45m, 2700s, or 1h")
                       .toStdString())
      ->required();
  commands.meetingStart->add_option("--reason", commands.meetingStartReason,
                                    CliTranslation::tr("Meeting reason").toStdString());

  commands.meetingEnd = addCliCommand(*meeting, QStringLiteral("end"),
                                      CliTranslation::tr("End meeting mode"));
  CLI::Option* meetingEndBreakNowOption = commands.meetingEnd->add_flag(
      "--break-now", commands.meetingEndBreakNow,
      CliTranslation::tr("Start a break immediately").toStdString());
  CLI::Option* meetingEndBreakLaterOption = commands.meetingEnd->add_option(
      "--break-later", commands.meetingEndBreakLater,
      CliTranslation::tr("Schedule the next break after this duration").toStdString());
  meetingEndBreakNowOption->excludes(meetingEndBreakLaterOption);

  commands.meetingExtend =
      addCliCommand(*meeting, QStringLiteral("extend"),
                    CliTranslation::tr("Extend the current meeting"));
  commands.meetingExtend
      ->add_option(
          "time", commands.meetingExtendDuration,
          CliTranslation::tr("Duration to extend, for example 10m, 600s, or 1h")
              .toStdString())
      ->required();

  CLI::App* focus = addCliCommand(cli, QStringLiteral("focus"),
                                  CliTranslation::tr("Manage focus mode"));
  focus->require_subcommand(1, 1);

  commands.focusStart =
      addCliCommand(*focus, QStringLiteral("start"),
                    CliTranslation::tr("Start focus mode for a duration"));
  commands.focusStart
      ->add_option("--for", commands.focusStartDuration,
                   CliTranslation::tr("Focus duration, for example 50m, 3000s, or 1h")
                       .toStdString())
      ->required();
  commands.focusStart->add_option("--reason", commands.focusStartReason,
                                  CliTranslation::tr("Focus reason").toStdString());

  commands.focusEnd = addCliCommand(*focus, QStringLiteral("end"),
                                    CliTranslation::tr("End focus mode"));

  commands.postpone = addCliCommand(cli, QStringLiteral("postpone"),
                                    CliTranslation::tr("Postpone the next break"));
  commands.postpone
      ->add_option(
          "time", commands.postponeTime,
          CliTranslation::tr("Duration to postpone, for example 10m, 600s, or 1h. "
                             "Bare numbers are minutes.")
              .toStdString())
      ->required();
}

CliCommandResult dispatchCliCommand(AbstractApp* app, const CliCommands& commands) {
  if (commands.breakNow->parsed()) {
    app->breakNow();
    return jsonStatusResult(app->trayDataSnapshot());
  }
  if (commands.bigBreakNow->parsed()) {
    app->bigBreakNow();
    return jsonStatusResult(app->trayDataSnapshot());
  }
  if (commands.pause->parsed()) {
    app->pauseByExternalControl();
    return jsonStatusResult(app->trayDataSnapshot());
  }
  if (commands.resume->parsed()) {
    app->resumeFromExternalControl();
    return jsonStatusResult(app->trayDataSnapshot());
  }
  if (commands.enableBreak->parsed()) {
    app->enableBreak();
    return jsonStatusResult(app->trayDataSnapshot());
  }
  if (commands.status->parsed()) {
    return jsonStatusResult(app->trayDataSnapshot());
  }

  if (commands.meetingStart->parsed()) {
    std::optional<int> seconds = parseDurationSeconds(commands.meetingStartDuration);
    if (!seconds) return invalidDurationResult(commands.meetingStartDuration);
    return commandResult(
        app, app->startMeeting(*seconds,
                               QString::fromStdString(commands.meetingStartReason)));
  }

  if (commands.meetingEnd->parsed()) {
    if (commands.meetingEndBreakNow) {
      return commandResult(app, app->endMeetingBreakNow());
    }
    int seconds = app->preferences->smallEvery->get();
    if (!commands.meetingEndBreakLater.empty()) {
      std::optional<int> parsedSeconds =
          parseDurationSeconds(commands.meetingEndBreakLater);
      if (!parsedSeconds) return invalidDurationResult(commands.meetingEndBreakLater);
      seconds = *parsedSeconds;
    }
    return commandResult(app, app->endMeetingBreakLater(seconds));
  }

  if (commands.meetingExtend->parsed()) {
    std::optional<int> seconds = parseDurationSeconds(commands.meetingExtendDuration);
    if (!seconds) return invalidDurationResult(commands.meetingExtendDuration);
    return commandResult(app, app->extendMeeting(*seconds));
  }

  if (commands.focusStart->parsed()) {
    std::optional<int> seconds = parseDurationSeconds(commands.focusStartDuration);
    std::optional<int> cycles =
        seconds
            ? focusCyclesForDuration(*seconds, app->preferences->focusSmallEvery->get())
            : std::nullopt;
    if (!cycles) return invalidDurationResult(commands.focusStartDuration);
    return commandResult(
        app,
        app->startFocus(*cycles, QString::fromStdString(commands.focusStartReason)));
  }

  if (commands.focusEnd->parsed()) {
    return commandResult(app, app->endFocus());
  }

  if (commands.postpone->parsed()) {
    std::optional<int> seconds = parseDurationSeconds(commands.postponeTime);
    if (!seconds) return invalidDurationResult(commands.postponeTime);
    return commandResult(app, app->postpone(*seconds));
  }

  return jsonErrorResult("expected-command", QStringLiteral("Expected a command."));
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

  CliCommands commands;
  registerCliCommands(cli, commands);

  std::vector<std::string> cliArguments = toCliArguments(arguments);
  try {
    cli.parse(cliArguments);
  } catch (const CLI::ParseError& error) {
    return handleCliParseError(cli, error);
  }

  return dispatchCliCommand(app, commands);
}
