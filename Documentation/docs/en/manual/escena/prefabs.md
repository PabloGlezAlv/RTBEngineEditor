# Prefabs

A prefab is a `.prefab` with the same Lua shape as a scene GameObject: root, children, and components. It lives in `Assets/Prefabs/` and is instantiated as many times as you need.

```text
Assets/Prefabs/
  Player/Gameplay/     match pawns
  Player/Preview/      character select
  Enemies/
  Combat/Projectiles/
  Combat/Effects/
```

## Instantiating

```cpp
RTBEngine::Scene::Prefab prefab;
// After the asset is loaded (PrefabRegistry::Load in the engine / editor):
RTBEngine::Scene::GameObject* pawn =
    RTBEngine::Scene::SceneManager::GetInstance().Instantiate(prefab, parent, true);
```

`regenerateUuids = true` gives new identities. That is the right choice when you drop several copies into a level. `false` keeps the file UUIDs: prefab edit mode uses it so the save round-trips.

## Instances in a scene

An instance remembers `GetPrefabName()`. In the level you can change properties without editing the asset:

- A property that differs from the prefab is an **override**.
- **Revert** restores that field from the asset.
- **Apply** writes that field into the `.prefab` and reloads the registry.
- **Revert All** reinstantiates the prefab and keeps the instance name, UUID, parent, and active state.
- **Unlink** stops it being an instance.

The Inspector marks overridden fields in blue. Right-click the label for Revert or Apply on that field. The transform (position, rotation, scale) uses the same menu.

## Editing the asset

Double-clicking the `.prefab` in the Content Browser opens a staging scene. The level stays loaded and is left alone. **Play** is blocked. `Ctrl+S` saves the prefab. **Back to Scene** closes staging.

The session details are in [Prefabs in the editor](../../editor/prefabs.md).

## Pool

Projectiles and effects should not `Instantiate` and destroy on every shot. [Object Pool](object-pool.md) recycles instances of the same prefab.
