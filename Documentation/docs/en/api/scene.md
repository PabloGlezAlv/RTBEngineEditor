# Scene and SceneManager

## GetInstance

```cpp
static SceneManager& GetInstance();
```

The engine singleton. There is always one: it does not return a pointer and a script does not create it.

**Returns** the live instance.

## LoadScene

```cpp
bool LoadScene(const std::string& path);
```

Unloads the active scene and loads `path` immediately, inside this call. It clears the `ObjectPool`, fires `onSceneUnloading` / `onSceneLoaded`, and brings the new scene's lifecycle up. An empty path, a file that fails to load, or a call while another transition is already running leaves the previous scene in place when the load fails and returns false. Do not call it from a component's `OnUpdate`: it destroys the scene that is executing you. Queue with `RequestSceneLoad` and let the player apply the change at the frame boundary.

- `path`: scene path, for example `Assets/Scenes/MainMenu.lua`.

**Returns** true if the new scene is active.

## RequestSceneLoad

```cpp
bool RequestSceneLoad(const char* path);
```

Stores the path and loads nothing yet. The application loop calls `ProcessPendingSceneLoad` when it is safe to destroy the current scene. A null or empty path, or a call in the middle of another transition, returns false. If there is no transition, the latest call wins and replaces the pending path.

- `path`: scene path. It must stay alive until it is copied; a literal or a stable buffer is fine.

**Returns** true if the request was queued.

```cpp
RTBEngine::Scene::SceneManager::GetInstance()
    .RequestSceneLoad("Assets/Scenes/MainMenu.lua");
```

## ProcessPendingSceneLoad

```cpp
bool ProcessPendingSceneLoad();
```

If a path is pending, clears it and calls `LoadScene`. The player and the editor do this at the frame boundary. A game script does not need to call it: `RequestSceneLoad` is enough.

**Returns** false if nothing was pending. If something was, it returns whatever `LoadScene` returned.

## ClearPendingSceneLoad

```cpp
void ClearPendingSceneLoad();
```

Drops the pending path without loading. `LoadScene` and `UnloadCurrentScene` also do this when they start.

## UnloadCurrentScene

```cpp
void UnloadCurrentScene();
```

Destroys the active scene, clears the pool, and leaves the manager with no scene. If there is no scene, it does nothing. If a transition is already running, it logs and returns. Do not call it from a component of that scene.

## GetActiveScene

```cpp
Scene* GetActiveScene() const;
```

**Returns** the loaded scene, or null if there is none.

## GetActiveScenePath

```cpp
const std::string& GetActiveScenePath() const;
```

**Returns** the path the active scene was loaded from. Empty if there is no scene.

## HasActiveScene

```cpp
bool HasActiveScene() const;
```

**Returns** true if `GetActiveScene()` is not null.

## Instantiate

```cpp
GameObject* Instantiate(const std::string& name = "GameObject", GameObject* parent = nullptr);
```

Creates an empty `GameObject` in the active scene, with that name and parent. With no active scene it logs an error and returns null. The default name is `"GameObject"`. Keep the pointer: `FindGameObject` returns the first equal name, not “the one you just created” if another one already existed.

- `name`: initial name.
- `parent`: parent, or null for the root.

**Returns** the new object, or null if there is no scene.

## Instantiate

```cpp
GameObject* Instantiate(const Prefab& prefab, GameObject* parent = nullptr, bool regenerateUuids = true);
```

Clones the prefab into the active scene. With no scene it returns null. `regenerateUuids` true generates new UUIDs so a level instance is unique. `false` keeps the asset identities, which is what the editor does when it reopens the prefab for editing.

- `prefab`: asset already loaded in memory.
- `parent`: parent of the instantiated root, or null.
- `regenerateUuids`: true for new identities. The default is true.

**Returns** the instantiated root, or null if there is no scene.

## MarkSceneDirty

```cpp
void MarkSceneDirty();
```

Marks the active scene as modified so the editor can offer to save. Gameplay in the player does not depend on this.

## ClearSceneDirty

```cpp
void ClearSceneDirty();
```

Clears the modified flag. A successful save does this. `LoadScene` also clears it when it finishes.

## IsSceneDirty

```cpp
bool IsSceneDirty() const;
```

**Returns** true if there are unsaved changes. A freshly loaded scene is clean.

## AddGameObject

```cpp
void AddGameObject(GameObject* gameObject, bool queueLifecycle = true);
```

Gives the scene ownership of that object. If the scene has already finished startup and `queueLifecycle` is true, it queues `OnAwake` / `OnEnable` / `OnStart`. If it is called in the middle of a scene iteration, the add waits until the iteration ends. `Instantiate` already does this; use it when you build an object by hand.

- `gameObject`: object the scene will own. Do not delete it yourself afterwards.
- `queueLifecycle`: true to wake the lifecycle. The default is true.

## FindGameObject

```cpp
GameObject* FindGameObject(const std::string& name);
```

Walks the objects the scene owns, and those waiting to be inserted, and returns the first whose name matches. It is not a unique search: if there are two `"Arrow"` objects, you get whichever appears first in that list. After an `Instantiate` with the same name, keep the pointer you were given.

- `name`: exact name.

**Returns** the object, or null if none match.

## FindGameObjectByUUID

```cpp
GameObject* FindGameObjectByUUID(const std::string& uuid);
```

Looks up a UUID in the scene map. An empty UUID returns null. If the object no longer belongs to this scene, it also returns null.

- `uuid`: identity stored in the `.lua` or the prefab.

**Returns** the object, or null.

## GetActiveCamera

```cpp
Rendering::Camera* GetActiveCamera();
```

The render camera of the `CameraComponent` with `isMainCamera`. If none is marked, it uses the first camera in the scene. If there is none, it returns null. The editor Scene View camera does not come from here.

**Returns** the active `Rendering::Camera`, or null.
