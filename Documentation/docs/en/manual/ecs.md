# ECS

`RTBEngine::ECS::World` is an entity registry with sparse sets. It lives beside the scene. It has no hierarchy and no Inspector.

```cpp
RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
RTBEngine::ECS::Entity entity = world->Create();

world->Add<LocalTransform>(entity, local);
bool alive = world->IsAlive(entity);
LocalTransform* tr = world->TryGet<LocalTransform>(entity);
bool has = world->Has<LocalTransform>(entity);

world->Remove<LocalTransform>(entity);
world->Destroy(entity);
world->Clear();
```

`Add` emplaces and returns a reference to the component inside the storage. `TryGet` returns null if that entity does not have it. `Clear` runs when the scene unloads; registered systems stay.

## When to use it

Use it for many objects of the same type with a short life, where a virtual `OnUpdate` per unit gets expensive. The case the engine already ships is projectile flight:

| ECS piece | Role |
| --- | --- |
| `LocalTransform` | Flat pose, no parent |
| Flight state | Integration in the system |
| Hit query | Sphere cast on the simulation tick |
| `VisualLink` | Which `GameObject` represents the entity |

The scene component still exists. Its `OnUpdate` does not integrate while the entity is alive. `OnLateUpdate` reads the impact and applies damage, particles, and audio. `Tick(Presentation)` copies the pose onto the visual transform.

## Who registers the systems

The engine provides `World`, the scheduler, and the instanced renderer. Game systems (projectiles, banks) are registered from `GameScripts` with `RTBScripts_InitializeEcs`. If you add a system, it goes in the script DLL, not inside a `GameObject`.

## Tick

`Application` calls `Tick(Simulation)` after `Scene::Update` and before the bridge is resolved in `LateUpdate`. `Tick(Presentation)` comes after that, so drawing sees the new pose.

Do not move a character `GameObject` onto an entity to "make it ECS". You lose the prefab, reflection, networking, and Bullet callbacks, and the character was not the bottleneck.
