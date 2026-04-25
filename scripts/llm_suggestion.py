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
from concurrent.futures import Future, ThreadPoolExecutor, as_completed
from pathlib import Path
from queue import Queue
from threading import Thread
from typing import TypedDict
from xml.dom import minidom

from openai import OpenAI


class TranslationString(TypedDict):
    source: str
    translation: str | list[str]
    comment: str


class ProgressEvent(TypedDict):
    language: str
    kind: str
    message: str


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
        "additionalProperties": False,
    },
}


def emit_progress(
    progress_queue: Queue[ProgressEvent] | None,
    language: str,
    kind: str,
    message: str,
) -> None:
    if progress_queue is None:
        return
    progress_queue.put({"language": language, "kind": kind, "message": message})


def ask_ai(
    language: str,
    prompt: str,
    progress_queue: Queue[ProgressEvent] | None = None,
) -> list[dict]:
    client = OpenAI(
        api_key=os.getenv("OPENAI_API_KEY"),
        base_url=os.getenv("OPENAI_API_BASE"),
    )
    response = client.chat.completions.create(
        model=os.getenv("OPENAI_API_MODEL"),
        messages=[{"role": "user", "content": prompt}],
        extra_body={"enable_thinking": False},
        response_format={
            "type": "json_schema",
            "json_schema": TRANSLATION_JSON_SCHEMA,
        },
        stream=True,
    )
    chunks = []
    received_chars = 0
    has_announced_stream = False
    for chunk in response:
        chunk_message = chunk.choices[0].delta.content
        if chunk_message:
            chunks.append(chunk_message)
            received_chars += len(chunk_message)
            if not has_announced_stream:
                emit_progress(progress_queue, language, "stream", "Receiving response")
                has_announced_stream = True
            elif received_chars % 2048 < len(chunk_message):
                emit_progress(
                    progress_queue,
                    language,
                    "progress",
                    f"{received_chars} chars received",
                )
    return json.loads("".join(chunks))


def is_translated(ts: TranslationString) -> bool:
    t = ts["translation"]
    if isinstance(t, list):
        return all(form != "" for form in t)
    return t != ""


def translate_strings(
    language: str,
    reference: list[TranslationString],
    to_translate: list[TranslationString],
    progress_queue: Queue[ProgressEvent] | None = None,
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
    return ask_ai(language, prompt, progress_queue)


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


def translate_ts_file(
    ts_file: Path, progress_queue: Queue[ProgressEvent] | None = None
) -> None:
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
        emit_progress(
            progress_queue,
            language,
            "done",
            "All strings are already translated",
        )
        return

    emit_progress(
        progress_queue,
        language,
        "start",
        f"{len(untranslated_strings)} to translate, {len(reference)} as reference",
    )
    results = translate_strings(
        language, reference, untranslated_strings, progress_queue
    )

    for message, translation in zip(untranslated_messages, results):
        source = get_text(message.getElementsByTagName("source")[0])
        if source != translation["source"]:
            emit_progress(
                progress_queue,
                language,
                "warning",
                f"Source mismatch: {source} vs {translation['source']}",
            )
            continue
        translation_element = message.getElementsByTagName("translation")[0]
        translation_element.setAttribute("type", "unfinished")
        if message.getAttribute("numerus"):
            # Numerus form: translation should be a list
            if not isinstance(translation["translation"], list):
                emit_progress(
                    progress_queue,
                    language,
                    "warning",
                    f"Skip (expected list for numerus form): {source}",
                )
                continue
            numerus_elements = translation_element.getElementsByTagName("numerusform")
            if len(translation["translation"]) != len(numerus_elements):
                emit_progress(
                    progress_queue,
                    language,
                    "warning",
                    f"Skip (expected {len(numerus_elements)} numerus forms. "
                    f"Got {len(translation['translation'])}): {source}",
                )
                continue
            for numerus_element, numerus_form in zip(
                numerus_elements, translation["translation"]
            ):
                set_text(numerus_element, numerus_form)
        else:
            # Non-numerus form: translation should be a string
            if isinstance(translation["translation"], list):
                emit_progress(
                    progress_queue,
                    language,
                    "warning",
                    f"Skip (expected string, got list): {source}",
                )
                continue
            set_text(translation_element, translation["translation"])
    with open(ts_file, "wb") as f:
        f.write(document.toprettyxml(indent="", newl="", encoding="utf-8"))
    emit_progress(progress_queue, language, "done", "Updated translation file")


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


def print_progress(progress_queue: Queue[ProgressEvent]) -> None:
    while True:
        event = progress_queue.get()
        if event["kind"] == "shutdown":
            return
        stream = sys.stderr if event["kind"] in {"error", "warning"} else sys.stdout
        print(f"[{event['language']}] {event['message']}", file=stream, flush=True)


def translate_language(
    lang: str, translations_dir: Path, progress_queue: Queue[ProgressEvent]
) -> None:
    emit_progress(progress_queue, lang, "info", "Queued")
    ts_file = translations_dir / f"{lang}.ts"
    if not ts_file.exists():
        emit_progress(progress_queue, lang, "info", "Creating translation file")
        create_new_ts_file(ts_file)
    translate_ts_file(ts_file, progress_queue)


def max_workers_for(languages: list[str]) -> int:
    configured = os.getenv("LLM_SUGGESTION_MAX_WORKERS")
    if configured:
        return max(1, min(len(languages), int(configured)))
    return min(len(languages), 4)


if __name__ == "__main__":
    languages = sys.argv[1:]
    if not languages:
        raise SystemExit("Usage: llm_suggestion.py <lang> [<lang> ...]")

    translations_dir = Path(__file__).parent.parent / "resources" / "translations"
    progress_queue: Queue[ProgressEvent] = Queue()
    printer = Thread(target=print_progress, args=(progress_queue,), daemon=True)
    printer.start()

    exit_code = 0
    try:
        with ThreadPoolExecutor(max_workers=max_workers_for(languages)) as executor:
            futures: dict[Future[None], str] = {
                executor.submit(
                    translate_language, lang, translations_dir, progress_queue
                ): lang
                for lang in languages
            }
            for future in as_completed(futures):
                lang = futures[future]
                try:
                    future.result()
                except Exception as exc:
                    exit_code = 1
                    emit_progress(progress_queue, lang, "error", str(exc))
    finally:
        progress_queue.put({"language": "", "kind": "shutdown", "message": ""})
        printer.join()

    raise SystemExit(exit_code)
