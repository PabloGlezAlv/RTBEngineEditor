# ObjectPool

`RTBEngine::Scene::ObjectPool::GetInstance()` reuses prefab instances. `kDefaultMaxPoolSize` is 32: past that many free copies, `Release` destroys instead of storing.

## ResolvePoolKey

```cpp
static std::string ResolvePoolKey(const std::string& prefabRefOrPath);
```

Normalizes a prefab path or name into the key the pool groups copies under. An empty string returns empty. If the registry knows the resolved path, the path as given, or the asset name, it returns that key. Otherwise it returns the path resolved by `ResourceManager`, or the original text if nothing was resolved.

- `prefabRefOrPath`: an `Assets/...prefab` path or a registered name.

**Returns** the key, or empty if the input is empty.

## Acquire

```cpp
GameObject* Acquire(const std::string& prefabPath,
                    const Math::Vector3& position,
                    const Math::Quaternion& rotation);
```

Takes an instance from that prefab's free list or, if none is free, creates one. It writes the local position and rotation and restarts the lifecycle, so `OnEnable` / `OnStart` run again. With no active scene, or with an empty key, it returns null. If the prefab is not in the registry, creation logs a warning and returns null.

Rotation is a quaternion. If you start from Euler angles, build the quaternion in radians.

- `prefabPath`: path or name `ResolvePoolKey` can resolve.
- `position`: local position on hand-out.
- `rotation`: local rotation on hand-out.

**Returns** the instance, or null.

```cpp
auto& pool = RTBEngine::Scene::ObjectPool::GetInstance();
pool.SetMaxPoolSize("Assets/Prefabs/Combat/Projectiles/Arrow.prefab", 64);
RTBEngine::Scene::GameObject* arrow = pool.Acquire(path, pos, rot);
```

## Release

```cpp
void Release(GameObject* instance);
```

Returns the instance to the free list and prepares it for the next `Acquire` (it leaves gameplay). It does not destroy it while the free list still fits under the maximum. A null pointer does nothing. If it is called while the scene is unloading, it returns without touching the object: the scene already owns destruction. If the object is not from the pool, it is removed from the scene. If it was already free, it is not queued again. If the free list is already at the maximum, this copy is destroyed.

- `instance`: object obtained from `Acquire`.

```cpp
pool.Release(arrow);
```

## Prewarm

```cpp
void Prewarm(const std::string& prefabPath, int count);
```

Creates `count` instances (zero position, identity rotation) and releases them immediately, so the first `Acquire` of the match does not pay for the load. An empty path or a `count` less than or equal to 0 does nothing. If an `Acquire` fails halfway, it stops.

- `prefabPath`: prefab to reserve.
- `count`: how many free copies to leave ready.

## ClearUnused

```cpp
void ClearUnused();
```

Destroys only the copies sitting on the free lists and empties those lists. Instances a script still holds from `Acquire` stay alive. If there is no active scene, it forgets the lists and records without walking objects.

## Clear

```cpp
void Clear();
```

Forgets free lists, instance records, and maximum sizes, and puts the default maximum back to `kDefaultMaxPoolSize`. It does not walk the scene destroying objects: after this the pool no longer recognizes those instances. `LoadScene` calls `ClearIfAlive`, which is the unload path, not this method. From gameplay, return a copy with `Release`.

## SetDefaultMaxPoolSize

```cpp
void SetDefaultMaxPoolSize(int maxSize);
```

Maximum free copies for prefabs that have no size of their own. A negative value is stored as 0: `Release` will destroy instead of store.

- `maxSize`: cap. The pool default is 32.

## SetMaxPoolSize

```cpp
void SetMaxPoolSize(const std::string& prefabPath, int maxSize);
```

Free-copy cap for one prefab. The path is resolved with `ResolvePoolKey`; if that comes back empty, nothing changes. A negative `maxSize` is stored as 0.

- `prefabPath`: prefab.
- `maxSize`: how many free copies to keep.

## GetMaxPoolSize

```cpp
int GetMaxPoolSize(const std::string& prefabPath) const;
```

**Returns** that prefab's cap, or the default cap if `SetMaxPoolSize` was never called for its key.

- `prefabPath`: prefab.

## OwnsInstance

```cpp
bool OwnsInstance(GameObject* instance) const;
```

**Returns** true if the pool has a record for that object, whether it is free or checked out. Null returns false.
