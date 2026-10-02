# Prefabs in the editor

Double-click a `.prefab`, or **Open Prefab** in the Inspector, opens a staging scene. The level stays in memory and is not written.

## Session

1. A scene is created that does not become the active scene.
2. The prefab is instantiated **without** regenerating UUIDs, so the file stays stable.
3. Components on that root are awakened.
4. An editor light `__PrefabEditorLight` is added. It is not part of the asset.
5. The Hierarchy selects the root.

**Save** (`Ctrl+S`) captures the root, writes the `.prefab`, and reloads the registry. **Back to Scene** destroys the staging scene and the light. If there are unsaved changes when you open another prefab, the editor asks you to save, discard, or cancel.

## What still works

Hierarchy, Scene View, Inspector, copy and paste, and the gizmo operate on the staging scene. Particles with `simulateInEditMode` and the animator preview advance. Game scripts do not advance. **Play** is disabled.

## Instances in the level

Outside prefab mode, an instance placed in the scene shows overrides. Apply writes the `.prefab` to disk. Revert reads the asset. Revert All reinstantiates and keeps the instance name, UUID, parent, and active state.

## Known limit

`Animator::OnValidate` still creates bones against the active scene, not against staging. Do not rely on bone GameObjects as authored content inside the prefab.
