# Changelog — RTBEngineEditor

**Current version:** `0.8.1` (2 commits ahead of tag `0.8.0`)

API documentation: [`README.md`](README.md)

**Requirements:** RTBEngine SDK **0.8.0+** (generate with `../RTBEngine/BuildSDK.bat`).

---

## [0.8.1] — 2026 (development, untagged)

### Added
- **Source code** inspector for scripts (`.h`/`.cpp`) and shaders.

### Changed
- EmberOrb shader for Arcanist.

## [0.8.0]

### Added
- Custom **magic orb** shader for Arcanist.
- Shader swapping and `.rtbshader` assets in Inspector.
- Selected character sync in online matches.
- **Prefab Edit Mode**: isolated `.prefab` editing with staging scene.
- Per-character prefabs (Gameplay / Preview); main menu character selection.
- **DataAsset** inspector; folder-based asset organization.
- Arrow trail, impact VFX, ammo system, damage numbers, attack sounds.

### Changed
- Engine gameplay feature integration (pool, combat authority, overlap, countdown, stat receiver).
- Spectator camera on death in online; synced combat, rounds, and nameplates.

## [0.7.0]

- Optional panels (Online, Physics Layers, Navigation Debug) with persistence.
- Enemy navigation and debug overlay in Scene View.
- Collider autosize from mesh; copy objects between scenes.
- Search bars in Hierarchy and Inspector; animation preview.
- ParticleSystem support in Inspector.

## [0.6.0]

- Notification system and match exit flow.
- Refined Online panel; in-game LAN/Relay selection.
- Online sync: spawns, deaths, enemy animations, projectiles.
- Health bar UI; timed revive; player names in lobby.

## [0.5.0]

- **Physics Layers** panel (layer configuration).
- Online gameplay: player input, host authority.
- Per-character ability architecture; trail renderer in scene.
- Pause menu; attack joystick; top-down camera.

## [0.4.0]

- Physics in editor Play mode.
- Third-person character with animations.
- Health system; enemy AI.
- UI menus (MainMenu, game scenes).

## [0.3.1]

- Third-person character with locomotion animations.
- Animation packs; auto-assign clips.

## [0.3.0]

- **ButtonStyle** for UI button styling.
- Latent actions in UI scripts; default scene on open.

## [0.2.0]

- Character prefab; drag-and-drop prefabs.
- Object copy/paste; collider visualization.
- Automatic script compilation; open Visual Studio.

## [0.1.0]

- **Play / Stop / Pause** buttons.
- ImGui docking, Hierarchy, Inspector, Scene View, Game View.
- Content Browser, gizmos (ImGuizmo), grid, build system.
- Component reflection in Inspector; scene saving.

---

## Additional dependencies

| Library | Version |
|---------|---------|
| ImGuizmo | 1.83 |
| ImGui (editor UI) | 1.92.6 WIP |

---

Format: [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) · [Semantic Versioning](https://semver.org/)
