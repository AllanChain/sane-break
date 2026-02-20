# Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
# Copyright (C) 2024-2025 Sane Break developers
# SPDX-License-Identifier: GPL-3.0-or-later

# /// script
# dependencies = [
#   "requests",
# ]
# ///

import os

import requests

WEBLATE_DEFAULT_PRIORITY = 100
MANUAL_PRIORITY_THRESHOLD = 500
WEBLATE_API_KEY = os.getenv("WEBLATE_API_KEY")
headers = {"User-Agent": "Python", "Authorization": "Token " + WEBLATE_API_KEY}
weblate_api_base = "https://hosted.weblate.org/api/translations/sane-break/sane-break"
url = weblate_api_base + "/en/units/?format=json"
priorities = {
    "PrefWindow": 200,
    "PreferenceWindow": 190,
    "SanePreferences": 180,
    "BreakReminder": 180,
    "BreakWindow": 180,
    "WelcomeWindow": 180,
    "StatusTrayWindow": 170,
    "LanguageSelect": 170,
    "QCoreApplication": 160,
    # Weblate default priority is 100
    "SaneBreakApp": 90,
    "AutoStart": 60,
    # For any other strings not in the listed contexts
    "Others": 10,
}


def update_flags_priority(flags: str, priority: int) -> str:
    parts = flags.split(",") if flags else []

    existing_priority = None
    for part in parts:
        if part.startswith("priority:"):
            try:
                existing_priority = int(part.split(":", 1)[1])
            except (ValueError, IndexError):
                pass  # invalid format, ignore

    if existing_priority is not None and existing_priority >= MANUAL_PRIORITY_THRESHOLD:
        return flags
    else:
        # Remove existing priority parts and add new one
        filtered_parts = [p for p in parts if not p.startswith("priority:")]
        filtered_parts.append(f"priority:{priority}")
        return ",".join(filtered_parts)


while url:
    r = requests.get(url, headers=headers)
    r.raise_for_status()
    units = r.json()

    for unit in units["results"]:
        context = unit["context"]
        priority = priorities.get(context, priorities["Others"])
        extra_flags = unit.get("extra_flags", "")
        new_extra_flags = update_flags_priority(extra_flags, priority)
        if new_extra_flags == extra_flags or (
            "priority" not in extra_flags and priority == WEBLATE_DEFAULT_PRIORITY
        ):
            continue
        print(f"{new_extra_flags:<20} {unit['source']}")
        d = {"extra_flags": new_extra_flags}
        p = requests.patch(unit["source_unit"], headers=headers, json=d)
        print(p.json())

    url = units.get("next")
