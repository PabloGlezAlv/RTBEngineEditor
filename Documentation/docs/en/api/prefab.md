# Prefab

A `RTBEngine::Scene::Prefab` is the asset in memory. The file on disk is `.prefab`. Gameplay does not call Apply or Revert: that is the editor's `PrefabOverrideOps`. In Play the instance is already a `GameObject` with the values applied.

## Instantiate

```cpp
GameObject* SceneManager::Instantiate(const Prefab& prefab,
                                      GameObject* parent = nullptr,
                                      bool regenerateUuids = true);
```

Clones the prefab into the active scene and returns the root. With no active scene it logs an error and returns null. A null parent leaves the root at the scene root.

`regenerateUuids` true (the default) generates new UUIDs. Use that when dropping a copy into a level: otherwise two instances would share an identity and `.lua` references could not tell them apart. `false` keeps the asset UUIDs. The editor passes that when it reopens the prefab for editing, so saved identities stay put.

- `prefab`: loaded asset.
- `parent`: parent of the root, or null.
- `regenerateUuids`: true for new identities.

**Returns** the root, or null if there is no scene.

## IsPrefabInstance

```cpp
bool GameObject::IsPrefabInstance() const;
```

**Returns** true when the object came from a `.prefab` (it has a prefab name). An object created with `Instantiate("Name")` returns false.

## GetPrefabName

```cpp
const std::string& GameObject::GetPrefabName() const;
```

Name of the asset this instance came from. Empty when `IsPrefabInstance` is false.

**Returns** the prefab name, or an empty string.

The registry (`PrefabRegistry`) is what the editor and the pool use to load by path. From a script, repeated copies come from the [ObjectPool](objectpool.md) with an `Assets/...` path.
