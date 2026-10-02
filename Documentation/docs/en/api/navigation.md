# Navigation

The agent does not write the transform. You move the character with the direction it returns.

`NavAgentComponent` fields: `recalcInterval` 0.5, `targetMoveThreshold` 0.75, `waypointReachDistance` 0.35.

## SetDestination

```cpp
void NavAgentComponent::SetDestination(const Math::Vector3& worldDestination);
```

Asks for a path to that world point. It stores the destination and marks that there is one. If the point moved at least `targetMoveThreshold` (0.75 by default) from the last request, it drops the previous polyline so a new search can run. If there are still no waypoints and a grid is active, it solves the path inside this call, so AI can move on the spawn frame. With no grid, the destination is stored and the path waits.

- `worldDestination`: world point. Y is flattened when the move direction is computed.

```cpp
agent->SetDestination(target->GetWorldPosition());
```

## ClearDestination

```cpp
void NavAgentComponent::ClearDestination();
```

Forgets the destination, the path, the waypoint index, and any queued search. `HasDestination` and `HasActivePath` become false. `GetPlanarMoveDirection` returns zero.

## EnsurePathReady

```cpp
void NavAgentComponent::EnsurePathReady();
```

If there is a destination, an owner, and a grid, and the path is not ready yet, it computes it now. With no destination, no owner, or no grid, it returns without doing anything. If waypoints already exist and the path is active, it does not recompute.

## HasActivePath

```cpp
bool NavAgentComponent::HasActivePath() const;
```

**Returns** true when the last search left a usable path. `SetDestination` sets it false when it invalidates the polyline. `ClearDestination` leaves it false.

## HasDestination

```cpp
bool NavAgentComponent::HasDestination() const;
```

**Returns** true from `SetDestination` until `ClearDestination`, even when the path does not exist yet.

## GetDestination

```cpp
const Math::Vector3& NavAgentComponent::GetDestination() const;
```

**Returns** the last point passed to `SetDestination`. If none was ever requested, it is the vector's default (zero) and `HasDestination` is false.

## HasMoveDirection

```cpp
bool NavAgentComponent::HasMoveDirection() const;
```

**Returns** true if there is an active path, the waypoint list is not empty, and the current index falls inside it. Check this before moving.

```cpp
if (agent->HasMoveDirection()) {
    Math::Vector3 dir = agent->GetPlanarMoveDirection(GetOwner()->GetWorldPosition());
}
```

## GetPlanarMoveDirection

```cpp
Math::Vector3 NavAgentComponent::GetPlanarMoveDirection(const Math::Vector3& ownerWorldPosition) const;
```

XZ direction from the agent's world position to the current waypoint, normalized, with Y set to 0. It does not move the object. With no path, an empty list, an index out of range, or when you are already on the waypoint (squared length under 0.0001), it returns zero.

- `ownerWorldPosition`: the character's world position, usually `GetOwner()->GetWorldPosition()`.

**Returns** a planar vector of length 1, or zero if there is nowhere to go.

## GetWaypoints

```cpp
const std::vector<Math::Vector3>& NavAgentComponent::GetWaypoints() const;
```

The last path's polyline, in world space. Empty if there is no path. From a script, avoid walking the `vector` across the DLL when you can: `GetPlanarMoveDirection` is enough to move.

**Returns** the points, or an empty list.

## GetCurrentWaypointIndex

```cpp
int NavAgentComponent::GetCurrentWaypointIndex() const;
```

Index of the waypoint `GetPlanarMoveDirection` aims at. `SetDestination`, when it invalidates the path, sets it to 0. `ClearDestination` does too.

**Returns** the index. 0 if there is no path.

## NavGridComponent

The grid component defines origin, size, and cell size. **Bake Grid** in the editor fills the grid against physics, and saving the scene writes `SceneName.navmesh`. **Clear Baked** throws it away.

`ProcessPathRequest` is the internal step the service calls with the grid and the pathfinder. Gameplay does not call it when it already uses `SetDestination`.
