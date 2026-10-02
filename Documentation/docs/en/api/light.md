# Light

`RTBEngine::Scene::LightComponent`. `LightType`: `Directional`, `Point`, `Spot`.

Fields: `lightType` `Point`, `color` white, `intensity` 1, `range` 10, `spotAngle` 45, `spotInnerAngle` 30, `syncPosition` true, `syncDirection` true. `range` does not affect `Directional`. Spot angles are degrees.

## GetLight

```cpp
Rendering::Light* GetLight() const;
```

The lighting object the light pass reads. It can be null before the component creates it during startup.

**Returns** the render light, or null if it does not exist yet.

## SetLight

```cpp
void SetLight(std::unique_ptr<Rendering::Light> light);
```

Replaces the render object. The component takes ownership of the pointer. Passing an empty unique leaves the component without a light until the next startup. Normal gameplay does not call this: set the reflected fields and call `SyncProperties`.

- `light`: the new light. The unique is moved; the caller no longer owns it.

## SyncProperties

```cpp
void SyncProperties();
```

Copies type, color, intensity, range, spot angles, and transform sync onto the render object. Without this, writing `lightType` or `spotAngle` from C++ changes the reflected field and the drawn light keeps the previous value until the next `OnValidate`.

```cpp
auto* light = GetOwner()->GetComponent<RTBEngine::Scene::LightComponent>();
light->lightType = RTBEngine::Rendering::LightType::Spot;
light->spotAngle = 35.0f;
light->SyncProperties();
```
