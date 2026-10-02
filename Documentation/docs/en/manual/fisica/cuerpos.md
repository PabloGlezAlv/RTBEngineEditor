# Rigid bodies

Physics is Bullet 3.25, wrapped by `PhysicsWorld` and `PhysicsSystem`. The editor does not simulate in Edit. In Play, the editor calls `ResetPhysics` and `InitializePhysicsForScene` on entry.

The authoring component is `RigidBodyComponent`.

| Field | Meaning |
| --- | --- |
| `mass` | Mass. Default `1` |
| `friction` | Friction. Default `0.5` |
| `restitution` | Bounce. Default `0` |
| `bodyType` | `Static`, `Dynamic`, or `Kinematic` |
| `freezeRotationX/Y/Z` | Locks spin on that axis |

```cpp
auto* body = GetOwner()->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
if (body && body->HasRigidBody()) {
    RTBEngine::Physics::RigidBody* rb = body->GetRigidBody();
}
```

## Types

| Type | Moved by |
| --- | --- |
| `Static` | Nothing. Floors, walls |
| `Dynamic` | Forces and gravity |
| `Kinematic` | You move the transform. It pushes dynamics; gravity does not pull it |

A visual with no `RigidBodyComponent` does not participate. A dynamic body with no collider falls, but it does not produce useful contacts against the world.

## A floor that actually holds

1. A plane or cube with `MeshRenderer`.
2. `RigidBodyComponent`, `bodyType = Static`.
3. `BoxColliderComponent` with a `size` that covers the plane.

The falling cube is `Dynamic`, mass > 0, and has its own collider.

## Lifecycle

Bullet bodies are created once the scene is loaded (`InitializePhysicsForScene`), not in the component constructor. In `OnAwake`, `GetRigidBody()` can be null. In `OnStart`, after that initialization, it is there.

Changing scenes destroys the previous physics world. Do not store `btRigidBody*` or raw Bullet pointers in your scripts. Stay on the engine `RigidBody*` and drop it in `OnDestroy` if you kept a copy.

## Debugging in the editor

Select the object in Play. The Scene View draws the collider wireframe. The Game View does not. If the object does not fall, check that `bodyType` is `Dynamic` and that Play is actually running (in Pause, time is stopped).
