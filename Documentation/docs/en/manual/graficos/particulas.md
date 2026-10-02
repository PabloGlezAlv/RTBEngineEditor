# Particles

`ParticleSystem` is a scene component. In the Inspector it has **Play**, **Pause**, **Stop**, and **Burst** buttons, then the reflected fields.

| Field | Default | Use |
| --- | --- | --- |
| `maxParticles` | `256` | Cap on live particles |
| `emissionRate` | `40` | Particles per second |
| `playOnAwake` | `true` | Starts when activated |
| `simulateInEditMode` | `true` | Visible in the Scene View without Play |
| `burstCount` | `10` | How many the Burst button releases |

```cpp
auto* fx = GetOwner()->GetComponent<RTBEngine::Scene::ParticleSystem>();
fx->Play();
fx->Emit(24);
fx->Pause();
fx->Stop();
```

Create one from the hierarchy: right-click → **Create Particle System**. The object is born with a cone emitter, `playOnAwake`, and `simulateInEditMode`.

In Edit, `EditorApplication` calls the preview of systems that simulate in edit mode and are playing. Game scripts still do not run.

The system texture is assigned like any other asset, by dragging from the Content Browser.

## Trails

`TrailRenderer` draws the ribbon behind the object. Pair it with a pooled projectile: on `Release`, the trail has to reset so it does not drag the previous trip. The projectile bridge does that in `OnLateUpdate`, once the ECS flight has finished.

## Order in the frame

Opaque particles go into the geometry pass. If your game component wants its own transparent pass, return true from `WantsTransparentRender` and implement `OnTransparentRender`. The engine has no special case by class name.
