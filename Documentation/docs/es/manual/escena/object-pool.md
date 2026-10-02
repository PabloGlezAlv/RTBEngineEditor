# Object Pool

`RTBEngine::Scene::ObjectPool` recicla instancias de un prefab. El default de tamaño es `kDefaultMaxPoolSize` (32) por clave de prefab.

```cpp
auto& pool = RTBEngine::Scene::ObjectPool::GetInstance();

pool.Prewarm("Assets/Prefabs/Combat/Projectiles/Arrow.prefab", 16);

RTBEngine::Scene::GameObject* arrow = pool.Acquire(
    "Assets/Prefabs/Combat/Projectiles/Arrow.prefab",
    origin,
    rotation);

pool.Release(arrow);
```

| Método | Efecto |
| --- | --- |
| `Prewarm(path, count)` | Crea instancias y las deja libres |
| `Acquire(path, position, rotation)` | Saca una libre o crea otra si cabe |
| `Release(instance)` | La desactiva y la devuelve a la lista libre |
| `SetMaxPoolSize(path, n)` | Tope de ese prefab |
| `ClearUnused()` | Destruye las que están libres |
| `Clear()` | Vacía el pool |
| `OwnsInstance` | True si esa instancia salió del pool |

`ResolvePoolKey` normaliza la ruta del prefab para que `Assets/...` y la ruta absoluta no creen dos pools.

## Ciclo

`Acquire` reanima la instancia (vuelve a pasar por el arranque de componentes que corresponda). `Release` la prepara para no seguir simulando ni pintando. No llames `delete` sobre un objeto del pool.

Al descargar la escena, no dejes punteros a instancias adquiridas. Si el pool se limpia, esos `GameObject*` mueren.

## Cuándo no usarlo

Un personaje de jugador, una luz o un canvas de menú no necesitan pool. El pool compensa objetos que nacen y mueren muchas veces por segundo: proyectiles, impactos, números de daño, miembros de un enjambre.

El vuelo denso de esos proyectiles puede vivir además en el [ECS](../ecs.md). El pool sigue siendo el dueño del `GameObject` visual.
