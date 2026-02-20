# Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
# Copyright (C) 2024-2026 Sane Break developers
# SPDX-License-Identifier: GPL-3.0-or-later

# /// script
# dependencies = [
#   "openai",
# ]
# ///

import json
import os
import sys
from pathlib import Path
from typing import TypedDict
from xml.dom import minidom

from openai import OpenAI


class TranslationString(TypedDict):
    source: str
    translation: str | list[str]
    comment: str


PROMPT = """You are a helpful translation assistant.

You are going to help me translate the strings in a break reminder app called
Sane Break. The source strings are in English and the target language is {lang}.

Rules:
1. Preserve all XML tags (e.g. `<h3>`, `<b>`) and placeholders (e.g. `%1`, `%n`)
2. Keep punctuation consistent with the source string
3. For numerus forms (multiple translations in a list), provide all required forms
4. Use the comment field for context about where/how the string is used
5. It's okay to leave difficult strings untranslated rather than guessing

Here are some existing translations for reference (do NOT include these in your response):
{reference_json}

Translate these strings (respond with ONLY the translations array in JSON format):
{translate_json}
"""

TRANSLATION_JSON_SCHEMA = {
    "type": "object",
    "name": "translations",
    "strict": True,
    "schema": {
        "type": "object",
        "properties": {
            "translations": {
                "type": "array",
                "items": {
                    "type": "object",
                    "properties": {
                        "source": {"type": "string"},
                        "translation": {
                            "oneOf": [
                                {"type": "string"},
                                {"type": "array", "items": {"type": "string"}},
                            ]
                        },
                    },
                    "required": ["source", "translation"],
                    "additionalProperties": False,
                },
            }
        },
        "required": ["translations"],
        "additionalProperties": False,
    },
}


def ask_ai(language: str, prompt: str) -> list[dict]:
    client = OpenAI(
        api_key=os.getenv("OPENAI_API_KEY"),
        base_url=os.getenv("OPENAI_API_BASE"),
    )
    response = client.chat.completions.create(
        model=os.getenv("OPENAI_API_MODEL"),
        messages=[{"role": "user", "content": prompt}],
        response_format={
            "type": "json_schema",
            "json_schema": TRANSLATION_JSON_SCHEMA,
        },
        stream=True,
    )
    chunks = []
    for chunk in response:
        chunk_message = chunk.choices[0].delta.content
        if chunk_message:
            chunks.append(chunk_message)
            print(chunk_message, end="", flush=True)
    print()  # newline after streaming output
    result = json.loads("".join(chunks))
    return result["translations"]


def is_translated(ts: TranslationString) -> bool:
    t = ts["translation"]
    if isinstance(t, list):
        return all(form != "" for form in t)
    return t != ""


def translate_strings(
    language: str,
    reference: list[TranslationString],
    to_translate: list[TranslationString],
) -> list[TranslationString]:
    # Include comments in input for context (but not in output schema)
    reference_json = [
        {
            "source": ts["source"],
            "translation": ts["translation"],
            "comment": ts["comment"],
        }
        for ts in reference
    ]
    translate_json = [
        {
            "source": ts["source"],
            "translation": ts["translation"],
            "comment": ts["comment"],
        }
        for ts in to_translate
    ]

    prompt = PROMPT.format(
        lang=language,
        reference_json=json.dumps(reference_json, ensure_ascii=False, indent=2),
        translate_json=json.dumps(translate_json, ensure_ascii=False, indent=2),
    )
    return ask_ai(language, prompt)


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


def extract_string(message: minidom.Element) -> TranslationString:
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
    return {"source": source, "translation": translation, "comment": comment}


def translate_ts_file(ts_file: Path):
    document = minidom.parse(ts_file.open())
    ts_element = document.getElementsByTagName("TS")[0]
    language = ts_element.getAttribute("language")

    reference: list[TranslationString] = []
    untranslated_messages: list[minidom.Element] = []
    untranslated_strings: list[TranslationString] = []

    for message in document.getElementsByTagName("message"):
        ts = extract_string(message)
        if is_translated(ts):
            reference.append(ts)
        else:
            untranslated_messages.append(message)
            untranslated_strings.append(ts)

    if not untranslated_strings:
        print("All strings are already translated.")
        return

    print(f"{len(untranslated_strings)} to translate, {len(reference)} as reference.")
    results = translate_strings(language, reference, untranslated_strings)

    for message, translation in zip(untranslated_messages, results):
        source = get_text(message.getElementsByTagName("source")[0])
        if source != translation["source"]:
            print("Not match!", source, "vs", translation["source"])
            continue
        translation_element = message.getElementsByTagName("translation")[0]
        translation_element.setAttribute("type", "unfinished")
        if message.getAttribute("numerus"):
            # Numerus form: translation should be a list
            if not isinstance(translation["translation"], list):
                print(
                    f"Skip (expected list for numerus form): {source}",
                    file=sys.stderr,
                )
                continue
            if len(translation["translation"]) != 2:
                print(
                    f"Skip (expected 2 numerus forms, got {len(translation['translation'])}): {source}",
                    file=sys.stderr,
                )
                continue
            for numerus_element, numerus_form in zip(
                translation_element.getElementsByTagName("numerusform"),
                translation["translation"],
            ):
                set_text(numerus_element, numerus_form)
        else:
            # Non-numerus form: translation should be a string
            if isinstance(translation["translation"], list):
                print(
                    f"Skip (expected string, got list): {source}",
                    file=sys.stderr,
                )
                continue
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
