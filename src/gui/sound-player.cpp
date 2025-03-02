// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sound-player.h"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>
#include <QUrl>

void SoundPlayer::play(const QString &soundFile) {
  if (soundFile.isEmpty()) return;

  // player and audioOutput should be lazy-initialized
  // Some devices will become lagging if enabled sound playing
  // We sould avoid any effect when no sound needs to be played
  if (audioOutput == nullptr) {
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(100);
  }
  if (player == nullptr) {
    player = new QMediaPlayer(this);
    player->setAudioOutput(audioOutput);
  }

  player->setSource(QUrl(soundFile));
  player->play();
}
