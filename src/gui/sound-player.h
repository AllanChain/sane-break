// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>
#include <QUrl>

class SoundPlayer : public QObject {
  Q_OBJECT

 public:
  SoundPlayer(QObject *parent = nullptr) : QObject(parent) {};
  void play(const QString &soundFile);

 private:
  QMediaPlayer *player = nullptr;
  QAudioOutput *audioOutput = nullptr;
};

#endif  // SOUND_PLAYER_H
