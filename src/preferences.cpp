// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

#include <QObject>

Setting<int>* SanePreferences::smallEvery =
    new Setting<int>("break/small-every", 1200);
Setting<int>* SanePreferences::smallFor =
    new Setting<int>("break/small-for", 20);
Setting<int>* SanePreferences::bigAfter =
    new Setting<int>("break/big-after", 3);
Setting<int>* SanePreferences::bigFor = new Setting<int>("break/big-for", 60);
Setting<int>* SanePreferences::pauseOnIdleFor =
    new Setting<int>("break/pause-on-idle-for", 60);
Setting<int>* SanePreferences::resetOnIdleFor =
    new Setting<int>("break/reset-on-idle-for", 300);
Setting<bool>* SanePreferences::pauseOnBattery =
    new Setting<bool>("break/pause-on-battery", false);
