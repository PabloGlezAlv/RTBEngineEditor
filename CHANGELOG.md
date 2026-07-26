# Changelog — RTBEngineEditor

**Current version:** `0.11.0`

API documentation: [`README.md`](README.md)

**Requirements:** RTBEngine SDK **0.11.0** (generate with `../RTBEngine/BuildSDK.bat`).

---

## [0.11.0] — 2026

### Added
- **Apply & Restart Editor** in Project Settings when the selected Graphics API differs from the active session (persists `GraphicsAPI=` to `.rtbproj`, then relaunches the editor process).
- Custom **`VolumeComponent`** inspector: component enable toggle, zone settings (global / box / priority / weight), and Unity-style per-effect checkboxes for Distance Fog and Volumetric Fog.

### Changed
- Graphics API switching is **editor-only** and uses a clean process relaunch (`CreateProcessA`); the new instance reads the API via `Project::PeekGraphicsAPI` at startup. In-process GPU remount is intentionally not supported.
- Unsaved scene/prefab popup integrates with graphics API restart (Save / Ignore / Cancel); **Play** is stopped automatically before relaunch.

### Fixed
- Project Settings Graphics API UX: no manual close/reopen — one-click apply with automatic editor restart.

## [0.10.0] — 2026

### Added
- **Vulkan** graphics backend support (via engine SDK / RHI).
- **DDGI** lighting option in project settings and **Sponza** demo scene.
- **Shadow settings** UI for project lighting (shadow map resolution).
- **GitHub Actions** release workflow: publishes a runnable editor zip with embedded engine SDK.

### Changed
- Requires engine SDK **0.10.0** (Vulkan RHI + DDGI).
- Directional light visualization in Scene View.

### Fixed
- Editor camera position saved between sessions.

## [0.9.0] — 2026

### Added
- Game-owned ECS bootstrap (`GameEcsBootstrap` → `RTBScripts_InitializeEcs`) registering **projectile** systems from GameScripts.
- **`ProjectileSimulation`** / **`ProjectileComponents`** in `Assets/Scripts/Combat/` (no longer in the engine SDK).
- Swarm perf benchmark in GameScripts: `PerfSwarmBenchmark`, `PerfSwarmAgent`, `SwarmSimulation`.
- Test scenes **`SwarmPerf_OOP`** and **`SwarmPerf_ECS`** (1500 agents; OOP MeshRenderers vs ECS + one instanced `MeshRenderer`).
- Stats overlay: **ECS Entities** and **ECS Sim** (generic engine stats).
- **Source code** inspector for scripts (`.h`/`.cpp`) and shaders (from 0.8.1).

### Changed
- `ProjectileComponent` creates/syncs ECS entities through GameScripts simulation APIs.
- EmberOrb shader for Arcanist (from 0.8.1).
- Requires engine **0.9.0** (`MeshRenderer` instancing + `InitializeGameEcs`).

## [0.8.1]

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
