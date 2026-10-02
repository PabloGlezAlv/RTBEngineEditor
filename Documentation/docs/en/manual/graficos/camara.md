# Camera

There are two different cameras.

| | Scene View | Game |
| --- | --- | --- |
| Owner | The editor | `CameraComponent` |
| Saved in the scene | No | Yes |
| Overlays (grid, nav, collider) | Yes | No |

## CameraComponent

| Field | Default |
| --- | --- |
| `fov` | `45` degrees |
| `nearClip` | `0.1` |
| `farClip` | `1000` |
| `projectionType` | `Perspective` or `Orthographic` |
| `orthographicSize` | `10` |
| `syncWithTransform` | `true` |
| `isMainCamera` | `false` |

Mark **one** camera with `isMainCamera`. `Scene::GetActiveCamera()` and the Game View use that one. If there is none, the game view has no framing.

```cpp
auto* cam = GetOwner()->GetComponent<RTBEngine::Scene::CameraComponent>();
cam->SetFOV(60.0f);
cam->SetNearPlane(0.05f);
cam->SetFarPlane(500.0f);
cam->isMainCamera = true;
RTBEngine::Rendering::Camera* raw = cam->GetCamera();
```

With `syncWithTransform`, position and rotation come from the object's `Transform`. That is the third-person camera pattern: the script moves the GameObject and the camera follows.

## Projection

`Rendering::ProjectionType::Perspective` uses `fov` and the view aspect. `Orthographic` uses `orthographicSize`. The editor updates the aspect when you resize the Game View or the player window.

## Editor camera

It is not a component. It starts looking at the origin from `(0, 2, 5)`. Right button plus mouse orbits the look target. WASD moves, Q/E lowers and raises, Shift multiplies speed by 3, the wheel dollies in. The gizmo shortcuts `W` `E` `R` only act if the Scene View has focus and you are not typing in a field.

Gizmo and view-cube details are in [Scene view](../../editor/vista-escena.md).
