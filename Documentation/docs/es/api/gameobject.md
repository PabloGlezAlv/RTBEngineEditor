# GameObject

`RTBEngine::Scene::GameObject`. La copia está borrada. Desde un script, el nombre se lee con `GetNameCStr`: un `std::string` no cruza la frontera de la DLL.

## GetNameCStr

```cpp
const char* GetNameCStr() const;
```

Devuelve el nombre del objeto como `const char*`. Es el acceso seguro desde `GameScripts.dll`, porque el `std::string` interno vive en el heap del motor. El puntero sigue siendo válido hasta el siguiente `SetName` o hasta que el objeto se destruya.

**Devuelve** el nombre, nunca null: un objeto sin nombre custom sigue teniendo el default `"GameObject"`.

```cpp
const char* name = GetOwner()->GetNameCStr();
```

## GetName

```cpp
const std::string& GetName() const;
```

Devuelve la referencia al `std::string` del nombre. Úsala dentro del motor. Desde un script de juego usa `GetNameCStr`: devolver o destruir ese `std::string` en el otro lado de la DLL mezcla heaps.

**Devuelve** el nombre.

## SetName

```cpp
void SetName(const std::string& name);
```

Sustituye el nombre. `FindGameObject` busca por este texto y devuelve el primer coincidente, así que dos objetos pueden llamarse igual.

- `name`: nombre nuevo. Una cadena vacía deja el nombre vacío.

## GetUUID

```cpp
const std::string& GetUUID() const;
```

Identidad estable del objeto en la escena y en los prefabs. Las referencias del `.lua` apuntan aquí, no al nombre.

**Devuelve** el UUID. Puede estar vacío en un objeto recién construido que todavía no pasó por `GenerateNewUUID` o por el loader.

## SetUUID

```cpp
void SetUUID(const std::string& id);
```

Escribe el UUID. El loader y el prefab lo usan al instanciar. Cambiarlo en Play rompe referencias que ya apuntaban al valor anterior.

- `id`: UUID nuevo.

## GenerateNewUUID

```cpp
static std::string GenerateNewUUID();
```

Crea un UUID nuevo, sin asignarlo a ningún objeto. `Instantiate` de un prefab con `regenerateUuids` true lo usa para que cada copia tenga identidad propia.

**Devuelve** el UUID generado.

## AddComponent

```cpp
template <typename T>
T* AddComponent();
```

Crea un componente `T` con la fábrica registrada (`T::StaticTypeName()` y `RTB_REGISTER_COMPONENT`) y lo adopta. El módulo que registra el tipo es el que luego lo destruye. Llama a `OnAwake` cuando el ciclo de vida del objeto ya está en marcha.

- `T`: tipo que deriva de `Component` y está registrado.

**Devuelve** el componente nuevo, o null si el nombre de tipo está vacío, no hay `TypeInfo`, o la fábrica no creó el objeto.

## AddComponentOfType

```cpp
Component* AddComponentOfType(const char* typeName);
```

Igual que `AddComponent`, eligiendo el tipo por el nombre del registro. Es la vía cuando el tipo se conoce como texto (`"Spinner"`).

- `typeName`: nombre de `GetTypeName` / `StaticTypeName`. Null o cadena vacía devuelve null y escribe un error.

**Devuelve** el componente, o null si no está registrado o no tiene fábrica.

## GetComponent

```cpp
template <typename T>
T* GetComponent();
```

Busca en este objeto el primer componente que sea `T` (o una clase derivada que el `dynamic_cast` acepte). Si `T` publica `TypeId()`, mira primero la caché por id.

**Devuelve** el componente, o null si este objeto no lo tiene.

```cpp
auto* body = GetOwner()->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
```

## HasComponent

```cpp
template <typename T>
bool HasComponent();
```

**Devuelve** true si `GetComponent<T>()` no es null.

## GetComponentInChildren

```cpp
template <typename T>
T* GetComponentInChildren(int maxDepth = -1);
```

Busca `T` en este objeto y, si no está, en los hijos. `maxDepth` limita cuántos niveles baja: `-1` recorre toda la descendencia.

- `maxDepth`: profundidad máxima. `-1` no corta.

**Devuelve** el primer componente encontrado, o null.

## RemoveComponent

