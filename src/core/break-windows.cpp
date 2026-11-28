// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/break-windows.h"

#include <QColor>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "core/flags.h"
#include "core/preferences.h"

namespace {

QString rollMessages(const QStringList& messagesToRoll) {
  if (!messagesToRoll.empty()) {
    int randomIndex = QRandomGenerator::global()->bounded(messagesToRoll.size());
    return messagesToRoll[randomIndex];
  }
  return "";
}

BreakWindowMessage breakMessage(BreakType type, SanePreferences* preferences) {
  BreakTypeSettings settings = preferences->breakSettings(type);
  return {
      .prompt = settings.messages->defaultValue()[0],
      .fullScreen = rollMessages(settings.messages->get()),
  };
}

BreakWindowTheme breakTheme(BreakType type, SanePreferences* preferences) {
  BreakTypeSettings settings = preferences->breakSettings(type);
  return {
      .background = settings.backgroundColor->get(),
      .flash = settings.flashColor->get(),
      .messageColor = preferences->messageColor->get(),
      .countDownColor = preferences->countDownColor->get(),
      .backgroundImage = settings.backgroundImage->get(),
  };
}

BreakWindowShow breakShow(BreakType type, SanePreferences* preferences) {
  BreakTypeSettings settings = preferences->breakSettings(type);
  return {
      .prograssBar = settings.showProgressBar->get(),
      .countdown = settings.showCountdown->get(),
      .clock = settings.showClock->get(),
      .endTime = settings.showEndTime->get(),
      .buttons = settings.showButtons->get(),
  };
}

void applyColorIfValid(const QJsonObject& obj, const QString& key, QColor& target) {
  QJsonValue val = obj.value(key);
  if (!val.isString()) return;
  QColor color = QColor::fromString(val.toString());
  if (color.isValid()) target = color;
}

void applyBoolIfValid(const QJsonObject& obj, const QString& key, bool& target) {
  QJsonValue val = obj.value(key);
  if (val.isBool()) target = val.toBool();
}

}  // namespace

BreakWindowData BreakWindowData::create(BreakType type, SanePreferences* preferences,
                                        int breakDuration, bool isPostponed) {
  const int flashSpeed = preferences->flashSpeed->get();
  return {
      .type = type,
      .totalSeconds = breakDuration,
      .isPostponed = isPostponed,
      .flashAnimationDuration = flashSpeed > 0 ? 60000 / flashSpeed : 0,
      .message = breakMessage(type, preferences),
      .theme = breakTheme(type, preferences),
      .show = breakShow(type, preferences),
  };
}

QJsonObject BreakWindowData::toCustomizationJson() const {
  return {
      {"message",
       QJsonObject{
           {"prompt", message.prompt},
           {"fullScreen", message.fullScreen},
       }},
      {"theme",
       QJsonObject{
           {"background", theme.background.name(QColor::HexArgb)},
           {"flash", theme.flash.name(QColor::HexArgb)},
           {"messageColor", theme.messageColor.name(QColor::HexArgb)},
           {"countDownColor", theme.countDownColor.name(QColor::HexArgb)},
           {"backgroundImage", theme.backgroundImage},
       }},
      {"show",
       QJsonObject{
           {"progressBar", show.prograssBar},
           {"countdown", show.countdown},
           {"clock", show.clock},
           {"endTime", show.endTime},
           {"buttons", show.buttons},
       }},
  };
}

void BreakWindowData::applyCustomizationJson(const QJsonObject& json) {
  if (QJsonValue msgVal = json.value("message"); msgVal.isObject()) {
    QJsonObject msgObj = msgVal.toObject();
    if (QJsonValue v = msgObj.value("prompt"); v.isString()) {
      message.prompt = v.toString();
    }
    if (QJsonValue v = msgObj.value("fullScreen"); v.isString()) {
      message.fullScreen = v.toString();
    }
  }

  if (QJsonValue themeVal = json.value("theme"); themeVal.isObject()) {
    QJsonObject themeObj = themeVal.toObject();
    applyColorIfValid(themeObj, "background", theme.background);
    applyColorIfValid(themeObj, "flash", theme.flash);
    applyColorIfValid(themeObj, "messageColor", theme.messageColor);
    applyColorIfValid(themeObj, "countDownColor", theme.countDownColor);

    if (QJsonValue imgVal = themeObj.value("backgroundImage"); imgVal.isString()) {
      QString raw = imgVal.toString();
      if (raw.isEmpty()) {
        theme.backgroundImage.clear();
      } else {
        QString path = raw.startsWith("file:") ? QUrl(raw).toLocalFile() : raw;
        QFileInfo info(path);
        if (!path.isEmpty() && info.exists() && info.isFile()) {
          theme.backgroundImage = path;
        }
      }
    }
  }

  if (QJsonValue showVal = json.value("show"); showVal.isObject()) {
    QJsonObject showObj = showVal.toObject();
    applyBoolIfValid(showObj, "progressBar", show.prograssBar);
    applyBoolIfValid(showObj, "countdown", show.countdown);
    applyBoolIfValid(showObj, "clock", show.clock);
    applyBoolIfValid(showObj, "endTime", show.endTime);
    applyBoolIfValid(showObj, "buttons", show.buttons);
  }
}
