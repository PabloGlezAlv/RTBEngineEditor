# Lights and shadows

`LightComponent` creates a light and, if `syncPosition` / `syncDirection` are on, copies the transform every frame.

| Field | Default | Effect |
| --- | --- | --- |
| `lightType` | `Point` | `Directional`, `Point`, or `Spot` |
| `color` | White | Light color |
| `intensity` | `1` | Intensity |
| `range` | `10` | Reach of point and spot |
| `spotAngle` | `45` | Outer cone, degrees |
| `spotInnerAngle` | `30` | Inner cone |

```cpp
auto* light = GetOwner()->GetComponent<RTBEngine::Scene::LightComponent>();
light->lightType = RTBEngine::Rendering::LightType::Directional;
light->intensity = 1.2f;
light->SyncProperties();
```

## How to aim them

- **Directional**: the transform rotation defines the direction. It does not use `range`. It is the sun light, and the one that opens the shadow map.
- **Point**: the position lights in every direction out to `range`.
- **Spot**: transform position plus direction, clipped by `spotInnerAngle` and `spotAngle`.

`GetLight()` returns the render object (`Rendering::Light`) once the component has created it. As with the rigid body, do not assume it in `OnAwake`.

## Shadows

The frame runs `RenderShadowPass` before geometry. The directional light feeds the map. Objects the frustum and the static flags leave out do not write a shadow.

In the editor, the Scene View also runs the shadow pass so the authoring view looks like the game view.

## Environment and project

Ambient light, the DDGI volume, shadows, and global fog are not on `LightComponent`. They live in `lighting.ini`, edited from **Window → Project Settings**. See [Project settings](../../editor/ajustes.md) and [Post-processing](postproceso.md).

Prefab mode creates an editor light (`__PrefabEditorLight`) only so you can see the asset. It is not saved in the `.prefab`.
