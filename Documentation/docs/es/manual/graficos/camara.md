# Cámara

Hay dos cámaras distintas.

| | Scene View | Juego |
| --- | --- | --- |
| Dueño | El editor | `CameraComponent` |
| Se guarda en la escena | No | Sí |
| Overlays (grid, nav, collider) | Sí | No |

## CameraComponent

| Campo | Default |
| --- | --- |
| `fov` | `45` grados |
| `nearClip` | `0.1` |
| `farClip` | `1000` |
| `projectionType` | `Perspective` o `Orthographic` |
| `orthographicSize` | `10` |
| `syncWithTransform` | `true` |
| `isMainCamera` | `false` |

Marca **una** cámara con `isMainCamera`. `Scene::GetActiveCamera()` y la Game View usan esa. Si no hay ninguna, la vista de juego no tiene encuadre.

```cpp
auto* cam = GetOwner()->GetComponent<RTBEngine::Scene::CameraComponent>();
cam->SetFOV(60.0f);
cam->SetNearPlane(0.05f);
cam->SetFarPlane(500.0f);
cam->isMainCamera = true;
RTBEngine::Rendering::Camera* raw = cam->GetCamera();
```

Con `syncWithTransform`, posición y rotación salen del `Transform` del objeto. Es el patrón de una cámara en tercera persona: el script mueve el GameObject y la cámara lo sigue.

## Proyección

`Rendering::ProjectionType::Perspective` usa `fov` y el aspect de la vista. `Orthographic` usa `orthographicSize`. El editor actualiza el aspect cuando redimensionas Game View o la ventana del player.

## Cámara del editor

No es un componente. Arranca mirando al origen desde `(0, 2, 5)`. Botón derecho + ratón orbita la mirada. WASD se mueve, Q/E baja y sube, Shift multiplica la velocidad por 3, la rueda acerca. Los atajos `W` `E` `R` del gizmo solo actúan si la Scene View tiene foco y no estás escribiendo en un campo.

El detalle de gizmos y view cube está en [Vista de escena](../../editor/vista-escena.md).
