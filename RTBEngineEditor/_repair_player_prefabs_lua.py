"""Fix Player prefab Lua syntax: commas between children, MainCamera tail, balanced braces."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "_player_pawn_source.lua"
PREFABS = ROOT / "Assets" / "Prefabs"


def balance_depth(text: str) -> int:
    depth = 0
    for ch in text:
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
    return depth


def fix_child_commas(text: str) -> str:
    # Lua 5.1 requires ',' or ';' between table entries in children arrays.
    text = re.sub(
        r"(\n\s+\})\n(\s+\{\n\s+name = \"Player\")",
        r"\1,\n\2",
        text,
        count=1,
    )
    text = re.sub(
        r"(\n\s+\})\n(\s+\{\n\s+name = \"MainCamera\")",
        r"\1,\n\2",
        text,
        count=1,
    )
    text = re.sub(
        r"\n\s+\}\n\s+\},\{",
        ",\n                    {",
        text,
        count=1,
    )
    text = re.sub(
        r"\n\s+\}\n\s+\},\n\s+\{\n\s+name = \"MainCamera\"",
        ",\n                    {\n                        name = \"MainCamera\"",
        text,
        count=1,
    )
    return text


def trim_extra_closes(text: str) -> str:
    stripped = text.rstrip()
    while balance_depth(stripped) < 0:
        if stripped.endswith("}"):
            stripped = stripped[:-1].rstrip()
        else:
            break
    while balance_depth(stripped) > 0:
        stripped += "\n}"
    return stripped + "\n"


def fix_file(path: Path) -> None:
    text = path.read_text(encoding="utf-8")
    if text.startswith("return"):
        match = re.search(r"return\s*\{", text)
        if not match:
            raise ValueError(f"Invalid return block in {path}")
        prefix = text[: match.end() - 1]
        body = text[match.end() - 1 :]
        body = fix_child_commas(body)
        body = trim_extra_closes(body)
        path.write_text(prefix + body, encoding="utf-8")
    else:
        body = fix_child_commas(text)
        body = trim_extra_closes(body)
        path.write_text(body, encoding="utf-8")


def main() -> None:
    fix_file(SOURCE)
    for path in sorted(PREFABS.glob("Player *.prefab")):
        fix_file(path)
        depth = balance_depth(path.read_text(encoding="utf-8"))
        print(f"{path.name}: balance={depth}")


if __name__ == "__main__":
    main()
