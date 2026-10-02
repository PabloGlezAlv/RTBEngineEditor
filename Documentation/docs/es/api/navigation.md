# Navigation

El agente no escribe el transform. Tú mueves al personaje con la dirección que devuelve.

Campos de `NavAgentComponent`: `recalcInterval` 0.5, `targetMoveThreshold` 0.75, `waypointReachDistance` 0.35.

## SetDestination

```cpp
void NavAgentComponent::SetDestination(const Math::Vector3& worldDestination);
```

Pide un camino hasta ese punto de mundo. Guarda el destino y marca que hay uno. Si el punto se alejó del último pedido al menos `targetMoveThreshold` (0.75 por defecto), tira la polilínea anterior para buscar otra. Si todavía no hay waypoints y hay un grid activo, resuelve el camino en esta misma llamada, para que la IA pueda moverse en el frame del spawn. Sin grid, el destino queda guardado y el camino espera.

- `worldDestination`: punto de mundo. La Y se aplana al calcular la dirección de movimiento.

```cpp
agent->SetDestination(target->GetWorldPosition());
```

## ClearDestination

```cpp
void NavAgentComponent::ClearDestination();
```

Olvida destino, camino, índice de waypoint y cualquier búsqueda encolada. `HasDestination` y `HasActivePath` pasan a false. `GetPlanarMoveDirection` devuelve cero.

## EnsurePathReady

```cpp
void NavAgentComponent::EnsurePathReady();
```

Si hay destino, hay dueño y hay grid, y el camino aún no está listo, lo calcula ahora. Si no hay destino, no hay dueño o no hay grid, vuelve sin hacer nada. Si ya hay waypoints y el camino está activo, tampoco recalcula.

## HasActivePath

```cpp
bool NavAgentComponent::HasActivePath() const;
```

**Devuelve** true cuando la última búsqueda dejó un camino usable. `SetDestination` lo pone a false al invalidar la polilínea. `ClearDestination` lo deja en false.

## HasDestination

```cpp
bool NavAgentComponent::HasDestination() const;
```

**Devuelve** true desde `SetDestination` hasta `ClearDestination`, aunque el camino todavía no exista.

## GetDestination

```cpp
const Math::Vector3& GetDestination() const;
```

**Devuelve** el último punto pasado a `SetDestination`. Si nunca se pidió uno, es el valor por defecto del vector (cero) y `HasDestination` es false.

## HasMoveDirection

```cpp
bool NavAgentComponent::HasMoveDirection() const;
```

**Devuelve** true si hay camino activo, la lista de waypoints no está vacía y el índice actual cae dentro. Es la comprobación previa a moverse.

```cpp
if (agent->HasMoveDirection()) {
    Math::Vector3 dir = agent->GetPlanarMoveDirection(GetOwner()->GetWorldPosition());
}
```

## GetPlanarMoveDirection

```cpp
Math::Vector3 NavAgentComponent::GetPlanarMoveDirection(const Math::Vector3& ownerWorldPosition) const;
```

Dirección en XZ desde la posición mundo del agente hasta el waypoint actual, normalizada, con Y a 0. No mueve al objeto. Sin camino, con la lista vacía, con el índice fuera de rango, o si ya estás encima del waypoint (longitud al cuadrado menor que 0.0001), devuelve cero.

- `ownerWorldPosition`: posición mundo del personaje, normalmente `GetOwner()->GetWorldPosition()`.

**Devuelve** un vector plano de longitud 1, o cero si no hay hacia dónde ir.

## GetWaypoints

```cpp
const std::vector<Math::Vector3>& NavAgentComponent::GetWaypoints() const;
```

La polilínea del último camino, en mundo. Vacía si no hay camino. Desde un script no recorras el `vector` cruzando la DLL si puedes evitarlo: para moverte basta `GetPlanarMoveDirection`.

**Devuelve** los puntos, o una lista vacía.

## GetCurrentWaypointIndex

```cpp
int NavAgentComponent::GetCurrentWaypointIndex() const;
```

Índice del waypoint hacia el que apunta `GetPlanarMoveDirection`. `SetDestination`, cuando invalida el camino, lo pone a 0. `ClearDestination` también.

**Devuelve** el índice. 0 si no hay camino.

## NavGridComponent

El componente de grid define origen, tamaño y tamaño de celda. **Bake Grid** en el editor rellena el grid contra la física y el guardado de escena escribe `NombreEscena.navmesh`. **Clear Baked** lo tira.

`ProcessPathRequest` es el paso interno que el servicio llama con el grid y el pathfinder. El gameplay no lo invoca si ya usa `SetDestination`.
