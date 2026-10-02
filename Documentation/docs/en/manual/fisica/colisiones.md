# Collisions

Contacts reach **any** component on the same `GameObject` that has the body and the collider.

```cpp
void Bumper::OnCollisionEnter(const RTBEngine::Physics::CollisionInfo& hit) {
    const char* name = hit.otherObject ? hit.otherObject->GetNameCStr() : "?";
    char msg[256];
    snprintf(msg, sizeof(msg), "Choque con %s", name);
    RTB_INFO(msg);
}
```

`CollisionInfo`:

| Field | Type | Contents |
| --- | --- | --- |
| `otherObject` | `GameObject*` | The other scene object |
| `contactPoint` | `Vector3` | Contact point in world space |
| `contactNormal` | `Vector3` | Contact normal |
| `penetrationDepth` | `float` | Penetration |

`OnCollisionStay` repeats while the contact lasts. `OnCollisionExit` fires when they separate.

## Triggers

`BoxColliderComponent::SetIsTrigger(true)` (reflected field `isTrigger`) does not resolve a push. It fires:

- `OnTriggerEnter`
- `OnTriggerStay`
- `OnTriggerExit`

The argument is the same `CollisionInfo`. Use it for damage zones, pickups, and gameplay volumes. The trigger object is usually `Kinematic` or `Static`.

## Box and sphere

`BoxColliderComponent`:

```cpp
collider->SetSize(RTBEngine::Math::Vector3(1.0f, 2.0f, 1.0f));
collider->FitToOwnerMesh();
bool trigger = collider->IsTrigger();
```

`size` is the full reflected size (default `1,1,1`). `FitToOwnerMesh` fits size and center to the owner's `MeshRenderer` bounds, in local space.

`SphereColliderComponent` follows the same trigger pattern and the same lifetime, tied to the scene `PhysicsWorld`.

## What has to be on the object

| You want | On the same GameObject |
| --- | --- |
| Solid contact | `RigidBodyComponent` + collider with `isTrigger = false` |
| Zone | `RigidBodyComponent` + collider with `isTrigger = true` |
| Callbacks | Any sibling script. The script does not have to be the collider |

If the other object has no collider, `otherObject` will not show up: Bullet does not invent contacts.

## Layers

Two objects are tested only if the layer matrix allows it. Change an object's layer with `SetCollisionLayer` or `SetCollisionLayerByName`. The matrix is edited in [Layers](capas.md).
