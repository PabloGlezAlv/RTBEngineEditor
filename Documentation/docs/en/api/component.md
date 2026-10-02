# Component

`RTBEngine::Scene::Component` is the base of built-in components and of `GameScripts` scripts. The engine calls the messages; a script does not invoke them. Copy is deleted.

## OnAwake

```cpp
virtual void OnAwake();
```

The engine calls this once, when the component enters the scene and scene-file properties have not been applied yet and UUIDs are not resolved. Use it to reserve your own state. The owner's physics body does not exist yet, and `GetComponent` on a sibling can return null if that sibling has not awakened.

## OnEnable

```cpp
virtual void OnEnable();
```

Called when the component becomes active in the hierarchy: `IsEnabled()` is true and the owning `GameObject` is active, including its parents. It runs on load, when the object is reactivated, and when the component is enabled again. Subscribe here to anything that should live only while the object takes part in the game.

## OnDisable

```cpp
virtual void OnDisable();
```

Called when the component leaves the active hierarchy, because it was disabled, because the owner or a parent became inactive, or because it is about to be destroyed. Drop subscriptions made in `OnEnable`. Do not destroy the owner from here.

## OnStart

```cpp
virtual void OnStart();
```

Called once, after `OnAwake`, when reflected properties already hold the scene values and reference UUIDs point at objects. If the component or the owner is born inactive, it waits until the first time they are active. Read other components and set up gameplay here, not in `OnAwake`.

## OnUpdate

```cpp
virtual void OnUpdate(float deltaTime);
```

Called every game frame while the component is active and its update tick is on. `deltaTime` is the seconds since the previous frame, scaled by this component's time mode.

- `deltaTime`: seconds of this frame.

## OnFixedUpdate

```cpp
virtual void OnFixedUpdate(float fixedDeltaTime);
```

Called on the physics step, at a fixed interval, not once per rendered frame. Use it for forces and for contacts that must line up with Bullet.

- `fixedDeltaTime`: seconds of this physics step.

## OnLateUpdate

```cpp
virtual void OnLateUpdate(float deltaTime);
```

Called every frame after `OnUpdate`, physics, and the ECS simulation tick. Use it to read a resolved impact, apply damage, VFX, or audio, and to follow another transform that already moved in `OnUpdate`.

- `deltaTime`: seconds of this frame.

## OnValidate

```cpp
virtual void OnValidate();
```

The editor and the loader call this when Inspector properties change or when the object is deserialized. Adjust derived fields (clamp a range, rebuild a collider). Do not start gameplay: Edit mode is not a match.

## OnDestroy

```cpp
virtual void OnDestroy();
```

Called when the component is removed or the owner is destroyed. Latent actions on this component are cancelled here. Do not use the owner after you return: it may be mid-destruction.

## OnParentChanged

```cpp
virtual void OnParentChanged(GameObject* oldParent, GameObject* newParent);
```

Called on the components of the object whose parent just changed, after `SetParent`. `oldParent` is the previous parent and `newParent` is the new one. Either pointer can be null: null `oldParent` means the object was at the root, and null `newParent` means it moves to the root.

- `oldParent`: previous parent, or null.
- `newParent`: new parent, or null.

## OnCollisionEnter

```cpp
virtual void OnCollisionEnter(const Physics::CollisionInfo& collision);
```

First frame of a solid contact with another body. `collision.otherObject` is the other `GameObject`, `contactPoint` and `contactNormal` describe the contact, and `penetrationDepth` is how far they overlap. This object needs a `RigidBodyComponent`; a lone collider does not deliver the message. If the other side has no scene object, `otherObject` can be null.

- `collision`: contact data for this frame.

## OnCollisionStay

```cpp
virtual void OnCollisionStay(const Physics::CollisionInfo& collision);
```

Called every step while the solid contact remains, after `OnCollisionEnter` and before `OnCollisionExit`.

- `collision`: contact data for this step.

## OnCollisionExit

```cpp
virtual void OnCollisionExit(const Physics::CollisionInfo& collision);
```

Called on the frame the solid contact ends.

- `collision`: last known contact.

## OnTriggerEnter

```cpp
virtual void OnTriggerEnter(const Physics::CollisionInfo& collision);
```

Same as `OnCollisionEnter`, but this object's collider (or the other one) is a trigger: there is no physics response, only the notification. Without a `RigidBodyComponent` on the object, Bullet does not deliver the message.

- `collision`: overlap data.

## OnTriggerStay

```cpp
virtual void OnTriggerStay(const Physics::CollisionInfo& collision);
```

Called every step while the trigger overlap continues.

- `collision`: overlap data for this step.

## OnTriggerExit

```cpp
virtual void OnTriggerExit(const Physics::CollisionInfo& collision);
```

Called when the trigger overlap ends.

- `collision`: last known overlap.

## WantsTransparentRender

```cpp
virtual bool WantsTransparentRender() const;
```

Returns true when this component wants its own transparent draw. The default is false and the scene does not call `OnTransparentRender`. A gameplay effect that draws after opaque geometry returns true.

