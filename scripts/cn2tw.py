from pathlib import Path
from xml.dom import minidom

from opencc import OpenCC


def convert_ts_file(src_file, dest_file):
    cc = OpenCC("s2tw")
    document = minidom.parse(src_file.open())
    ts_element = document.getElementsByTagName("TS")[0]
    ts_element.setAttribute("language", "zh_TW")
    for message in document.getElementsByTagName("message"):
        translation = message.getElementsByTagName("translation")[0]
        source = translation.firstChild.data
        if source == "简体中文":
            source = "繁体中文"
        translation.firstChild.data = cc.convert(source)
    with open(dest_file, "wb") as f:
        f.write(document.toprettyxml(indent="", newl="", encoding="utf-8"))


if __name__ == "__main__":
    zh_cn_ts = Path(".").parent.parent / "resources" / "translations" / "zh_CN.ts"
    zh_tw_ts = zh_cn_ts.with_name("zh_TW.ts")
    convert_ts_file(zh_cn_ts, zh_tw_ts)
