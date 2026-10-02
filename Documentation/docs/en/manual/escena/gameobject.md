# GameObject

`RTBEngine::Scene::GameObject` is the unit of the hierarchy. It has a name, a UUID, a `Transform`, a list of components, and an optional parent.

Do not copy the object. The copy constructor is deleted. Create it through the scene or through `SceneManager::Instantiate`.

## Creating and finding

```cpp
auto& scenes = RTBEngine::Scene::SceneManager::GetInstance();
RTBEngine::Scene::GameObject* cube = scenes.Instantiate("Cube");
cube->SetName("Caja");

RTBEngine::Scene::Scene* scene = scenes.GetActiveScene();
RTBEngine::Scene::GameObject* found = scene->FindGameObject("Caja");
RTBEngine::Scene::GameObject* byId = scene->FindGameObjectByUUID(cube->GetUUID());
```

From a script, the name that crosses the DLL is `GetNameCStr()`, not `GetName()` by value. The details are in [DLL boundary](../scripting/frontera-dll.md).

## Hierarchy

```cpp
weapon->SetParent(player);
RTBEngine::Scene::GameObject* parent = weapon->GetParent();
const std::vector<RTBEngine::Scene::GameObject*>& children = player->GetChildren();
```

`SetParent` rejects cycles: a parent cannot become a child of its own descendant. The editor makes the same check when you drop a node onto the hierarchy.

`GetWorldPosition`, `GetWorldRotation`, `GetWorldScale`, and `GetWorldMatrix` compose the parent chain. The `Transform` stores the **local** pose.

## Active

```cpp
go->SetActive(false);
bool self = go->IsActive();
bool visible = go->IsActiveInHierarchy();
```

`IsActive` is the local flag. `IsActiveInHierarchy` is false if this object or any parent is inactive. Deactivating propagates `OnDisable` to the components. Activating again calls `OnEnable` and, if it has not run yet, `OnStart`.

## Components

```cpp
auto* body = go->AddComponent<RTBEngine::Scene::RigidBodyComponent>();
auto* same = go->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
bool has = go->HasComponent<RTBEngine::Scene::MeshRenderer>();
auto* childCam = go->GetComponentInChildren<RTBEngine::Scene::CameraComponent>();

go->RemoveComponent(body);
```

`AddComponentOfType("Spinner")` creates a type registered by name. That is what the Inspector **Add Component** menu uses. The type has to be registered with `RTB_REGISTER_COMPONENT` inside `GameScripts.dll`.

## Layers, static flags, and prefabs

| Method | Use |
| --- | --- |
| `SetCollisionLayer` / `SetCollisionLayerByName` | Layer in the collision matrix |
| `SetStatic` / `SetStaticFlags` | Static-object flags (GI contribution, and similar) |
| `IsPrefabInstance` / `GetPrefabName` | The instance came from a `.prefab` |
| `IsTransient` | Not serialized with the scene |
| `GetOwningScene` | Scene that owns the object |

Bones created by the `Animator` are marked with `SetAnimatorBone`. The editor treats them differently from the rest of the hierarchy.

## Identity

`GenerateNewUUID()` creates a new id. References between objects in the scene are stored by UUID and resolved once every object exists. That is why a reflected `GameObject*` is null in `OnAwake` and valid in `OnStart`.