**Returns** true when `OnTransparentRender` should run this frame.

## OnTransparentRender

```cpp
virtual void OnTransparentRender(Rendering::Camera* camera);
```

The scene calls this during the transparent pass only if `WantsTransparentRender` returned true. `camera` is the camera drawing the view. The default body does nothing.

- `camera`: camera of the current pass.

## WantsEditModeSimulate

```cpp
virtual bool WantsEditModeSimulate() const;
```

Returns true when the component should advance in the Scene View without entering Play. The default is false. Particles and other effects with `simulateInEditMode` use it.

**Returns** true when the Scene View should call `OnEditModeSimulate`.

## OnEditModeSimulate

```cpp
virtual void OnEditModeSimulate(float deltaTime);
```

The Scene View calls this in Edit, with no match running, when `WantsEditModeSimulate` is true. `OnUpdate` and game physics do not run.

- `deltaTime`: seconds since the last edit-mode step.

## GetOwner

```cpp
GameObject* GetOwner() const;
```

Returns the `GameObject` this component is attached to. It is null only before the engine assigns the owner, so do not use it in the constructor.

**Returns** the owner, or null if it is not assigned yet.

## SetEnabled

```cpp
void SetEnabled(bool enabled);
```

Enables or disables this component without touching the `GameObject`. Turning it off calls `OnDisable` if it was active in the hierarchy. Turning it on, when the owner is active in the hierarchy, calls `OnEnable` and queues `OnStart` if that has not run yet. If the value does not change, nothing happens.

- `enabled`: true to take part in the lifecycle.

## IsEnabled

```cpp
bool IsEnabled() const;
```

Reads the component's own flag. It does not check whether the owner is active: that is `GameObject::IsActiveInHierarchy`.

**Returns** true if the component is enabled.

## SetUpdateTickEnabled

```cpp
void SetUpdateTickEnabled(bool enabled);
```

Turns `OnUpdate`, `OnFixedUpdate`, and `OnLateUpdate` on or off without disabling the component. `OnEnable` and `OnDisable` still run. Use it for a visible object that should not simulate.

- `enabled`: true to receive ticks.

## IsUpdateTickEnabled

```cpp
bool IsUpdateTickEnabled() const;
```

**Returns** true if update ticks are on. The initial value is the component's constructed default (on).

## SetTimeMode

```cpp
void SetTimeMode(ComponentTimeMode mode);
```

Chooses whether this component's time and its `Invoke` calls use the game time scale or the unscaled clock. `ComponentTimeMode::Scaled` is the default. `Unscaled` keeps running when game time scale is 0.

- `mode`: `Scaled` or `Unscaled`.

## GetTimeMode

```cpp
ComponentTimeMode GetTimeMode() const;
```

**Returns** the current time mode. The default is `Scaled`.

## Invoke

```cpp
Scripting::LatentActionHandle Invoke(float delaySeconds, std::function<void()> callback);
```

Schedules `callback` after `delaySeconds`, measured with this component's `ComponentTimeMode`. It is cancelled automatically in `OnDestroy`. A delay of 0 fires on the next scheduler step, not in the middle of this call.

- `delaySeconds`: wait before the call.
- `callback`: function with no arguments.

**Returns** a handle for `CancelInvoke`.

## InvokeRepeating

```cpp
Scripting::LatentActionHandle InvokeRepeating(float initialDelaySeconds, float intervalSeconds, std::function<void()> callback);
```

Same as `Invoke`, then repeats `callback` every `intervalSeconds`. Also cancelled in `OnDestroy`.

- `initialDelaySeconds`: wait before the first call.
- `intervalSeconds`: period between calls.
- `callback`: function with no arguments.

**Returns** a handle for `CancelInvoke`.

## StartSequence

```cpp
Scripting::LatentActionHandle StartSequence(Scripting::LatentSequence sequence);
```

Starts a latent sequence (waits and chained steps) on the engine scheduler, using the same time mode as `Invoke`. Cancelled in `OnDestroy`.

- `sequence`: steps to run.

**Returns** a handle for `CancelInvoke`.

## CancelInvoke

```cpp
void CancelInvoke(Scripting::LatentActionHandle handle);
```

Cancels the latent action for that handle. A handle that already fired or was already cancelled does nothing.

- `handle`: value returned by `Invoke`, `InvokeRepeating`, or `StartSequence`.

## CancelAllInvokes

```cpp
void CancelAllInvokes();
```

Cancels every latent action owned by this component. `OnDestroy` already does this; call it earlier if the component is still alive and you want the queue empty.

## GetTypeName

```cpp
virtual const char* GetTypeName() const = 0;
```

Stable type name used by the registry and by `.lua` scenes. `RTB_COMPONENT` implements it. Do not write it by hand.

**Returns** the type name, for example `"Spinner"`.

## GetTypeInfo

```cpp
virtual const Reflection::TypeInfo* GetTypeInfo() const;
```

Reflection metadata for the Inspector and the loader. `RTB_COMPONENT` fills it in. Without the macro it returns null and the type is not serialized.

**Returns** the registered `TypeInfo`, or null.
