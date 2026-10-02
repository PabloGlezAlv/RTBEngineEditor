# ECS

`RTBEngine::ECS::World`. `Add` hace emplace en el sparse set de `T`. `T` es un componente de simulación (POD), no un `RTBEngine::Scene::Component`.

El tick lo llama `Application`: `Simulation` después de `Scene::Update`, `Presentation` después de `LateUpdate`. `Clear` al descargar la escena borra entidades y deja los sistemas. Los sistemas se registran desde `RTBScripts_InitializeEcs` en `GameScripts`, no desde el Inspector.

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

**Devuelve** el mundo que la aplicación está tickeando, o null si nadie llamó a `SetActive` o el mundo activo ya se destruyó.

## SetActive

```cpp
static void SetActive(World* world);
```

Elige qué mundo ven `GetActive` y el tick. Null deja el motor sin mundo activo: el siguiente `GetActive` es null. Al destruir el mundo que era el activo, el puntero se limpia solo.

- `world`: mundo, o null.

## Create

```cpp
Entity Create();
```

Reserva una entidad. Reutiliza un índice libre y sube la generación, así un `Entity` guardado de antes no sigue vivo. La entidad nueva no tiene componentes.

**Devuelve** el handle. La generación arranca en 1.

## Destroy

```cpp
void Destroy(Entity entity);
```

Si `IsAlive` es false, no hace nada. Si está viva, le quita todos los componentes, devuelve el índice al pool y sube la generación: el handle que tenías deja de ser válido al momento.

- `entity`: handle de `Create`. Uno ya destruido se ignora.

## IsAlive

```cpp
bool IsAlive(Entity entity) const;
```

**Devuelve** true si el índice existe y la generación coincide. Un handle por defecto, o uno ya pasado a `Destroy`, devuelve false.

- `entity`: handle a comprobar.

## Clear

```cpp
void Clear();
```

Borra todos los componentes, todas las entidades y las estadísticas. El `SystemScheduler` no se toca: los sistemas registrados siguen para la siguiente escena. Después de esto cualquier `Entity` guardado falla `IsAlive`.

## Add

```cpp
template<typename T>
T& Add(Entity entity, T component = T{});
```

Inserta `T` en la entidad. Si omites el valor, se construye `T{}`. Comprueba `IsAlive` antes: un handle muerto no es un hueco válido de gameplay.

- `entity`: entidad viva.
- `component`: valor inicial. El default es `T{}`.

**Devuelve** una referencia al componente guardado en el sparse set.

## Remove

```cpp
template<typename T>
void Remove(Entity entity);
```

Quita `T` de la entidad si lo tiene. Si no hay almacén de `T` o la entidad no lo tiene, no hace nada. No destruye la entidad.

- `entity`: entidad.

## Has

```cpp
template<typename T>
bool Has(Entity entity) const;
```

**Devuelve** true si existe el almacén de `T` y esa entidad lo tiene. Si nadie ha hecho `Add<T>` todavía, false.

- `entity`: entidad.

## TryGet

```cpp
template<typename T>
T* TryGet(Entity entity);
```

**Devuelve** el puntero al componente, o null si no hay almacén de `T` o la entidad no lo tiene. Hay una sobrecarga const con el mismo contrato. El puntero queda inválido si haces `Remove<T>`, `Destroy` o `Clear`.

- `entity`: entidad.

## Tick

```cpp
void Tick(SystemPhase phase, float deltaTime);
```

Ejecuta los sistemas registrados en esa fase. `Application` llama `Simulation` después de `Scene::Update` y `Presentation` después de `LateUpdate`. Un script no lo llama: si lo haces, los sistemas corren otra vez en el mismo frame. `deltaTime` es el que recibe el bucle; en `Simulation` el mundo guarda cuánto tardó ese tick.

- `phase`: `Simulation` o `Presentation`.
- `deltaTime`: segundos de este frame.