```cpp
void RemoveComponent(Component* component);
```

Quita ese componente de este objeto y lo destruye con el `TypeInfo` que lo creó, así corre `OnDisable` si estaba activo y después `OnDestroy`. Un puntero null, un componente que no es de este objeto, o una llamada mientras el `GameObject` ya se está destruyendo, no hacen nada. Si se llama en mitad de un recorrido de componentes, la destrucción se aplaza hasta el final del recorrido.

- `component`: componente de este objeto.

## GetComponents

```cpp
const std::vector<ComponentPtr>& GetComponents() const;
```

La lista interna de componentes. Desde `GameScripts` no recorras ese `vector` de `unique_ptr`: el contenedor STL no cruza la DLL. Usa `GetComponentCount` y `GetComponentAt`.

**Devuelve** la lista que posee el motor.

## GetComponentCount

```cpp
std::size_t GetComponentCount() const;
```

**Devuelve** cuántos componentes tiene el objeto, incluidos los que están deshabilitados.

## GetComponentAt

```cpp
Component* GetComponentAt(std::size_t index) const;
```

Componente en esa posición de la lista. El orden es el de alta, no un orden de tipos.

- `index`: índice desde 0. Fuera de rango el acceso no es válido: comprueba `GetComponentCount` antes.

**Devuelve** el componente de ese índice.

## SetParent

```cpp
void SetParent(GameObject* newParent);
```

Cambia el padre. Null deja el objeto en la raíz de la escena. Si el padre es el mismo, no hace nada. Avisa a los componentes con `OnParentChanged`, invalida la matriz mundo y la actividad en jerarquía, y si el ciclo de vida ya empezó sincroniza `OnEnable` / `OnDisable` en este objeto y en sus hijos.

- `newParent`: nuevo padre, o null para la raíz.

## GetParent

```cpp
GameObject* GetParent() const;
```

**Devuelve** el padre, o null si está en la raíz.

## AddChild

```cpp
void AddChild(GameObject* child);
```

Añade `child` a la lista de hijos si aún no está. No cambia el `parent` del hijo: el camino completo es `SetParent`. Un `child` null no hace nada. Sube la versión de jerarquía que cachea, entre otras cosas, el `Animator` de un `MeshRenderer`.

- `child`: hijo a registrar.

## RemoveChild

```cpp
void RemoveChild(GameObject* child);
```

Quita `child` de la lista de hijos si estaba. No pone `parent` del hijo a null por sí solo. Un `child` null no hace nada.

- `child`: hijo a quitar.

## GetChildren

```cpp
const std::vector<GameObject*>& GetChildren() const;
```

Lista de hijos directos. Desde un script usa `GetChildCount` y `GetChildAt` para no cruzar el `vector` de la DLL.

**Devuelve** los hijos directos.

## GetChildCount

```cpp
std::size_t GetChildCount() const;
```

**Devuelve** el número de hijos directos.

## GetChildAt

```cpp
GameObject* GetChildAt(std::size_t index) const;
```

Hijo directo en ese índice. Comprueba `GetChildCount` antes.

- `index`: índice desde 0.

**Devuelve** el hijo.

## SetActive

```cpp
void SetActive(bool active);
```

Activa o desactiva este objeto. Si el valor no cambia, no hace nada. Cuando el ciclo de vida ya empezó, propaga el estado a los componentes de este objeto y de los hijos: los que pasan a activos reciben `OnEnable` (y `OnStart` si aún no corrió) y los que pasan a inactivos reciben `OnDisable`. Un hijo con `SetActive(true)` sigue inactivo en la jerarquía si un padre está inactivo.

- `active`: true para encender el objeto.

```cpp
GetOwner()->SetActive(true);
```

## IsActive

```cpp
bool IsActive() const;
```

Lee solo la bandera de este objeto. Un padre inactivo no la pone a false.

**Devuelve** true si este objeto está marcado activo.

## IsActiveInHierarchy

```cpp
bool IsActiveInHierarchy() const;
```

True solo si este objeto y todos sus padres están activos. Es la condición que usa el motor para llamar `OnEnable` y los ticks.

**Devuelve** true si la cadena hasta la raíz está activa.

## GetTransform

