# Scene y SceneManager

## GetInstance

```cpp
static SceneManager& GetInstance();
```

El singleton del motor. Siempre hay uno: no devuelve puntero y no hay que crearlo desde un script.

**Devuelve** la instancia viva.

## LoadScene

```cpp
bool LoadScene(const std::string& path);
```

Descarga la escena activa y carga `path` ahora mismo, en esta llamada. Vacía el `ObjectPool`, avisa `onSceneUnloading` / `onSceneLoaded` y despierta el ciclo de vida de la escena nueva. Una ruta vacía, un archivo que no carga, o una llamada mientras ya hay otra transición, dejan la escena anterior (si la carga falló) y devuelven false. Desde `OnUpdate` de un componente no la uses: destruye la escena que te está ejecutando. Encola con `RequestSceneLoad` y deja que el player procese el cambio al borde del frame.

- `path`: ruta de escena, por ejemplo `Assets/Scenes/MainMenu.lua`.

**Devuelve** true si la escena nueva quedó activa.

## RequestSceneLoad

```cpp
bool RequestSceneLoad(const char* path);
```

Guarda la ruta y no carga nada todavía. El bucle de la aplicación llama `ProcessPendingSceneLoad` cuando es seguro destruir la escena actual. Un path null o vacío, o una llamada en mitad de otra transición, devuelve false y no pisa un cambio ya pedido si la transición está en curso. Si no hay transición, la última llamada gana: sustituye la ruta pendiente.

- `path`: ruta de escena. Tiene que seguir viva hasta que se copie; un literal o un buffer estable vale.

**Devuelve** true si la petición quedó encolada.

```cpp
RTBEngine::Scene::SceneManager::GetInstance()
    .RequestSceneLoad("Assets/Scenes/MainMenu.lua");
```

## ProcessPendingSceneLoad

```cpp
bool ProcessPendingSceneLoad();
```

Si hay una ruta pendiente, la limpia y llama a `LoadScene`. Lo hace el player y el editor al borde del frame. Un script de juego no tiene que llamarlo: `RequestSceneLoad` basta.

**Devuelve** false si no había nada pendiente. Si lo había, devuelve lo que devolvió `LoadScene`.

## ClearPendingSceneLoad

```cpp
void ClearPendingSceneLoad();
```

Tira la ruta pendiente sin cargar. `LoadScene` y `UnloadCurrentScene` también lo hacen al empezar.

## UnloadCurrentScene

```cpp
void UnloadCurrentScene();
```

Destruye la escena activa, limpia el pool y deja el manager sin escena. Si no hay escena, no hace nada. Si ya hay una transición en curso, avisa y vuelve sin tocar nada. No la llames desde un componente de esa escena.

## GetActiveScene

```cpp
Scene* GetActiveScene() const;
```

**Devuelve** la escena cargada, o null si no hay ninguna.

## GetActiveScenePath

```cpp
const std::string& GetActiveScenePath() const;
```

**Devuelve** la ruta con la que se cargó la escena activa. Vacía si no hay escena.

## HasActiveScene

```cpp
bool HasActiveScene() const;
```

**Devuelve** true si `GetActiveScene()` no es null.

## Instantiate

```cpp
GameObject* Instantiate(const std::string& name = "GameObject", GameObject* parent = nullptr);
```

Crea un `GameObject` vacío en la escena activa, con ese nombre y ese padre. Sin escena activa escribe un error y devuelve null. El nombre por defecto es `"GameObject"`. Quédate el puntero: `FindGameObject` devuelve el primer nombre igual, no “el que acabas de crear” si ya había otro.

- `name`: nombre inicial.
- `parent`: padre, o null para la raíz.

**Devuelve** el objeto nuevo, o null si no hay escena.

## Instantiate

```cpp
GameObject* Instantiate(const Prefab& prefab, GameObject* parent = nullptr, bool regenerateUuids = true);
```

Clona el prefab dentro de la escena activa. Sin escena devuelve null. `regenerateUuids` true genera UUID nuevos para soltar una instancia de nivel. `false` conserva las identidades del asset, que es lo que hace el editor al reabrir el prefab para editarlo.

- `prefab`: asset ya cargado en memoria.
- `parent`: padre de la raíz instanciada, o null.
- `regenerateUuids`: true para identidades nuevas. El default es true.

**Devuelve** la raíz instanciada, o null si no hay escena.

## MarkSceneDirty

```cpp
void MarkSceneDirty();
```

Marca la escena activa como modificada para que el editor ofrezca guardar. El gameplay en el player no depende de esto.

## ClearSceneDirty

```cpp
void ClearSceneDirty();
```

Quita la marca de modificada. Lo hace un guardado correcto. `LoadScene` también la limpia al terminar.

## IsSceneDirty

```cpp
bool IsSceneDirty() const;
```

**Devuelve** true si hay cambios sin guardar. Una escena recién cargada está limpia.

## AddGameObject

```cpp
void AddGameObject(GameObject* gameObject, bool queueLifecycle = true);
```

Pasa a la escena la propiedad de ese objeto. Si la escena ya completó el arranque y `queueLifecycle` es true, encola `OnAwake` / `OnEnable` / `OnStart`. Si se llama en mitad de un recorrido de la escena, el alta espera al final del recorrido. `Instantiate` ya lo hace; úsalo cuando construyas un objeto a mano.

- `gameObject`: objeto que la escena pasa a poseer. No lo borres tú después.
- `queueLifecycle`: true para despertar el ciclo de vida. El default es true.

## FindGameObject

```cpp
GameObject* FindGameObject(const std::string& name);
```

Recorre los objetos que la escena posee, y los que están pendientes de insertarse, y devuelve el primero cuyo nombre coincide. No es una búsqueda única: si hay dos `"Arrow"`, te quedas con el que aparezca antes en esa lista. Después de un `Instantiate` del mismo nombre, guarda el puntero que te devolvieron.

- `name`: nombre exacto.

**Devuelve** el objeto, o null si ninguno coincide.

## FindGameObjectByUUID

```cpp
GameObject* FindGameObjectByUUID(const std::string& uuid);
```

Busca por UUID en el mapa de la escena. Un UUID vacío devuelve null. Si el objeto ya no pertenece a esta escena, también devuelve null.

- `uuid`: identidad guardada en el `.lua` o en el prefab.

**Devuelve** el objeto, o null.

## GetActiveCamera

```cpp
Rendering::Camera* GetActiveCamera();
```

La cámara de render de la `CameraComponent` con `isMainCamera`. Si ninguna está marcada, usa la primera cámara de la escena. Si no hay ninguna, devuelve null. La cámara de la Scene View del editor no sale de aquí.

**Devuelve** la `Rendering::Camera` activa, o null.
