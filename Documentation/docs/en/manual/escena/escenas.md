# Scenes

A scene is a **Lua** file in `Assets/Scenes/`. It describes GameObjects, components, and properties. It is not a gameplay script: gameplay lives in `GameScripts.dll`.

`SceneManager` is a singleton.

```cpp
auto& scenes = RTBEngine::Scene::SceneManager::GetInstance();
scenes.LoadScene("Assets/Scenes/DefaultScene.lua");
scenes.RequestSceneLoad("Assets/Scenes/MainMenu.lua");
bool swapped = scenes.ProcessPendingSceneLoad();
scenes.UnloadCurrentScene();

RTBEngine::Scene::Scene* active = scenes.GetActiveScene();
const std::string& path = scenes.GetActiveScenePath();
```

| Method | When to use it |
| --- | --- |
| `LoadScene` | Outside update, when you can switch immediately |
| `RequestSceneLoad` | From `OnStart`, `OnUpdate`, or a button |
| `ProcessPendingSceneLoad` | Called by `Application` at the start of the frame |
| `MarkSceneDirty` | The editor shows `*` and `Ctrl+S` writes the `.lua` |

## What happens on load

1. Each `GameObject` is created and its components are added. `OnAwake` runs there. Reflected properties are still the constructor defaults.
2. Lua properties are applied and `OnValidate` is called.
3. References to other GameObjects are resolved by UUID.
4. On the first tick, `OnStart`.

That is why reading `speedRef` or a `GameObject*` in `OnAwake` gives the default value or null. Read them in `OnStart` or in `OnValidate`.

## Active camera

```cpp
RTBEngine::Rendering::Camera* camera = scene->GetActiveCamera();
```

It comes from the `CameraComponent` marked `isMainCamera`. The Game View and the player use that camera. The Scene View uses a different one, the editor camera, and does not write into the component.

## Saving

The editor serializes the hierarchy, the transform, and properties marked with `RTB_PROPERTY` / `RTB_PROPERTY_*`. A field that is not inside the `RTB_REGISTER_COMPONENT` block does not enter the file or the Inspector.

Objects with `IsTransient()` and animator bones are not saved as authored content.

## Skybox

The scene can enable a skybox and assign a `.cubemap` (six faces: +X, −X, +Y, −Y, +Z, −Z). In the editor it lives in the **Scene Settings** dropdown on the hierarchy. The Inspector for a `.cubemap` edits the six paths.

## Several scenes in the sample game

| Scene | Role |
| --- | --- |
| `MainMenu.lua` | Menu. Play, multiplayer, quit |
| `MultiplayerMenu.lua` | Choose LAN or Online |
| `LobbyScene.lua` | Create or join a lobby |
| `DefaultScene.lua` | Match |

The flow is in [Online](../../online/index.md).
