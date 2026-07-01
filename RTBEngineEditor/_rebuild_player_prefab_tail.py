"""Rebuild player prefab Lua from Attack Aim Trail through file end."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "_player_pawn_source.lua"
PREFABS = ROOT / "Assets" / "Prefabs"
PLAYER_VISUAL_UUID = "5200AC9B-039A-416E-82AE-FD5FE1754C95"

MAIN_CAMERA_BLOCK = """                    {
                        name = "MainCamera",
                        uuid = "0E7CE370-7C0E-45A8-89CA-AD7A3E70C216",
                        position = Vector3(0.00, 9.48, -7.07),
                        rotation = Quaternion.FromEulerAngles(50.00, 0.00, 0.00),
                        components = {
                            {
                                type = "CameraComponent",
                                fov = 45.00,
                                nearClip = 0.10,
                                farClip = 100.00,
                                projectionType = "Perspective",
                                orthographicSize = 3.50,
                                syncWithTransform = true,
                                isMainCamera = true
                            },
                        }
                    },"""


def extract_balanced_block(text: str, start_index: int) -> tuple[str, int]:
    depth = 0
    i = start_index
    while i < len(text):
        ch = text[i]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return text[start_index : i + 1], i + 1
        i += 1
    raise ValueError(f"Unbalanced braces at index {start_index}")


def balance_depth(text: str) -> int:
    depth = 0
    for ch in text:
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
    return depth


def rebuild_body(body: str) -> str:
    aim_marker = 'name = "Attack Aim Trail"'
    player_marker = 'name = "Player",\n                        uuid = "' + PLAYER_VISUAL_UUID + '"'

    aim_idx = body.index(aim_marker)
    aim_brace = body.rfind("{", 0, aim_idx)
    _, aim_end = extract_balanced_block(body, aim_brace)

    prefix = body[:aim_end].rstrip()
    if not prefix.endswith(","):
        prefix += ","

    player_idx = body.index(player_marker)
    player_brace = body.rfind("{", 0, player_idx)
    player_block, _ = extract_balanced_block(body, player_brace)

    player_block = player_block.rstrip()
    if not player_block.endswith(","):
        player_block += ","

    return prefix + "\n" + player_block + "\n" + MAIN_CAMERA_BLOCK + "\n                }\n}"


def rebuild_file(path: Path) -> None:
    text = path.read_text(encoding="utf-8")
    if text.lstrip().startswith("return"):
        match = re.search(r"return\s*\{", text)
        if not match:
            raise ValueError(f"No return block in {path}")
        prefix = text[: match.end() - 1]
        body = text[match.end() - 1 :]
        rebuilt = rebuild_body(body)
        path.write_text(prefix + rebuilt + "\n", encoding="utf-8")
    else:
        path.write_text(rebuild_body(text) + "\n", encoding="utf-8")


def main() -> None:
    for path in [SOURCE, *sorted(PREFABS.glob("Player *.prefab"))]:
        rebuild_file(path)
        depth = balance_depth(path.read_text(encoding="utf-8"))
        print(f"{path.name}: balance={depth}")


if __name__ == "__main__":
    main()
