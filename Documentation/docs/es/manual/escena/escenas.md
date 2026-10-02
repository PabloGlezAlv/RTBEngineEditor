# Escenas

Una escena es un archivo **Lua** en `Assets/Scenes/`. Describe GameObjects, componentes y propiedades. No es un script de gameplay: el gameplay está en `GameScripts.dll`.

`SceneManager` es un singleton.

```cpp
auto& scenes = RTBEngine::Scene::SceneManager::GetInstance();
scenes.LoadScene("Assets/Scenes/DefaultScene.lua");
scenes.RequestSceneLoad("Assets/Scenes/MainMenu.lua");
bool swapped = scenes.ProcessPendingSceneLoad();
scenes.UnloadCurrentScene();

RTBEngine::Scene::Scene* active = scenes.GetActiveScene();
const std::string& path = scenes.GetActiveScenePath();
```

| Método | Cuándo usarlo |
| --- | --- |
| `LoadScene` | Fuera del update, cuando puedes cambiar ya |
| `RequestSceneLoad` | Desde `OnStart`, `OnUpdate` o un botón |
| `ProcessPendingSceneLoad` | Lo llama `Application` al principio del frame |
| `MarkSceneDirty` | El editor muestra `*` y `Ctrl+S` escribe el `.lua` |

## Qué pasa al cargar

1. Se crea cada `GameObject` y se le añaden componentes. Ahí corre `OnAwake`. Las propiedades reflejadas todavía son los valores por defecto del constructor.
2. Se aplican las propiedades del Lua y se llama `OnValidate`.
3. Se resuelven las referencias a otros GameObjects por UUID.
4. En el primer tick, `OnStart`.

Por eso leer `speedRef` o un `GameObject*` en `OnAwake` da el valor por defecto o null. Léelos en `OnStart` o en `OnValidate`.

## Cámara activa

```cpp
RTBEngine::Rendering::Camera* camera = scene->GetActiveCamera();
```

Sale del `CameraComponent` marcado como `isMainCamera`. La Game View y el player usan esa cámara. La Scene View usa otra, la del editor, y no escribe en el componente.

## Guardar

El editor serializa la jerarquía, el transform y las propiedades con `RTB_PROPERTY` / `RTB_PROPERTY_*`. Un campo que no esté en el bloque `RTB_REGISTER_COMPONENT` no entra en el archivo ni en el Inspector.

Los objetos `IsTransient()` y los huesos del animator no se guardan como contenido de autor.

## Skybox

La escena puede activar un skybox y asignarle un `.cubemap` (seis caras: +X, −X, +Y, −Y, +Z, −Z). En el editor está en el desplegable **Scene Settings** de la jerarquía. El Inspector de un `.cubemap` edita las seis rutas.

## Varias escenas en el juego de ejemplo

| Escena | Rol |
| --- | --- |
| `MainMenu.lua` | Menú. Play, multiplayer, salir |
| `MultiplayerMenu.lua` | Elegir LAN u Online |
| `LobbyScene.lua` | Crear o unir lobby |
| `DefaultScene.lua` | Partida |

El flujo está en [Online](../../online/index.md).
