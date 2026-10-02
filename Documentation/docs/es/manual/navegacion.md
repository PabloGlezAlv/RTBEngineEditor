# Navegación

La navegación es un grid horneado, no un navmesh de triángulos. `NavGridComponent` define el área. `NavAgentComponent` pide caminos. `NavPathService` corre A* sobre el grid de la escena activa.

## Hornear

1. Crea un objeto, por ejemplo `Navigation`, y añade `NavGridComponent`.
2. Ajusta origen, tamaño y `cellSize` para cubrir el suelo jugable.
3. En el Inspector, **Bake Grid**. Hace falta la física de la escena inicializada (el bake lanza rayos contra los colliders).
4. `Ctrl+S`. El resultado se guarda en `Assets/Scenes/<Escena>.navmesh` (formato versión 1).
5. **Clear Baked** tira el grid de esa sesión.

## Agente

```cpp
auto* agent = GetOwner()->GetComponent<RTBEngine::Scene::NavAgentComponent>();
agent->SetDestination(worldPoint);
agent->EnsurePathReady();

if (agent->HasActivePath()) {
    RTBEngine::Math::Vector3 dir =
        agent->GetPlanarMoveDirection(GetOwner()->GetWorldPosition());
}
```

| Campo | Default aprox. | Uso |
| --- | --- | --- |
| `recalcInterval` | `0.5` s | Cada cuánto se puede rehacer el camino |
| `targetMoveThreshold` | `0.75` | El destino tiene que moverse más que esto para replanear |
| `waypointReachDistance` | `0.35` | Distancia a la que se da por alcanzado un waypoint |

`ClearDestination` suelta el objetivo. `HasDestination` dice si hay objetivo aunque el camino aún no exista. `GetWaypoints` es la polilínea actual.

El agente no teletransporta al objeto. Te da dirección plana; tu controlador aplica el movimiento (y la animación).

## Verlo

**Window → Navigation Debug** activa el overlay, solo en Scene View:

| Toggle | Dibuja |
| --- | --- |
| Bounds | Rectángulo del grid |
| Walkable | Celdas verdes |
| Blocked | Celdas rojas |
| Agent paths | Camino, waypoints y destino |
| Grid step | `0` es automático (1 o 2). Sube el paso si el grid es enorme |
| Y offset | Levanta las líneas del suelo. Default `0.15` |

La Game View no dibuja este overlay. El panel tampoco hornea: el bake sigue en el Inspector del `NavGridComponent`.
