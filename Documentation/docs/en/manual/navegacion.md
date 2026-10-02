# Navigation

Navigation is a baked grid, not a triangle navmesh. `NavGridComponent` defines the area. `NavAgentComponent` requests paths. `NavPathService` runs A* on the active scene's grid.

## Baking

1. Create an object, for example `Navigation`, and add `NavGridComponent`.
2. Adjust origin, size, and `cellSize` so they cover the playable floor.
3. In the Inspector, **Bake Grid**. The scene physics must already be initialized (the bake casts rays against the colliders).
4. `Ctrl+S`. The result is saved to `Assets/Scenes/<Escena>.navmesh` (format version 1).
5. **Clear Baked** drops the grid for that session.

## Agent

```cpp
auto* agent = GetOwner()->GetComponent<RTBEngine::Scene::NavAgentComponent>();
agent->SetDestination(worldPoint);
agent->EnsurePathReady();

if (agent->HasActivePath()) {
    RTBEngine::Math::Vector3 dir =
        agent->GetPlanarMoveDirection(GetOwner()->GetWorldPosition());
}
```

| Field | Approx. default | Use |
| --- | --- | --- |
| `recalcInterval` | `0.5` s | How often the path may be rebuilt |
| `targetMoveThreshold` | `0.75` | The destination has to move more than this to replan |
| `waypointReachDistance` | `0.35` | Distance at which a waypoint counts as reached |

`ClearDestination` drops the goal. `HasDestination` says whether there is a goal even if the path does not exist yet. `GetWaypoints` is the current polyline.

The agent does not teleport the object. It gives you a planar direction; your controller applies the movement (and the animation).

## Seeing it

**Window → Navigation Debug** turns on the overlay, only in the Scene View:

| Toggle | Draws |
| --- | --- |
| Bounds | Grid rectangle |
| Walkable | Green cells |
| Blocked | Red cells |
| Agent paths | Path, waypoints, and destination |
| Grid step | `0` is automatic (1 or 2). Raise the step if the grid is huge |
| Y offset | Lifts the lines off the floor. Default `0.15` |

The Game View does not draw this overlay. The panel does not bake either: baking stays on the `NavGridComponent` Inspector.
