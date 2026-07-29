from pathlib import Path
import re

prefab_dir = Path(r"C:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Assets\Prefabs\Player\Gameplay")

AURA_CHILD = """                    {
                        name = "Special Beam Aura Trail",
                        uuid = "SP3C14L-AURA-4000-8000-000000000001",
                        position = Vector3(0.00, 1.00, 0.40),
                        components = {
                            {
                                type = "TrailRenderer",
                                width = 1.20,
                                color = Color(0.40, 0.88, 1.00, 0.85),
                                fadeAlphaAlongLength = false,
                                visible = false,
                                blendMode = "Additive",
                                alignment = "CameraFacing",
                                softEdge = 0.95,
                                uvScrollSpeed = 1.80,
                                uvTilesPerMeter = 0.45
                            },
                        }
                    },
"""

HALO_CHILD = """                    {
                        name = "Special Beam Halo Trail",
                        uuid = "SP3C14L-HAL0-4000-8000-000000000001",
                        position = Vector3(0.00, 1.00, 0.40),
                        components = {
                            {
                                type = "TrailRenderer",
                                width = 2.20,
                                color = Color(0.20, 0.42, 1.00, 0.55),
                                fadeAlphaAlongLength = false,
                                visible = false,
                                blendMode = "Additive",
                                alignment = "CameraFacing",
                                softEdge = 0.70,
                                uvScrollSpeed = 0.90,
                                uvTilesPerMeter = 0.45
                            },
                        }
                    },
"""

for prefab in sorted(prefab_dir.glob("Player *.prefab")):
    text = prefab.read_text(encoding="utf-8")
    original = text

    # Wire new component refs on PlayerSpecialBeamAttack
    if "beamAuraTrail" not in text:
        text = text.replace(
            'beamTrail = "SP3C14L-BEAM-4000-8000-000000000001/TrailRenderer",\n'
            '                        aimPreviewTrail = "SP3C14L-A1M0-4000-8000-000000000001/TrailRenderer",',
            'beamTrail = "SP3C14L-BEAM-4000-8000-000000000001/TrailRenderer",\n'
            '                        beamAuraTrail = "SP3C14L-AURA-4000-8000-000000000001/TrailRenderer",\n'
            '                        beamHaloTrail = "SP3C14L-HAL0-4000-8000-000000000001/TrailRenderer",\n'
            '                        aimPreviewTrail = "SP3C14L-A1M0-4000-8000-000000000001/TrailRenderer",',
        )

    # Update core beam trail defaults
    text = re.sub(
        r'(name = "Special Beam Trail",[\s\S]*?type = "TrailRenderer",\s*)'
        r'width = 1\.60,\s*'
        r'color = Color\([^)]+\),\s*'
        r'fadeAlphaAlongLength = true,\s*'
        r'visible = false',
        r'\1width = 0.35,\n'
        r'                                color = Color(1.00, 1.00, 0.96, 1.00),\n'
        r'                                fadeAlphaAlongLength = false,\n'
        r'                                visible = false,\n'
        r'                                blendMode = "Additive",\n'
        r'                                alignment = "CameraFacing",\n'
        r'                                softEdge = 1.25,\n'
        r'                                uvScrollSpeed = 3.20,\n'
        r'                                uvTilesPerMeter = 0.45',
        text,
        count=1,
    )

    # Update preview trail defaults
    text = re.sub(
        r'(name = "Special Attack Aim Trail",[\s\S]*?type = "TrailRenderer",\s*)'
        r'width = 1\.10,\s*'
        r'color = Color\([^)]+\),\s*'
        r'fadeAlphaAlongLength = false,\s*'
        r'visible = false',
        r'\1width = 0.90,\n'
        r'                                color = Color(0.88, 0.96, 1.00, 0.40),\n'
        r'                                fadeAlphaAlongLength = false,\n'
        r'                                visible = false,\n'
        r'                                blendMode = "Additive",\n'
        r'                                alignment = "CameraFacing",\n'
        r'                                softEdge = 0.85,\n'
        r'                                uvScrollSpeed = 0.00,\n'
        r'                                uvTilesPerMeter = 0.45',
        text,
        count=1,
    )

    if "Special Beam Aura Trail" not in text:
        # Insert aura+halo after Special Beam Trail child block
        marker = 'name = "Special Beam Trail"'
        idx = text.find(marker)
        if idx < 0:
            print("SKIP no beam trail", prefab.name)
            continue
        # find end of that child object: closing "}," after its components
        # Search for the Avatar child that usually follows
        avatar = text.find('name = "Avatar"', idx)
        insert_at = text.rfind('{', idx, avatar)  # not ideal
        # Better: find the block end by matching after Special Beam Trail section
        # Locate uuid SP3C14L-BEAM then find the closing of that child
        beam_uuid = 'uuid = "SP3C14L-BEAM-4000-8000-000000000001"'
        b = text.find(beam_uuid)
        # from the '{' of the child containing this uuid, find matching end
        # walk backwards to child start
        child_start = text.rfind('{', 0, b)
        # naive: find next occurrence of 'name = "Avatar"' and insert before the preceding '{'
        if avatar > 0:
            # find the '{' that starts Avatar child - insert before it
            # look back for "},\n                    {\n                        name = \"Avatar\""
            pattern = re.search(r'\},\s*\{\s*name = "Avatar"', text[idx:])
            if pattern:
                abs_pos = idx + pattern.start() + 1  # after },
                # insert after "},"
                insert_pos = idx + pattern.start() + 2
                text = text[:insert_pos] + "\n" + AURA_CHILD + HALO_CHILD + text[insert_pos:]
            else:
                print("SKIP insert pattern", prefab.name)
                continue

    if text != original:
        prefab.write_text(text, encoding="utf-8")
        print("updated", prefab.name)
    else:
        print("unchanged", prefab.name)
