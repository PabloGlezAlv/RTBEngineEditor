# GameObject

`RTBEngine::Scene::GameObject`. Copy is deleted. From a script, read the name with `GetNameCStr`: a `std::string` does not cross the DLL boundary.

## GetNameCStr

```cpp
const char* GetNameCStr() const;
```

Returns the object name as a `const char*`. This is the safe read from `GameScripts.dll`, because the internal `std::string` lives on the engine heap. The pointer stays valid until the next `SetName` or until the object is destroyed.

**Returns** the name, never null: an object without a custom name still has the default `"GameObject"`.

```cpp
const char* name = GetOwner()->GetNameCStr();
```

## GetName

```cpp
const std::string& GetName() const;
```

Returns a reference to the name `std::string`. Use it inside the engine. From a game script use `GetNameCStr`: returning or destroying that `std::string` on the other side of the DLL mixes heaps.

**Returns** the name.

## SetName

```cpp
void SetName(const std::string& name);
```

Replaces the name. `FindGameObject` searches this text and returns the first match, so two objects can share a name.

- `name`: new name. An empty string stores an empty name.

## GetUUID

```cpp
const std::string& GetUUID() const;
```

Stable identity of the object in the scene and in prefabs. `.lua` references point here, not at the name.

**Returns** the UUID. It can be empty on a freshly built object that has not gone through `GenerateNewUUID` or the loader.

## SetUUID

```cpp
void SetUUID(const std::string& id);
```

Writes the UUID. The loader and prefabs use it when instantiating. Changing it during Play breaks references that already pointed at the previous value.

- `id`: new UUID.

## GenerateNewUUID

```cpp
static std::string GenerateNewUUID();
```

Creates a new UUID and does not assign it to any object. Prefab `Instantiate` with `regenerateUuids` true uses it so each copy has its own identity.

**Returns** the generated UUID.

## AddComponent

```cpp
template <typename T>
T* AddComponent();
```

Creates a `T` component through the registered factory (`T::StaticTypeName()` and `RTB_REGISTER_COMPONENT`) and adopts it. The module that registered the type is the one that later destroys it. `OnAwake` runs when the object's lifecycle is already going.

- `T`: a type derived from `Component` that is registered.

**Returns** the new component, or null if the type name is empty, there is no `TypeInfo`, or the factory did not create the object.

## AddComponentOfType

```cpp
Component* AddComponentOfType(const char* typeName);
```

Same as `AddComponent`, picking the type by its registry name. Use it when the type is known as text (`"Spinner"`).

- `typeName`: name from `GetTypeName` / `StaticTypeName`. Null or an empty string returns null and logs an error.

**Returns** the component, or null if it is not registered or has no factory.

## GetComponent

```cpp
template <typename T>
T* GetComponent();
```

Finds on this object the first component that is `T` (or a derived class `dynamic_cast` accepts). If `T` publishes `TypeId()`, the type-id cache is checked first.

**Returns** the component, or null if this object does not have one.

```cpp
auto* body = GetOwner()->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
```

## HasComponent

```cpp
template <typename T>
bool HasComponent();
```

**Returns** true if `GetComponent<T>()` is not null.

## GetComponentInChildren

```cpp
template <typename T>
T* GetComponentInChildren(int maxDepth = -1);
```

Looks for `T` on this object and, if missing, on children. `maxDepth` limits how many levels to walk: `-1` walks the whole subtree.

- `maxDepth`: maximum depth. `-1` does not cut the walk.

**Returns** the first component found, or null.

## RemoveComponent

```cpp
void RemoveComponent(Component* component);
```

Removes that component from this object and destroys it with the `TypeInfo` that created it, so `OnDisable` runs if it was active and then `OnDestroy`. A null pointer, a component that does not belong to this object, or a call while the `GameObject` is already being destroyed does nothing. If it is called in the middle of a component iteration, destruction waits until the iteration ends.

- `component`: a component on this object.

## GetComponents

```cpp
const std::vector<ComponentPtr>& GetComponents() const;
```

The internal component list. From `GameScripts` do not walk that `vector` of `unique_ptr`: the STL container does not cross the DLL. Use `GetComponentCount` and `GetComponentAt`.

**Returns** the list owned by the engine.

## GetComponentCount

```cpp
std::size_t GetComponentCount() const;
```

**Returns** how many components the object has, including disabled ones.

## GetComponentAt

```cpp
Component* GetComponentAt(std::size_t index) const;
```

Component at that list position. The order is insertion order, not a type order.

- `index`: index from 0. Out of range is not a valid access: check `GetComponentCount` first.

**Returns** the component at that index.

## SetParent

```cpp
void SetParent(GameObject* newParent);
```

Changes the parent. Null puts the object at the scene root. If the parent is already the same, nothing happens. Components get `OnParentChanged`, the world matrix and hierarchy-active cache are invalidated, and if the lifecycle has started, `OnEnable` / `OnDisable` are synced on this object and its children.

- `newParent`: new parent, or null for the root.

## GetParent

```cpp
GameObject* GetParent() const;
```

**Returns** the parent, or null if the object is at the root.

## AddChild

```cpp
void AddChild(GameObject* child);
```

Adds `child` to the child list if it is not already there. It does not set the child's `parent`: the full path is `SetParent`. A null `child` does nothing. It bumps the hierarchy version that caches, among other things, a `MeshRenderer`'s `Animator`.

- `child`: child to register.

## RemoveChild

