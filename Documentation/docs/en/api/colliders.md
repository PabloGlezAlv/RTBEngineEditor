# Colliders

Contact callbacks are on [Component](component.md). Without a `RigidBodyComponent` on the same object, Bullet does not deliver those messages to the script.

## CollisionInfo

`RTBEngine::Physics::CollisionInfo` is what `OnCollisionEnter` / `OnTriggerEnter` and their Stay/Exit callbacks receive.

- `otherObject`: the other `GameObject`. It can be null when the contact has no scene object.
- `contactPoint`: contact point in world space.
- `contactNormal`: contact normal.
- `penetrationDepth`: how far they overlap. On a trigger this is the overlap, not a push response.

## SetSize

```cpp
void BoxColliderComponent::SetSize(const Math::Vector3& size);
```

Box size in the owner's local space. The reflected `size` field starts at `(1, 1, 1)`. An axis of 0 flattens that dimension.

- `size`: local width, height, and depth.

## GetSize

```cpp
Math::Vector3 BoxColliderComponent::GetSize() const;
```

**Returns** the box's current size. After `FitToOwnerMesh` it is the size fitted to the mesh, not the initial `(1, 1, 1)`.

## GetCenterOffset

```cpp
Math::Vector3 BoxColliderComponent::GetCenterOffset() const;
```

Center of the box relative to the owner's origin. If the physics collider does not exist yet, it returns `(0, 0, 0)`.

**Returns** the center offset, or zero if there is no collider.

## FitToOwnerMesh

```cpp
void BoxColliderComponent::FitToOwnerMesh();
```

Fits size and center to the local bounds of the owner's `MeshRenderer`. With no owner, no physics collider, or no `MeshRenderer`, it does nothing. A single mesh whose pointer is null also does nothing. A multi-mesh with degenerate bounds (minimum equal to maximum) returns without changing. It then scales the fitted size by the transform's local scale.

## SetIsTrigger

```cpp
void BoxColliderComponent::SetIsTrigger(bool trigger);
void SphereColliderComponent::SetIsTrigger(bool trigger);
```

Stores the flag. With `true` the volume is a trigger: it does not push, and the engine calls `OnTriggerEnter` / `Stay` / `Exit` instead of the solid collision messages when physics builds or syncs the body. The reflected `isTrigger` field starts false. The box and the sphere both write that field; this call does not rebuild the Bullet body.

- `trigger`: true for a notification volume.

## IsTrigger

```cpp
bool BoxColliderComponent::IsTrigger() const;
bool SphereColliderComponent::IsTrigger() const;
```

**Returns** true if the volume is a trigger. The default is false.

## SetRadius

```cpp
void SphereColliderComponent::SetRadius(float r);
```

Sphere radius in local space. This is the sphere's equivalent of the box `size`. A radius of 0 is not a useful volume.

- `r`: local radius.

## GetRadius

```cpp
float SphereColliderComponent::GetRadius() const;
```

**Returns** the sphere's local radius.
