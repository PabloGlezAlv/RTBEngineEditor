# ObjectPool

`RTBEngine::Scene::ObjectPool::GetInstance()` reutiliza instancias de un prefab. `kDefaultMaxPoolSize` es 32: por encima de ese número de copias libres, `Release` destruye en vez de guardar.

## ResolvePoolKey

```cpp
static std::string ResolvePoolKey(const std::string& prefabRefOrPath);
```

Normaliza una ruta o un nombre de prefab a la clave con la que el pool agrupa las copias. Una cadena vacía devuelve vacía. Si el registro conoce la ruta resuelta, el path tal cual o el nombre del asset, devuelve esa clave. Si no, devuelve la ruta resuelta por el `ResourceManager`, o el texto original si no hubo resolución.

- `prefabRefOrPath`: ruta `Assets/...prefab` o nombre registrado.

**Devuelve** la clave, o vacía si la entrada está vacía.

## Acquire

```cpp
GameObject* Acquire(const std::string& prefabPath,
                    const Math::Vector3& position,
                    const Math::Quaternion& rotation);
```

Saca una instancia de la lista libre de ese prefab o, si no hay, crea una. Coloca la posición y la rotación local y reinicia el ciclo de vida, así `OnEnable` / `OnStart` vuelven a correr. Sin escena activa, o con una clave vacía, devuelve null. Si el prefab no está en el registro, la creación avisa y devuelve null.

La rotación es un quaternion. Si partes de ángulos de Euler, construye el quaternion en radianes.

- `prefabPath`: ruta o nombre que `ResolvePoolKey` pueda resolver.
- `position`: posición local al entregar.
- `rotation`: rotación local al entregar.

**Devuelve** la instancia, o null.

```cpp
auto& pool = RTBEngine::Scene::ObjectPool::GetInstance();
pool.SetMaxPoolSize("Assets/Prefabs/Combat/Projectiles/Arrow.prefab", 64);
RTBEngine::Scene::GameObject* arrow = pool.Acquire(path, pos, rot);
```

## Release

```cpp
void Release(GameObject* instance);
```

Devuelve la instancia a la lista libre y la prepara para el siguiente `Acquire` (deja de estar activa en juego). No la destruye mientras la lista libre quepa en el máximo. Un puntero null no hace nada. Si se llama mientras la escena se está descargando, vuelve sin tocar nada: la escena ya posee la destrucción. Si el objeto no es del pool, lo quita de la escena. Si ya estaba libre, no lo encola otra vez. Si la lista libre ya llegó al máximo, destruye esta copia.

- `instance`: objeto obtenido con `Acquire`.

```cpp
pool.Release(arrow);
```

## Prewarm

```cpp
void Prewarm(const std::string& prefabPath, int count);
```

Crea `count` instancias (posición cero, rotación identidad) y las suelta enseguida, para que el primer `Acquire` de la partida no pague la carga. Una ruta vacía o un `count` menor o igual que 0 no hace nada. Si un `Acquire` falla a mitad, para.

- `prefabPath`: prefab a reservar.
- `count`: cuántas copias libres dejar listas.

## ClearUnused

```cpp
void ClearUnused();
```

Destruye solo las copias que están en las listas libres y vacía esas listas. Las instancias que un script tiene sacadas con `Acquire` siguen vivas. Si no hay escena activa, olvida las listas y los registros sin recorrer objetos.

## Clear

```cpp
void Clear();
```

Olvida listas libres, registros de instancias y tamaños máximos, y devuelve el máximo por defecto a `kDefaultMaxPoolSize`. No recorre la escena destruyendo objetos: después de esto el pool ya no reconoce esas instancias. `LoadScene` llama a `ClearIfAlive`, que es el camino de descarga, no este método. Desde gameplay, para devolver una copia usa `Release`.

## SetDefaultMaxPoolSize

```cpp
void SetDefaultMaxPoolSize(int maxSize);
```

Máximo de copias libres para los prefabs que no tienen un tope propio. Un valor negativo se guarda como 0: `Release` destruirá en vez de guardar.

- `maxSize`: tope. El default del pool es 32.

## SetMaxPoolSize

```cpp
void SetMaxPoolSize(const std::string& prefabPath, int maxSize);
```

Tope de copias libres para un prefab. La ruta se resuelve con `ResolvePoolKey`; si queda vacía, no cambia nada. Un `maxSize` negativo se guarda como 0.

- `prefabPath`: prefab.
- `maxSize`: cuántas copias libres conservar.

## GetMaxPoolSize

```cpp
int GetMaxPoolSize(const std::string& prefabPath) const;
```

**Devuelve** el tope de ese prefab, o el tope por defecto si no se llamó a `SetMaxPoolSize` para su clave.

- `prefabPath`: prefab.

## OwnsInstance

```cpp
bool OwnsInstance(GameObject* instance) const;
```

**Devuelve** true si el pool tiene ficha de ese objeto, esté libre o prestado. Null devuelve false.