```cpp
void RemoveChild(GameObject* child);
```

Removes `child` from the child list if it was there. It does not by itself set the child's `parent` to null. A null `child` does nothing.

- `child`: child to remove.

## GetChildren

```cpp
const std::vector<GameObject*>& GetChildren() const;
```

Direct children. From a script use `GetChildCount` and `GetChildAt` so the `vector` does not cross the DLL.

**Returns** the direct children.

## GetChildCount

```cpp
std::size_t GetChildCount() const;
```

**Returns** the number of direct children.

## GetChildAt

```cpp
GameObject* GetChildAt(std::size_t index) const;
```

Direct child at that index. Check `GetChildCount` first.

- `index`: index from 0.

**Returns** the child.

## SetActive

```cpp
void SetActive(bool active);
```

Activates or deactivates this object. If the value does not change, nothing happens. Once the lifecycle has started, the state is pushed to components on this object and its children: those that become active receive `OnEnable` (and `OnStart` if it has not run) and those that become inactive receive `OnDisable`. A child with `SetActive(true)` stays inactive in the hierarchy if a parent is inactive.

- `active`: true to turn the object on.

```cpp
GetOwner()->SetActive(true);
```

## IsActive

```cpp
bool IsActive() const;
```

Reads only this object's flag. An inactive parent does not clear it.

**Returns** true if this object is marked active.

## IsActiveInHierarchy

```cpp
bool IsActiveInHierarchy() const;
```

True only when this object and every parent are active. The engine uses this to call `OnEnable` and the ticks.

**Returns** true if the chain up to the root is active.

## GetTransform

```cpp
Transform& GetTransform();
const Transform& GetTransform() const;
```

The local pose. It always exists: a `GameObject` is never without a transform. World pose is `GetWorldPosition`, `GetWorldRotation`, `GetWorldScale`, and `GetWorldMatrix`.

**Returns** the local transform.

## GetWorldMatrix

```cpp
Math::Matrix4 GetWorldMatrix() const;
```

World matrix, the parent matrix times the local matrix. It is rebuilt when the local pose or an ancestor's pose changed.

**Returns** the world matrix. With no parent it matches `Transform::GetModelMatrix`.

## GetWorldPosition

```cpp
Math::Vector3 GetWorldPosition() const;
```

World position. With no parent it is the local position.

**Returns** the world position.

## GetWorldRotation

```cpp
Math::Quaternion GetWorldRotation() const;
```

World rotation: the parent's rotation composed with the local one. With no parent it is the local rotation.

**Returns** the world rotation.

## GetWorldScale

```cpp
Math::Vector3 GetWorldScale() const;
```

World scale, component-wise product with the parent's scale. With no parent it is the local scale.

**Returns** the world scale.

## GetOwningScene

```cpp
Scene* GetOwningScene() const;
```

Scene that owns this object. Null if it has not been added to a scene yet or it was already released.

**Returns** the owning scene, or null.

## SetCollisionLayer

```cpp
void SetCollisionLayer(int layerIndex);
```

Assigns the physics layer. The index is clamped to `[0, kMaxPhysicsLayers)`. The layer matrix decides what it collides with.

- `layerIndex`: layer index. Out-of-range values are clamped.

## SetCollisionLayerByName

```cpp
void SetCollisionLayerByName(const std::string& layerName);
```

Resolves the name in `PhysicsLayerSettings` and calls `SetCollisionLayer`. An unknown name ends at whatever index `GetLayerIndex` returns (usually 0 when the name is missing).

- `layerName`: project layer name.

## GetCollisionLayer

```cpp
int GetCollisionLayer() const;
```

**Returns** the current layer index. The default is 0.

## SetStatic

```cpp
void SetStatic(bool enabled);
```

Static-flag shortcut. `true` sets `Batching`, `ContributeGI`, and `Navigation`. `false` clears every flag. It does not set `Occluder`: that flag on every static mesh used to drive camera fade and broke locomotion. Fade uses `Occludable`.

- `enabled`: true for the default static set.

## SetStaticFlags

```cpp
void SetStaticFlags(StaticFlags flags);
```

Replaces the whole flag set at once. Writing does not inherit: inheritance is what `HasStaticFlag` and `IsStatic` query.

- `flags`: full mask. `StaticFlags::None` clears the object's own flags.

## IsPrefabInstance

```cpp
bool IsPrefabInstance() const;
```

**Returns** true if the object has a prefab name, meaning it was spawned from a `.prefab` asset. An empty `GameObject` returns false.

## GetPrefabName

```cpp
const std::string& GetPrefabName() const;
```

Name of the prefab asset this instance came from. Empty if it is not an instance.

**Returns** the prefab name, or an empty string.

## SetTransient

```cpp
void SetTransient(bool t);
```

Marks the object as transient. The editor uses this for nodes that must not be saved in the scene (preview bones, helpers). Normal gameplay leaves it false.

- `t`: true if it should not persist in the `.lua`.

## IsTransient

```cpp
bool IsTransient() const;
```

**Returns** true if the object is transient. The default is false.

## SetAnimatorBone

```cpp
void SetAnimatorBone(bool value);
```

Marks the object as a bone created by an `Animator`. The editor treats it differently from a gameplay object.

- `value`: true if it is a bone.

## IsAnimatorBone

```cpp
bool IsAnimatorBone() const;
```

**Returns** true if `SetAnimatorBone(true)` was applied. The default is false.
