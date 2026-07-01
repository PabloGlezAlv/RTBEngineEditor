"""Rebuild clean _player_pawn_source.lua from Ranger prefab prefix + generated visual tail."""

from __future__ import annotations

import re
from pathlib import Path

from _generate_player_prefabs import (
    build_player_visual_child,
    extract_balanced_block,
    read_preview_children_inner,
)
from _rebuild_player_prefab_tail import MAIN_CAMERA_BLOCK, balance_depth

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "_player_pawn_source.lua"
RANGER = ROOT / "Assets" / "Prefabs" / "Player Ranger.prefab"
PREFABS = ROOT / "Assets" / "Prefabs"


def restore_source() -> None:
    text = RANGER.read_text(encoding="utf-8")
    match = re.search(r"return\s*\{", text)
    if not match:
        raise ValueError("Ranger prefab has no return block")

    aim_marker = 'name = "Attack Aim Trail"'
    aim_idx = text.index(aim_marker)
    aim_brace = text.rfind("{", 0, aim_idx)
    _, aim_end = extract_balanced_block(text, aim_brace)
    prefix = text[match.end() - 1 : aim_end].rstrip()
    if not prefix.endswith(","):
        prefix += ","

    prefix = re.sub(
        r"\{\s*(?:\{\s*)+type = \"ProjectileAttackAbility\"",
        '{\n                        type = "ProjectileAttackAbility"',
        prefix,
        count=1,
    )

    preview_inner = read_preview_children_inner(PREFABS / "Character Preview Ranger.prefab")
    visual = build_player_visual_child(
        "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
        preview_inner,
        "ranged",
    )

    rebuilt = prefix + "\n" + visual + "\n" + MAIN_CAMERA_BLOCK + "\n                }\n}"
    SOURCE.write_text(rebuilt + "\n", encoding="utf-8")
    depth = balance_depth(rebuilt)
    print(f"Wrote {SOURCE.name} balance={depth}")
    if depth != 0:
        raise SystemExit(f"Unbalanced braces: {depth}")


if __name__ == "__main__":
    restore_source()
