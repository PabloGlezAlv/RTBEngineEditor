# Lifecycle

`SceneLifecycle` calls these methods. Do not call them yourself.

| Method | When |
| --- | --- |
| `OnAwake` | The component is added to the object. Scene data is not there yet |
| `OnEnable` | The object is active in the hierarchy and the component is enabled |
| `OnValidate` | After properties are applied, and when the Inspector edits a field |
| `OnStart` | First tick, once, with properties and references already resolved |
| `OnUpdate` | Every frame |
| `OnFixedUpdate` | Physics step |
| `OnLateUpdate` | After physics and the ECS simulation tick |
| `OnDisable` | The component or the object stops being active |
| `OnDestroy` | The component is removed or the object is destroyed |
| `OnParentChanged` | The owner's parent changes |

There are also `OnCollisionEnter` / `Stay` / `Exit` and `OnTriggerEnter` / `Stay` / `Exit`. You need a body and a collider on the same object. See [Collisions](../fisica/colisiones.md).

## Scene load, in order

```text
AddComponent          OnAwake          speedRef == constructor default
Apply properties      OnValidate       speedRef == value from the .lua
Resolve UUIDs         —                GameObject* pointers are set
first Update          OnStart          read authored state here
```

```cpp
void Spinner::OnAwake() {
    // speedRef may still be 0 or the default. Do not set up gameplay here.
}

void Spinner::OnStart() {
    speed = speedRef;
    auto* body = GetOwner()->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    if (body && body->GetRigidBody()) {
        // the Bullet body exists after the scene physics init
    }
}
```

## Enable

`SetActive(false)` on the GameObject deactivates the hierarchy and calls `OnDisable` on the components that were active. `SetEnabled(false)` on a single component does the same for that component. Reactivating calls `OnEnable`. `OnStart` does not run again.

`OnDestroy` is where you cancel your own work. The component's latent actions cancel themselves when it is destroyed.

## Edit versus Play

| | Edit | Play |
| --- | --- | --- |
| Script `OnAwake` / `OnStart` / `OnUpdate` | Do not run as simulation | Yes |
| `OnValidate` | Yes, while editing | Yes, if something rewrites properties |
| `OnEditModeSimulate` | Only if the component asks for it | Not needed |

Stopping Play reloads the scene. Anything you changed in memory during Play is gone, unless you wrote it to disk some other way.
