# Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
# Copyright (C) 2024-2026 Sane Break developers
# SPDX-License-Identifier: GPL-3.0-or-later

# /// script
# dependencies = [
#   "fonttools[woff]",
# ]
# ///

import re
from pathlib import Path
import tempfile
import urllib.request

from fontTools.subset import Subsetter, save_font, load_font, Options


PROJ_ROOT = Path(__file__).parent.parent
UI_DIR = PROJ_ROOT / "src" / "app"
FONT_OUTPUT = PROJ_ROOT / "resources" / "fonts" / "bootstrap-icons.ttf"
DOWNLOAD_URL = (
    "https://cdn.jsdelivr.net/npm/bootstrap-icons@1.13.1/"
    "font/fonts/bootstrap-icons.woff2"
)


def find_codepoints() -> set[int]:
    pattern = re.compile(r"&amp;#x([0-9A-Fa-f]+);")
    codepoints = set()
    for ui_file in UI_DIR.rglob("*.ui"):
        try:
            content = ui_file.read_text(encoding="utf-8", errors="ignore")
            matches = pattern.findall(content)
            codepoints.update(matches)
        except Exception:
            pass
    return {int(code, 16) for code in codepoints}


def main():
    codepoints = find_codepoints()
    if not codepoints:
        print("No icons found, skipping.")
        return

    with tempfile.NamedTemporaryFile() as woff_file:
        print("Downloading icon font files...")
        urllib.request.urlretrieve(DOWNLOAD_URL, woff_file.name)
        print("Downloaded icon font file.")
        print(f"Subsetting with {len(codepoints)} codepoints")
        font = load_font(woff_file, Options())
        subsetter = Subsetter()
        subsetter.populate(unicodes=codepoints)
        subsetter.subset(font)
        save_font(font, FONT_OUTPUT, Options())
        print(f"Saved output to {FONT_OUTPUT}")


if __name__ == "__main__":
    main()
