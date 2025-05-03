# Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
# Copyright (C) 2024-2025 Sane Break developers
# SPDX-License-Identifier: GPL-3.0-or-later

# /// script
# dependencies = [
#   "PyYAML",
#   "openai",
# ]
# ///

import os
import re
import sys
from pathlib import Path
from typing import TypedDict
from xml.dom import minidom

from openai import OpenAI
import yaml


class TranslationString(TypedDict):
    source: str
    translation: str | list[str]
    comment: str


PROMPT = """You are a helpful translation assistant.

You are going to help me translate the strings in a break reminder app called
Sane Break. The source strings are in English and the target language is {lang}.
Some of the strings may have been translated and you don't need to change them.
The source strings and the comments will be presented in a YAML document. You
need to respond with YAML in exactly the same format and structure, enclosed with
triple backquotes. Some source strings has punctuations while others does not. You
should make the translated text consistent with the source string in terms of the
punctuations. Besides, some source strings has special XML formats (i.e. `<h3>`) or
special placeholders (e.g. `%1` and `%n`) which you should preserve. Just FYI, the
formats are the same as the Qt `.ts` file except in YAML format. There are some
translations involving numerus forms which are presented in a YAML list. You should
always quote the single line strings to avoid syntax errors. It's okey to leave some
difficult strings untranslated becuase it's better than giving wrong answers.

```yaml
{yaml}
```
"""


def str_presenter(dumper, data):
    """Configures yaml for dumping multiline strings.

    Ref: https://stackoverflow.com/questions/8640959
    """
    if len(data.splitlines()) > 1:  # check for multiline string
        return dumper.represent_scalar("tag:yaml.org,2002:str", data, style="|")
    return dumper.represent_scalar("tag:yaml.org,2002:str", data)


yaml.add_representer(str, str_presenter)
yaml.representer.SafeRepresenter.add_representer(str, str_presenter)


def ask_ai(langauge: str, prompt: str) -> str:
    client = OpenAI(
        api_key=os.getenv("OPENAI_API_KEY"),
        base_url=os.getenv("OPENAI_API_BASE"),
    )
    response = client.chat.completions.create(
        model=os.getenv("OPENAI_API_MODEL"),
        messages=[{"role": "user", "content": prompt}],
        stream=True,
    )
    chunks = []
    for chunk in response:
        chunk_message = chunk.choices[0].delta.content
        chunks.append(chunk_message)
        print(chunk_message, end="")
    return "".join(chunks)


def translate_strings(language: str, strings: list[TranslationString]):
    prompt = PROMPT.format(lang=language, yaml=yaml.dump(strings, allow_unicode=True))
    response = ask_ai(language, prompt)
    print(response)
    yaml_pattern = re.compile("```yaml\n([^`]*)```")
    yaml_string = re.search(yaml_pattern, response).group(1)
    yaml_string = re.sub(":([^']*): *$", r":'\1:'", yaml_string)
    return yaml.safe_load(yaml_string)


def get_text(element: minidom.Document) -> str:
    child = element.firstChild
    if child is None:
        return ""
    return child.data.strip()


def set_text(element: minidom.Element, text: str) -> None:
    child = element.firstChild
    if child is None:
        element.appendChild(element.ownerDocument.createTextNode(text))
    else:
        child.data = text


def translate_ts_file(ts_file: Path):
    all_strings: list[TranslationString] = []
    document = minidom.parse(ts_file.open())
    ts_element = document.getElementsByTagName("TS")[0]
    language = ts_element.getAttribute("language")
    for message in document.getElementsByTagName("message"):
        source = get_text(message.getElementsByTagName("source")[0])
        if comments := message.getElementsByTagName("extracomment"):
            comment = get_text(comments[0])
        else:
            comment = ""
        translation_element = message.getElementsByTagName("translation")[0]
        if message.getAttribute("numerus"):
            translation = [
                get_text(element)
                for element in translation_element.getElementsByTagName("numerusform")
            ]
        else:
            translation = get_text(translation_element)
        all_strings.append(
            {"source": source, "translation": translation, "comment": comment}
        )
    all_strings = translate_strings(language, all_strings)
    ts_iterator = zip(document.getElementsByTagName("message"), all_strings)
    for message, translation in ts_iterator:
        source = get_text(message.getElementsByTagName("source")[0])
        if source != translation["source"]:
            print("Not match!", source, "vs", translation["source"])
            continue
        translation_element = message.getElementsByTagName("translation")[0]
        if get_text(translation_element):
            continue
        translation_element.setAttribute("type", "unfinished")
        if message.getAttribute("numerus"):
            for numerus_element, numerus_form in zip(
                translation_element.getElementsByTagName("numerusform"),
                translation["translation"],
            ):
                set_text(numerus_element, numerus_form)
        else:
            set_text(translation_element, translation["translation"])
    with open(ts_file, "wb") as f:
        f.write(document.toprettyxml(indent="", newl="", encoding="utf-8"))


def create_new_ts_file(ts_file: Path) -> None:
    lang = ts_file.stem
    zh_cn_ts = ts_file.with_stem("zh_CN")
    document = minidom.parse(zh_cn_ts.open())
    ts_element = document.getElementsByTagName("TS")[0]
    ts_element.setAttribute("language", lang)
    for message in document.getElementsByTagName("message"):
        translation_element = message.getElementsByTagName("translation")[0]
        if message.getAttribute("numerus"):
            for element in translation_element.getElementsByTagName("numerusform"):
                element.data = ""
        else:
            if child := translation_element.firstChild:
                child.data = ""
    with open(ts_file, "wb") as f:
        f.write(document.toprettyxml(indent="", newl="", encoding="utf-8"))


if __name__ == "__main__":
    for lang in sys.argv[1:]:
        print("Translating", lang)
        ts_file = (
            Path(__file__).parent.parent / "resources" / "translations" / f"{lang}.ts"
        )
        if not ts_file.exists():
            create_new_ts_file(ts_file)
        translate_ts_file(ts_file)
