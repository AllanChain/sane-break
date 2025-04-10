import os

import requests

WEBLATE_API_KEY = os.getenv("WEBLATE_API_KEY")
headers = {"User-Agent": "F-Droid", "Authorization": "Token " + WEBLATE_API_KEY}
weblate_api_base = "https://hosted.weblate.org/api/translations/sane-break/sane-break"
url = weblate_api_base + "/en/units/?format=json"
priorities = {
    "PrefWindow": 100,
    "PreferenceWindow": 90,
    "SanePreferences": 90,
    "StatusTrayWindow": 90,
    "WelcomeWindow": 80,
    "LanguageSelect": 70,
    "QCoreApplication": 60,
    "BreakWindow": 60,
    "SaneBreakApp": 60,
    "AutoStart": 20,
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

    if existing_priority is not None and existing_priority > 100:
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
        priority = priorities.get(context, 10)
        extra_flags = unit.get("extra_flags", "")
        new_extra_flags = update_flags_priority(extra_flags, priority)
        if new_extra_flags == extra_flags or (
            "priority" not in extra_flags and priority == 100
        ):
            continue
        print(f"{new_extra_flags:<20} {unit['source']}")
        d = {"extra_flags": new_extra_flags}
        p = requests.patch(unit["source_unit"], headers=headers, json=d)
        print(p.json())

    url = units.get("next")
