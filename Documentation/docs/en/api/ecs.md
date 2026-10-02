# ECS

`RTBEngine::ECS::World`. `Add` emplaces into the sparse set of `T`. `T` is a simulation component (POD), not an `RTBEngine::Scene::Component`.

`Application` calls the tick: `Simulation` after `Scene::Update`, `Presentation` after `LateUpdate`. `Clear` on scene unload deletes entities and leaves the systems. Systems are registered from `RTBScripts_InitializeEcs` in `GameScripts`, not from the Inspector.

```cpp
RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
RTBEngine::ECS::Entity e = world->Create();
world->Add<LocalTransform>(e);
LocalTransform* tr = world->TryGet<LocalTransform>(e);
world->Destroy(e);
```

## GetActive

```cpp
static World* GetActive();
```

**Returns** the world the application is ticking, or null if nobody called `SetActive` or the active world was already destroyed.

## SetActive

```cpp
static void SetActive(World* world);
```

Chooses which world `GetActive` and the tick see. Null leaves the engine with no active world: the next `GetActive` is null. Destroying the world that was active clears the pointer on its own.

- `world`: the world, or null.

## Create

```cpp
Entity Create();
```

Reserves an entity. It reuses a free index and bumps the generation, so an `Entity` you stored earlier does not stay alive. The new entity has no components.

**Returns** the handle. Generation starts at 1.

## Destroy

```cpp
void Destroy(Entity entity);
```

If `IsAlive` is false, it does nothing. If the entity is alive, it strips every component, returns the index to the pool, and bumps the generation: the handle you held becomes invalid immediately.

- `entity`: a handle from `Create`. One that was already destroyed is ignored.

## IsAlive

```cpp
bool IsAlive(Entity entity) const;
```

**Returns** true if the index exists and the generation matches. A default handle, or one already passed to `Destroy`, returns false.

- `entity`: handle to test.

## Clear

```cpp
void Clear();
```

Deletes every component, every entity, and the stats. The `SystemScheduler` is left alone: registered systems stay for the next scene. After this, any stored `Entity` fails `IsAlive`.

## Add

```cpp
template<typename T>
T& Add(Entity entity, T component = T{});
```

Inserts `T` on the entity. If you omit the value, it is built as `T{}`. Check `IsAlive` first: a dead handle is not a valid gameplay slot.

- `entity`: a live entity.
- `component`: initial value. The default is `T{}`.

**Returns** a reference to the component stored in the sparse set.

## Remove

```cpp
template<typename T>
void Remove(Entity entity);
```

Removes `T` from the entity if it has one. If there is no `T` storage or the entity does not have it, it does nothing. It does not destroy the entity.

- `entity`: entity.

## Has

```cpp
template<typename T>
bool Has(Entity entity) const;
```

**Returns** true if a `T` storage exists and that entity has one. If nobody has `Add<T>` yet, false.

- `entity`: entity.

## TryGet

```cpp
template<typename T>
T* TryGet(Entity entity);
```

**Returns** the pointer to the component, or null if there is no `T` storage or the entity does not have it. A const overload has the same contract. The pointer is invalid after `Remove<T>`, `Destroy`, or `Clear`.

- `entity`: entity.

## Tick

```cpp
void Tick(SystemPhase phase, float deltaTime);
```

Runs the systems registered for that phase. `Application` calls `Simulation` after `Scene::Update` and `Presentation` after `LateUpdate`. A script does not call it: doing so runs the systems again in the same frame. `deltaTime` is the one the loop passes in; during `Simulation` the world records how long that tick took.

- `phase`: `Simulation` or `Presentation`.
- `deltaTime`: seconds for this frame.
