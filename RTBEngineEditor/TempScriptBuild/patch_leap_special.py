from pathlib import Path
import re

base = Path(r"c:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Assets\Prefabs\Player\Gameplay")
files = ["Player Knight.prefab", "Player Barbarian.prefab"]

old = re.compile(
    r'\{\s*type = "PlayerSpecialBeamAttack",.*?beamAssetRef = "Assets/Data/VFX/KamehamehaBeam\.rtbasset",\s*\},',
    re.S,
)

new = """{
                        type = "PlayerSpecialLeapAttack",
                        jumpPathPreviewTrail = "SP3C14L-A1M0-4000-8000-000000000001/TrailRenderer",
                        impactPreviewTrail = "SP3C14L-AURA-4000-8000-000000000001/TrailRenderer",
                        maxRange = 7.00,
                        impactRadius = 2.25,
                        leapDuration = 0.55,
                        leapHeight = 2.60,
                        damage = 28.00,
                        stunDuration = 1.35,
                        knockbackStrength = 4.00,
                        castRadius = 0.45,
                        trailForwardOffset = 0.15,
                        trailHeightOffset = 0.05,
                        impactHeightOffset = 0.90,
                        ignoreSameTeam = true
                    },"""

for name in files:
    path = base / name
    text = path.read_text(encoding="utf-8")
    if "PlayerSpecialLeapAttack" in text:
        print("already", name)
        continue
    if not old.search(text):
        print("no match", name)
        continue
    path.write_text(old.sub(new, text, count=1), encoding="utf-8")
    print("updated", name)

enemy = Path(r"c:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Assets\Prefabs\Enemies\Enemy Melee.prefab")
etext = enemy.read_text(encoding="utf-8")
if 'type = "StunReceiver"' not in etext:
    etext = etext.replace(
        '{\n            type = "HealthComponent",',
        '{\n            type = "StunReceiver"\n        },\n        {\n            type = "HealthComponent",',
        1,
    )
    enemy.write_text(etext, encoding="utf-8")
    print("updated Enemy Melee")
else:
    print("enemy already has stun")

scene = Path(r"c:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Assets\Scenes\DefaultScene.lua")
stext = scene.read_text(encoding="utf-8")
count = stext.count('prefab = "Player Ranger"')
stext2 = stext.replace('prefab = "Player Ranger"', 'prefab = "Player Knight"')
if stext2 != stext:
    scene.write_text(stext2, encoding="utf-8")
    print(f"DefaultScene: replaced Player Ranger -> Knight ({count} occurrences)")
else:
    print("DefaultScene: no Ranger prefab ref found or already Knight")

default_copy = Path(r"c:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Default\Scenes\DefaultScene.lua")
if default_copy.exists():
    dtext = default_copy.read_text(encoding="utf-8")
    dtext2 = dtext.replace('prefab = "Player Ranger"', 'prefab = "Player Knight"')
    if dtext2 != dtext:
        default_copy.write_text(dtext2, encoding="utf-8")
        print("Default/Scenes copy updated")
