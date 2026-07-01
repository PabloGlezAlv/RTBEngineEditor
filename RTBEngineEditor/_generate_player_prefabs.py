#!/usr/bin/env python3
"""Generate Player *.prefab files from DefaultScene Player GO and update DefaultScene."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SCENE = ROOT / "Assets" / "Scenes" / "DefaultScene.lua"
PREFABS = ROOT / "Assets" / "Prefabs"

PLAYER_ROOT_UUID = "E8682E33-50ED-45D8-BC76-B31113639F9E"
PLAYER_VISUAL_UUID = "5200AC9B-039A-416E-82AE-FD5FE1754C95"

COMBAT_ANIMS = {
    "ranged": [
        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx",
    ],
    "melee": [
        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_MovementBasic.fbx",
        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx",
        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatMelee.fbx",
    ],
}

CHARACTERS = [
    {
        "id": "ranger",
        "prefab_name": "Player Ranger",
        "model_fbx": "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Ranger.fbx",
        "preview": "Character Preview Ranger.prefab",
        "ranged": True,
        "projectile_prefab": "Assets/Prefabs/Arrow Projectile.prefab",
    },
    {
        "id": "knight",
        "prefab_name": "Player Knight",
        "model_fbx": "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Knight.fbx",
        "preview": "Character Preview Knight.prefab",
        "ranged": False,
        "projectile_prefab": "",
    },
    {
        "id": "barbarian",
        "prefab_name": "Player Barbarian",
        "model_fbx": "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Barbarian.fbx",
        "preview": "Character Preview Barbarian.prefab",
        "ranged": False,
        "projectile_prefab": "",
    },
    {
        "id": "arcanist",
        "prefab_name": "Player Arcanist",
        "model_fbx": "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Mage.fbx",
        "preview": "Character Preview Arcanist.prefab",
        "ranged": True,
        "projectile_prefab": "Assets/Prefabs/Sphere Projectile.prefab",
    },
    {
        "id": "rogue",
        "prefab_name": "Player Rogue",
        "model_fbx": "Assets/3D/KayKit_Adventurers_2.0_FREE/Characters/fbx/Rogue.fbx",
        "preview": "Character Preview Rogue.prefab",
        "ranged": True,
        "projectile_prefab": "Assets/Prefabs/Sphere Projectile.prefab",
    },
]

def projectile_ability_block(projectile_prefab: str) -> str:
    return f"""                    {{
                        type = "ProjectileAttackAbility",
                        projectilePrefabRef = "{projectile_prefab}",
                        attackOriginHeightOffset = 0.50,
                        launchForwardOffset = 0.35,
                        fireAudio = "C0MBAT-F1RE-4001-8000-000000000001/AudioSourceComponent",
                        hitAudio = "C0MBAT-H1T0-4001-8000-000000000001/AudioSourceComponent"
                    }},"""


MELEE_ABILITY = """                    {
                        type = "PlayerMeleeSweepAttackAbility",
                        attackOriginOffset = Vector3(0.00, 1.05, 0.18),
                        cooldown = 0.00,
                        damage = 12.00,
                        hitDelay = 0.10,
                        recoveryDuration = 1.50,
                        sphereRadius = 0.45,
                        sphereDistance = 1.20,
                        tickInterval = 0.50,
                        knockbackStrength = 2.50,
                        ignoreSameTeam = true,
                        hitAudio = "C0MBAT-H1T0-4001-8000-000000000001/AudioSourceComponent"
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
    raise ValueError("Unbalanced braces while extracting block")


def extract_player_go(scene_text: str) -> str:
    source_path = ROOT / "_player_pawn_source.lua"
    if source_path.exists():
        return source_path.read_text(encoding="utf-8").strip()

    prefab_path = PREFABS / "Player Ranger.prefab"
    if prefab_path.exists():
        text = prefab_path.read_text(encoding="utf-8")
        match = re.search(r"return\s*\{", text)
        if match:
            block, _ = extract_balanced_block(text, match.end() - 1)
            return block

    marker = 'name = "Player GO"'
    idx = scene_text.index(marker)
    brace_start = scene_text.rfind("{", 0, idx)
    block, _ = extract_balanced_block(scene_text, brace_start)
    return block


def read_preview_children_inner(preview_path: Path) -> str:
    text = preview_path.read_text(encoding="utf-8")
    match = re.search(r"children\s*=\s*\{", text)
    if not match:
        raise ValueError(f"No children in preview prefab: {preview_path}")
    children_block, _ = extract_balanced_block(text, match.end() - 1)
    return children_block.strip()[1:-1].strip()


def build_player_visual_child(model_fbx: str, preview_inner_children: str, combat_style: str) -> str:
    anim_lines = ",\n                                    ".join(
        f'"{path}"' for path in COMBAT_ANIMS[combat_style]
    )
    default_clip = "Ranged_Bow_Aiming_Idle" if combat_style == "ranged" else "Melee_2H_Idle"
    return f"""                    {{
                        name = "Player",
                        uuid = "{PLAYER_VISUAL_UUID}",
                        scale = Vector3(0.85, 0.85, 0.85),
                        components = {{
                            {{
                                type = "Animator",
                                modelRef = "{model_fbx}",
                                currentClipName = "ThirdPerson.Idle",
                                defaultClip = "{default_clip}",
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


def sanitize_prefab_body(body: str) -> str:
    body = re.sub(
        r"\n\s+\}\n\s+\},+,?\n(\s+\{\n\s+name = \"MainCamera\")",
        r"\n\1",
        body,
    )
    return body


def ensure_closed_block(body: str) -> str:
    depth = 0
    for ch in body:
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
    if depth > 0:
        body = body.rstrip() + ("\n}" * depth)
    return body


def customize_prefab(base: str, character: dict) -> str:
    body = base
    body = re.sub(
        r'name = "Player (?:GO|Ranger)"',
        f'name = "{character["prefab_name"]}"',
        body,
        count=1,
    )
    body = re.sub(
        r"position = Vector3\([^\)]+\)",
        "position = Vector3(0.00, 0.00, 0.00)",
        body,
        count=1,
    )

    preview_path = PREFABS / character["preview"]
    combat_style = "ranged" if character["ranged"] else "melee"
    preview_inner_children = read_preview_children_inner(preview_path)
    visual_child = build_player_visual_child(
        character["model_fbx"], preview_inner_children, combat_style
    )
    body = replace_player_visual_block(body, visual_child)

    if character["ranged"]:
        body = re.sub(
            r'\{\s*type = "PlayerMeleeSweepAttackAbility"[\s\S]*?\},',
            "",
            body,
        )
        body = re.sub(
            r'\{\s*type = "ProjectileAttackAbility"[\s\S]*?\},',
            projectile_ability_block(character["projectile_prefab"]),
            body,
            count=1,
        )
        body = body.replace(
            f'attackAbility = "{PLAYER_ROOT_UUID}/PlayerMeleeSweepAttackAbility"',
            f'attackAbility = "{PLAYER_ROOT_UUID}/ProjectileAttackAbility"',
        )
    else:
        body = re.sub(
            r'\{\s*type = "ProjectileAttackAbility"[\s\S]*?\},',
            MELEE_ABILITY,
            body,
            count=1,
        )
        body = body.replace(
            f'attackAbility = "{PLAYER_ROOT_UUID}/ProjectileAttackAbility"',
            f'attackAbility = "{PLAYER_ROOT_UUID}/PlayerMeleeSweepAttackAbility"',
        )
        body = re.sub(
            r'aimArrowVisual = "[^"]*"',
            'aimArrowVisual = ""',
            body,
            count=1,
        )

    body = sanitize_prefab_body(body)
    stripped = body.strip()
    if stripped.endswith(","):
        stripped = stripped[:-1]
    return f"return {stripped}\n"


def update_default_scene(scene_text: str, player_go_block: str) -> str:
    if 'name = "Player GO"' not in scene_text:
        return scene_text

    marker = 'name = "Player GO"'
    idx = scene_text.index(marker)
    brace_start = scene_text.rfind("{", 0, idx)
    _, end = extract_balanced_block(scene_text, brace_start)

    spawn_block = """
            {
                name = "PlayerSpawnPoint",
                uuid = "PLSP-AWN0-4000-8000-000000000001",
                collisionLayer = "Characters",
                position = Vector3(0.90, 0.10, 0.00),
                components = {
                    {
                        type = "PlayerPawnSpawner",
                        onlinePlayerManager = "B1A2C3D4-E5F6-7890-ABCD-EF1234567891/OnlinePlayerManager",
                        roundManager = "4A7C26F9-7F62-4A3A-B8F0-91A4B36C1601/RoundManager",
                    },
                },
            },"""

    updated = scene_text[:brace_start] + spawn_block + scene_text[end:]
    if 'name = "Player GO"' in updated:
        updated = updated.replace(
            'playerObject = "E8682E33-50ED-45D8-BC76-B31113639F9E"',
            'playerObject = ""',
        )
        updated = updated.replace(
            'localPlayerObject = "E8682E33-50ED-45D8-BC76-B31113639F9E"',
            'localPlayerObject = ""',
        )
    return updated


def create_sphere_projectile_prefab() -> None:
    arrow = (PREFABS / "Arrow Projectile.prefab").read_text(encoding="utf-8")
    sphere = arrow.replace('name = "Arrow Projectile"', 'name = "Sphere Projectile"')
    sphere = re.sub(
        r'meshRef = "Assets/3D/KayKit_Adventurers_2.0_FREE/Assets/obj/arrow_bow\.obj"',
        'meshRef = "Default/Models/sphere.obj"',
        sphere,
    )
    (PREFABS / "Sphere Projectile.prefab").write_text(sphere, encoding="utf-8")


def main() -> None:
    from _rebuild_player_prefab_tail import balance_depth, rebuild_file

    scene_text = SCENE.read_text(encoding="utf-8")
    player_go = extract_player_go(scene_text)

    if not (PREFABS / "Sphere Projectile.prefab").exists():
        create_sphere_projectile_prefab()

    for character in CHARACTERS:
        prefab_content = customize_prefab(player_go, character)
        out_path = PREFABS / f"{character['prefab_name']}.prefab"
        out_path.write_text(prefab_content, encoding="utf-8")
        rebuild_file(out_path)
        depth = balance_depth(out_path.read_text(encoding="utf-8"))
        print(f"Wrote {out_path.name} balance={depth}")
        if depth != 0:
            raise SystemExit(f"Unbalanced prefab {out_path.name}: {depth}")

    SCENE.write_text(update_default_scene(scene_text, player_go), encoding="utf-8")
    print("Updated DefaultScene.lua (spawn point already present, skipped if absent)")


if __name__ == "__main__":
    main()
