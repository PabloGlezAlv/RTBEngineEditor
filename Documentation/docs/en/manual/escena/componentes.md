# Components

A component is a piece of behavior or data attached to a `GameObject`. Built-ins (mesh, light, camera, body, collider, audio, particles, navigation, network, UI) and your own types in `GameScripts` inherit from `RTBEngine::Scene::Component`.

## Surface

```cpp
class RTB_API Component {
public:
    virtual void OnAwake() {}
    virtual void OnEnable() {}
    virtual void OnStart() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnFixedUpdate(float fixedDeltaTime) {}
    virtual void OnLateUpdate(float deltaTime) {}
    virtual void OnDisable() {}
    virtual void OnDestroy() {}
    virtual void OnValidate() {}

    virtual void OnCollisionEnter(const Physics::CollisionInfo& collision) {}
    virtual void OnCollisionStay(const Physics::CollisionInfo& collision) {}
    virtual void OnCollisionExit(const Physics::CollisionInfo& collision) {}
    virtual void OnTriggerEnter(const Physics::CollisionInfo& collision) {}
    virtual void OnTriggerStay(const Physics::CollisionInfo& collision) {}
    virtual void OnTriggerExit(const Physics::CollisionInfo& collision) {}

    GameObject* GetOwner() const;
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
};
```

`GetOwner()` is the owning `GameObject`. `SetEnabled(false)` fires `OnDisable` without removing the component or destroying the object.

## Built-ins you will add in the Inspector

| Component | What it is for |
| --- | --- |
| `MeshRenderer` | Mesh, texture, color, shader |
| `CameraComponent` | Game camera. `isMainCamera` feeds the Game View |
| `LightComponent` | Directional, Point, or Spot |
| `RigidBodyComponent` | Static, Dynamic, or Kinematic |
| `BoxColliderComponent` / `SphereColliderComponent` | Volume. `isTrigger` does not push; it notifies |
| `AudioSourceComponent` | FMOD clip, volume, pitch, loop |
| `ParticleSystem` | Emitter. `Play`, `Stop`, `Pause`, `Emit` |
| `TrailRenderer` | Trail |
| `VolumeComponent` | Distance fog and local volumetric fog |
| `Animator` | FBX clips and bone pose |
| `NavGridComponent` / `NavAgentComponent` | Baked grid and agent |
| `NetworkIdentity` / `NetworkTransform` | Network identity and replicated pose |
| `Canvas`, `UIButton`, `UIText`, `UIImage`, `UIPanel` | UI |

Each one has a page in the [Scripting API](../../api/index.md).

## Enabling and disabling

An object that is inactive in the hierarchy does not receive updates. A disabled component does not either. `OnEnable` / `OnDisable` pair those transitions. You do not need to check `IsEnabled()` at the start of `OnUpdate`: if you are there, the tick is active.

`OnValidate` runs when the Inspector or the loader changes a reflected property. Use it to clamp ranges and refresh caches. It also runs in Edit.

## Optional drawing and preview

If `WantsTransparentRender()` returns true, the scene calls `OnTransparentRender` during the transparency pass. The engine does not know your effect; it only asks.

If `WantsEditModeSimulate()` returns true, the Scene View calls `OnEditModeSimulate` without entering Play. `ParticleSystem` and the `Animator` preview use that path.

The exact call order is in [Lifecycle](../scripting/ciclo-de-vida.md).
