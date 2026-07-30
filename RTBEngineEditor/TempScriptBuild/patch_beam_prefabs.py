from pathlib import Path
import re

base = Path(r"c:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Assets\Prefabs\Player\Gameplay")
files = [
    "Player Knight.prefab",
    "Player Barbarian.prefab",
    "Player Arcanist.prefab",
    "Player Ranger.prefab",
    "Player Rogue.prefab",
]

new_block = """{
                        name = "Special Beam Trail",
                        uuid = "SP3C14L-BEAM-4000-8000-000000000001",
                        position = Vector3(0.00, 1.00, 0.40),
                        components = {
                            {
                                type = "EnergyBeamComponent",
                                radius = 0.28,
                                useVerticalCross = false,
                                coreWidthScale = 0.40,
                                innerWidthScale = 0.72,
                                outerWidthScale = 1.10,
                                taperAmount = 0.55,
                                tipCapScale = 1.15,
                                muzzleFlareScale = 1.75,
                                fadeOutDuration = 0.22,
                                tipBurstCount = 22,
                                emissionStrength = 1.35,
                                glowIntensity = 0.85,
                                beamColor = Color(0.25, 0.78, 1.00, 0.55),
                                coreColor = Color(0.95, 0.98, 1.00, 0.95),
                                visible = false,
                                softEdge = 0.90,
                                uvScrollSpeed = 2.50,
                                uvTilesPerMeter = 0.45,
                                tipParticles = "SP3C14L-BTIP-4000-8000-000000000001/ParticleSystem",
                                muzzleParticles = "SP3C14L-BMZL-4000-8000-000000000001/ParticleSystem"
                            },
                        },
                        children = {
                            {
                                name = "Beam Tip FX",
                                uuid = "SP3C14L-BTIP-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "ParticleSystem",
                                        maxParticles = 64,
                                        emissionRate = 55.00,
                                        emitterShape = "Sphere",
                                        shapeRadius = 0.12,
                                        startLifetime = 0.35,
                                        startSpeed = 1.20,
                                        startSize = 0.18,
                                        endSize = 0.02,
                                        startColor = Color(0.75, 0.95, 1.00, 0.95),
                                        endColor = Color(0.20, 0.55, 1.00, 0.00),
                                        gravity = Vector3(0.00, 0.15, 0.00),
                                        worldSimulation = true,
                                        textureRef = "Assets/VFX/spark_soft.png",
                                        visible = true,
                                        loop = true,
                                        playOnAwake = false,
                                        simulateInEditMode = false,
                                        destroyOwnerWhenFinished = false,
                                        burstCount = 22,
                                        blendMode = "Additive"
                                    },
                                }
                            },
                            {
                                name = "Beam Muzzle FX",
                                uuid = "SP3C14L-BMZL-4000-8000-000000000001",
                                components = {
                                    {
                                        type = "ParticleSystem",
                                        maxParticles = 48,
                                        emissionRate = 40.00,
                                        emitterShape = "Cone",
                                        shapeRadius = 0.06,
                                        coneAngle = 35.00,
                                        startLifetime = 0.28,
                                        startSpeed = 2.40,
                                        startSize = 0.14,
                                        endSize = 0.02,
                                        startColor = Color(0.45, 0.85, 1.00, 0.85),
                                        endColor = Color(0.15, 0.40, 1.00, 0.00),
                                        gravity = Vector3(0.00, 0.00, 0.00),
                                        worldSimulation = true,
                                        textureRef = "Assets/VFX/spark_soft.png",
                                        visible = true,
                                        loop = true,
                                        playOnAwake = false,
                                        simulateInEditMode = false,
                                        destroyOwnerWhenFinished = false,
                                        burstCount = 12,
                                        blendMode = "Additive"
                                    },
                                }
                            },
                        }
                    }"""

pat = re.compile(
    r'\{\s*name = "Special Beam Trail",.*?uvTilesPerMeter = 0\.45\s*\},?\s*\}\s*\}',
    re.S,
)

for name in files:
    path = base / name
    text = path.read_text(encoding="utf-8")
    if "Beam Tip FX" in text:
        print("already", name)
        continue
    if not pat.search(text):
        print("no match", name)
        continue
    path.write_text(pat.sub(new_block, text, count=1), encoding="utf-8")
    print("updated", name)
