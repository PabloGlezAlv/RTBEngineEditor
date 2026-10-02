# RigidBody

`RTBEngine::Scene::RigidBodyComponent`. A script does not include Bullet headers: it stays on `Physics::RigidBody*`.

Fields: `mass` 1, `friction` 0.5, `restitution` 0, `bodyType` `Physics::RigidBodyType::Dynamic`, `freezeRotationX/Y/Z` false.

`RigidBodyType`: `Static` is not moved by forces, `Dynamic` is, `Kinematic` is moved by you and pushes dynamic bodies.

## SetRigidBody

```cpp
void SetRigidBody(std::unique_ptr<Physics::RigidBody> rb);
```

Hands over the body the scene physics built. The component takes ownership. `InitializePhysicsForScene` calls this, not gameplay. An empty unique leaves the component without a body.

- `rb`: the new body. It is moved; the caller no longer owns it.

## GetRigidBody

```cpp
Physics::RigidBody* GetRigidBody() const;
```

The live body. It is null until the scene initializes physics, so it is not there in `OnAwake`. Read it from `OnStart`.

**Returns** the `Physics::RigidBody`, or null.

## HasRigidBody

```cpp
bool HasRigidBody() const;
```

**Returns** true when `GetRigidBody()` is not null. False until `InitializePhysicsForScene`. In `OnAwake` it is false even though the component is in the scene.
