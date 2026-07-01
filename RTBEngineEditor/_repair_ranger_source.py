"""Repair Ranger prefab by slicing between Attack Aim Trail and MainCamera."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
PREFABS = ROOT / "Assets" / "Prefabs"
RANGER = PREFABS / "Player Ranger.prefab"
SOURCE = ROOT / "_player_pawn_source.lua"

PLAYER_VISUAL_UUID = "5200AC9B-039A-416E-82AE-FD5FE1754C95"
COMBAT_ANIMS = [
    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
    "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx",
]


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


def read_preview_children_inner(preview_path: Path) -> str:
    text = preview_path.read_text(encoding="utf-8")
    match = re.search(r"children\s*=\s*\{", text)
    if not match:
        raise ValueError(f"No children in {preview_path}")
    children_block, _ = extract_balanced_block(text, match.end() - 1)
    return children_block.strip()[1:-1].strip()


def build_player_visual_child(model_fbx: str, preview_inner_children: str) -> str:
    anim_lines = ",\n                                    ".join(f'"{path}"' for path in COMBAT_ANIMS)
    return f"""                    {{
                        name = "Player",
                        uuid = "{PLAYER_VISUAL_UUID}",
                        scale = Vector3(0.85, 0.85, 0.85),
                        components = {{
                            {{
                                type = "Animator",
                                modelRef = "{model_fbx}",
                                currentClipName = "ThirdPerson.Idle",
                                defaultClip = "Ranged_Bow_Aiming_Idle",
                                speed = 1.00,
                                playing = true,
                                looping = true,
                                additionalModels = {{
                                    {anim_lines}
                                }}
                            }},
                        }},
                        children = {{
                            {preview_inner_children}
                        }}
                    }},"""


def replace_player_visual_block(prefab_body: str, visual_child: str) -> str:
    aim_marker = 'name = "Attack Aim Trail"'
    camera_marker = 'name = "MainCamera"'
    aim_idx = prefab_body.index(aim_marker)
    aim_brace = prefab_body.rfind("{", 0, aim_idx)
    _, aim_end = extract_balanced_block(prefab_body, aim_brace)
    camera_idx = prefab_body.index(camera_marker)
    camera_brace = prefab_body.rfind("{", 0, camera_idx)
    return prefab_body[:aim_end] + "\n" + visual_child + prefab_body[camera_brace:]


def balance_depth(text: str) -> int:
    depth = 0
    for ch in text:
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
    return depth


def main() -> None:
    text = RANGER.read_text(encoding="utf-8")
    return_idx = text.index("return")
    body_start = text.index("{", return_idx)
    body = text[body_start:]

    preview_inner = read_preview_children_inner(PREFABS / "Character Preview Ranger.prefab")
    visual = build_player_visual_child(
        "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
        preview_inner,
    )
    repaired = replace_player_visual_block(body, visual)
    while balance_depth(repaired) > 0:
        repaired += "\n}"
    while balance_depth(repaired) < 0:
        repaired = repaired.rstrip()
        if repaired.endswith("}"):
            repaired = repaired[:-1]

    SOURCE.write_text(repaired, encoding="utf-8")
    RANGER.write_text(f"return {repaired.strip()}\n", encoding="utf-8")
    print("Repaired. Final depth:", balance_depth(repaired))


if __name__ == "__main__":
    main()
