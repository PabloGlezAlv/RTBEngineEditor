# Object Pool

`RTBEngine::Scene::ObjectPool` recycles instances of a prefab. The default size is `kDefaultMaxPoolSize` (32) per prefab key.

```cpp
auto& pool = RTBEngine::Scene::ObjectPool::GetInstance();

pool.Prewarm("Assets/Prefabs/Combat/Projectiles/Arrow.prefab", 16);

RTBEngine::Scene::GameObject* arrow = pool.Acquire(
    "Assets/Prefabs/Combat/Projectiles/Arrow.prefab",
    origin,
    rotation);

pool.Release(arrow);
```

| Method | Effect |
| --- | --- |
| `Prewarm(path, count)` | Creates instances and leaves them free |
| `Acquire(path, position, rotation)` | Takes a free one, or creates another if there is room |
| `Release(instance)` | Deactivates it and returns it to the free list |
| `SetMaxPoolSize(path, n)` | Cap for that prefab |
| `ClearUnused()` | Destroys the ones that are free |
| `Clear()` | Empties the pool |
| `OwnsInstance` | True if that instance came from the pool |

`ResolvePoolKey` normalizes the prefab path so `Assets/...` and the absolute path do not create two pools.

## Cycle

`Acquire` brings the instance back (it goes through the component startup that applies). `Release` prepares it so it stops simulating and drawing. Do not call `delete` on a pooled object.

When the scene unloads, do not keep pointers to acquired instances. If the pool is cleared, those `GameObject*` die.

## When not to use it

A player character, a light, or a menu canvas does not need a pool. The pool pays off for objects that are born and die many times a second: projectiles, impacts, damage numbers, members of a swarm.

The dense flight of those projectiles can also live in the [ECS](../ecs.md). The pool remains the owner of the visual `GameObject`.
