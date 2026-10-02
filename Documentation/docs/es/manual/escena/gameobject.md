# GameObject

`RTBEngine::Scene::GameObject` es la unidad de la jerarquía. Tiene nombre, UUID, un `Transform`, una lista de componentes y un padre opcional.

No copies el objeto. El constructor de copia está borrado. Se crea por la escena o por `SceneManager::Instantiate`.

## Crear y buscar

```cpp
auto& scenes = RTBEngine::Scene::SceneManager::GetInstance();
RTBEngine::Scene::GameObject* cube = scenes.Instantiate("Cube");
cube->SetName("Caja");

RTBEngine::Scene::Scene* scene = scenes.GetActiveScene();
RTBEngine::Scene::GameObject* found = scene->FindGameObject("Caja");
RTBEngine::Scene::GameObject* byId = scene->FindGameObjectByUUID(cube->GetUUID());
```

Desde un script, el nombre que cruza la DLL es `GetNameCStr()`, no `GetName()` por valor. El detalle está en [Frontera DLL](../scripting/frontera-dll.md).

## Jerarquía

```cpp
weapon->SetParent(player);
RTBEngine::Scene::GameObject* parent = weapon->GetParent();
const std::vector<RTBEngine::Scene::GameObject*>& children = player->GetChildren();
```

`SetParent` rechaza ciclos: un padre no puede pasar a ser hijo de su propio descendiente. El editor hace la misma comprobación al soltar un nodo en la jerarquía.

`GetWorldPosition`, `GetWorldRotation`, `GetWorldScale` y `GetWorldMatrix` componen la cadena de padres. El `Transform` guarda la pose **local**.

## Activo

```cpp
go->SetActive(false);
bool self = go->IsActive();
bool visible = go->IsActiveInHierarchy();
```

`IsActive` es el flag local. `IsActiveInHierarchy` es falso si este objeto o cualquier padre está inactivo. Desactivar propaga `OnDisable` a los componentes. Volver a activar llama `OnEnable` y, si aún no había corrido, `OnStart`.

## Componentes

```cpp
auto* body = go->AddComponent<RTBEngine::Scene::RigidBodyComponent>();
auto* same = go->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
bool has = go->HasComponent<RTBEngine::Scene::MeshRenderer>();
auto* childCam = go->GetComponentInChildren<RTBEngine::Scene::CameraComponent>();

go->RemoveComponent(body);
```

`AddComponentOfType("Spinner")` crea un tipo registrado por nombre. Es lo que usa el menú **Add Component** del Inspector. El tipo tiene que estar registrado con `RTB_REGISTER_COMPONENT` dentro de `GameScripts.dll`.

## Capas, estáticos y prefab

| Método | Uso |
| --- | --- |
| `SetCollisionLayer` / `SetCollisionLayerByName` | Capa de la matriz de colisión |
| `SetStatic` / `SetStaticFlags` | Marcas de objeto estático (contribución a GI, etc.) |
| `IsPrefabInstance` / `GetPrefabName` | La instancia vino de un `.prefab` |
| `IsTransient` | No se serializa con la escena |
| `GetOwningScene` | Escena que posee el objeto |

Los huesos que crea el `Animator` se marcan con `SetAnimatorBone`. El editor los trata distinto al resto de la jerarquía.

## Identidad

`GenerateNewUUID()` crea un id nuevo. Las referencias entre objetos en la escena se guardan por UUID y se resuelven cuando todos los objetos ya existen. Por eso un `GameObject*` reflejado es nulo en `OnAwake` y válido en `OnStart`.