```cpp
Transform& GetTransform();
const Transform& GetTransform() const;
```

La pose local. Siempre existe: un `GameObject` no se queda sin transform. La pose mundo se lee con `GetWorldPosition`, `GetWorldRotation`, `GetWorldScale` y `GetWorldMatrix`.

**Devuelve** el transform local.

## GetWorldMatrix

```cpp
Math::Matrix4 GetWorldMatrix() const;
```

Matriz mundo, el producto del padre con la matriz local. Se recalcula si la pose local o la de un antecesor cambió.

**Devuelve** la matriz mundo. Sin padre coincide con `Transform::GetModelMatrix`.

## GetWorldPosition

```cpp
Math::Vector3 GetWorldPosition() const;
```

Posición mundo. Sin padre es la posición local.

**Devuelve** la posición mundo.

## GetWorldRotation

```cpp
Math::Quaternion GetWorldRotation() const;
```

Rotación mundo: la del padre compuesta con la local. Sin padre es la rotación local.

**Devuelve** la rotación mundo.

## GetWorldScale

```cpp
Math::Vector3 GetWorldScale() const;
```

Escala mundo, producto componente a componente con la del padre. Sin padre es la escala local.

**Devuelve** la escala mundo.

## GetOwningScene

```cpp
Scene* GetOwningScene() const;
```

Escena que posee este objeto. Null si todavía no se ha añadido a una escena o si ya se soltó.

**Devuelve** la escena dueña, o null.

## SetCollisionLayer

```cpp
void SetCollisionLayer(int layerIndex);
```

Asigna la capa de física. El índice se recorta a `[0, kMaxPhysicsLayers)`. La matriz de capas decide con quién choca.

- `layerIndex`: índice de capa. Valores fuera de rango se recortan.

## SetCollisionLayerByName

```cpp
void SetCollisionLayerByName(const std::string& layerName);
```

Resuelve el nombre en `PhysicsLayerSettings` y llama a `SetCollisionLayer`. Un nombre desconocido acaba en el índice que devuelva `GetLayerIndex` (normalmente 0 si no existe).

- `layerName`: nombre de capa del proyecto.

## GetCollisionLayer

```cpp
int GetCollisionLayer() const;
```

**Devuelve** el índice de capa actual. El default es 0.

## SetStatic

```cpp
void SetStatic(bool enabled);
```

Atajo de flags estáticos. `true` pone `Batching`, `ContributeGI` y `Navigation`. `false` los limpia todos. No marca `Occluder`: ese flag, puesto en todas las mallas estáticas, disparaba el fade de cámara y rompía la locomoción. El fade usa `Occludable`.

- `enabled`: true para el conjunto estático por defecto.

## SetStaticFlags

```cpp
void SetStaticFlags(StaticFlags flags);
```

Sustituye el conjunto de flags de una vez. No se hereda al escribir: la herencia se consulta en `HasStaticFlag` e `IsStatic`.

- `flags`: máscara completa. `StaticFlags::None` limpia los flags propios.

## IsPrefabInstance

```cpp
bool IsPrefabInstance() const;
```

**Devuelve** true si el objeto tiene nombre de prefab, o sea, nació de un asset `.prefab`. Un `GameObject` vacío devuelve false.

## GetPrefabName

```cpp
const std::string& GetPrefabName() const;
```

Nombre del asset de prefab del que salió esta instancia. Vacío si no es instancia.

**Devuelve** el nombre del prefab, o una cadena vacía.

## SetTransient

```cpp
void SetTransient(bool t);
```

Marca el objeto como transitorio. El editor lo usa para nodos que no deben guardarse en la escena (huesos de preview, ayudas). El gameplay normal lo deja en false.

- `t`: true si no debe persistir en el `.lua`.

## IsTransient

```cpp
bool IsTransient() const;
```

**Devuelve** true si el objeto es transitorio. El default es false.

## SetAnimatorBone

```cpp
void SetAnimatorBone(bool value);
```

Marca el objeto como hueso creado por un `Animator`. El editor lo trata distinto de un objeto de juego.

- `value`: true si es un hueso.

## IsAnimatorBone

```cpp
bool IsAnimatorBone() const;
```

**Devuelve** true si `SetAnimatorBone(true)` se aplicó. El default es false.
