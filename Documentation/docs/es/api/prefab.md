# Prefab

Un `RTBEngine::Scene::Prefab` es el asset en memoria. El archivo en disco es `.prefab`. El gameplay no llama Apply ni Revert: eso es `PrefabOverrideOps` del editor. En Play la instancia ya es un `GameObject` con los valores aplicados.

## Instantiate

```cpp
GameObject* SceneManager::Instantiate(const Prefab& prefab,
                                      GameObject* parent = nullptr,
                                      bool regenerateUuids = true);
```

Clona el prefab en la escena activa y devuelve la raíz. Sin escena activa escribe un error y devuelve null. El padre null deja la raíz en la raíz de la escena.

`regenerateUuids` true (el default) genera UUID nuevos. Úsalo al soltar una copia en un nivel: si no, dos instancias compartirían identidad y las referencias del `.lua` no sabrían cuál es cuál. `false` conserva los UUID del asset. El editor lo pasa así al reabrir el prefab para editarlo, para no cambiar las identidades guardadas.

- `prefab`: asset cargado.
- `parent`: padre de la raíz, o null.
- `regenerateUuids`: true para identidades nuevas.

**Devuelve** la raíz, o null si no hay escena.

## IsPrefabInstance

```cpp
bool GameObject::IsPrefabInstance() const;
```

**Devuelve** true cuando el objeto salió de un `.prefab` (tiene nombre de prefab). Un objeto creado con `Instantiate("Nombre")` devuelve false.

## GetPrefabName

```cpp
const std::string& GameObject::GetPrefabName() const;
```

Nombre del asset del que salió la instancia. Vacío si `IsPrefabInstance` es false.

**Devuelve** el nombre del prefab, o una cadena vacía.

El registro (`PrefabRegistry`) lo usa el editor y el pool para cargar por ruta. Desde un script, las copias repetidas salen del [ObjectPool](objectpool.md) con la ruta `Assets/...`.
