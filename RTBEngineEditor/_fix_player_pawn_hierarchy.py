"""Normalize player pawn source/prefab hierarchy (MainCamera as root child)."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "_player_pawn_source.lua"
PREFABS = ROOT / "Assets" / "Prefabs"

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
    raise ValueError("Unbalanced braces")


def rebuild_body(body: str) -> str:
    aim_marker = 'name = "Attack Aim Trail"'
    camera_marker = 'name = "MainCamera"'
    player_marker = f'uuid = "5200AC9B-039A-416E-82AE-FD5FE1754C95"'

    aim_idx = body.index(aim_marker)
    aim_brace = body.rfind("{", 0, aim_idx)
    _, aim_end = extract_balanced_block(body, aim_brace)

    player_idx = body.index(player_marker)
    player_brace = body.rfind("{", 0, player_idx)
    player_block, _ = extract_balanced_block(body, player_brace)

    prefix = body[:aim_end]
    suffix = body[body.index(camera_marker) :]
    camera_brace = suffix.index("{")
    _, camera_end = extract_balanced_block(suffix, camera_brace)

    rebuilt = prefix + "\n" + player_block + "\n" + MAIN_CAMERA_BLOCK + "\n                }\n}"
    return rebuilt


def main() -> None:
    body = SOURCE.read_text(encoding="utf-8").strip()
    if body.startswith("return"):
        body = body[body.index("{") :]

    fixed = rebuild_body(body)
    SOURCE.write_text(fixed + "\n", encoding="utf-8")

    for path in sorted(PREFABS.glob("Player *.prefab")):
        text = path.read_text(encoding="utf-8")
        match = re.search(r"return\s*\{", text)
        if not match:
            continue
        inner, _ = extract_balanced_block(text, match.end() - 1)
        inner_fixed = rebuild_body(inner.strip())
        path.write_text(f"return {inner_fixed}\n", encoding="utf-8")
        print(f"Fixed {path.name}")

    print("Fixed _player_pawn_source.lua")


if __name__ == "__main__":
    main()
