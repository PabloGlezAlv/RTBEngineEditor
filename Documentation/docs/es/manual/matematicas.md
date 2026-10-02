# Matemáticas

Los tipos de `RTBEngine::Math` son structs de floats. Cruzan la frontera DLL sin problema.

## Vector3

```cpp
RTBEngine::Math::Vector3 a(1.0f, 0.0f, 0.0f);
RTBEngine::Math::Vector3 b(0.0f, 0.0f, 1.0f);
float d = a.Dot(b);
RTBEngine::Math::Vector3 c = a.Cross(b);
float len = a.Length();
RTBEngine::Math::Vector3 n = a.Normalized();
```

Existen `Vector2` y `Vector4` con el mismo estilo (`x`, `y`, `z`, `w`).

## Quaternion y Euler

`Quaternion::FromEulerAngles(pitch, yaw, roll)` y `FromEulerAngles(Vector3)` trabajan en **radianes**. Orden YXZ, con forward +Z cuando yaw y pitch son 0. El yaw se niega dentro de la función para que el forward por defecto mire a +Z.

`Transform::SetRotation(Vector3)` y `Rotate(Vector3)` pasan ese vector a `FromEulerAngles` sin convertir. También son radianes.

El Inspector y el Lua de escena hablan en **grados**. El binding Lua multiplica por `pi/180` antes de llamar al quaternion. Si copias un `90` del Inspector a C++, conviértelo:

```cpp
constexpr float kDegToRad = 3.14159265f / 180.0f;
transform.SetRotation(RTBEngine::Math::Vector3(0.0f, 90.0f * kDegToRad, 0.0f));
```

`Quaternion::Slerp` y `Lerp` interpolan. `FromMatrix` saca la rotación de una `Matrix4`.

## Transform

| Método | Espacio |
| --- | --- |
| `SetPosition` / `GetPosition` | Local |
| `SetScale` / `GetScale` | Local |
| `SetRotation(Quaternion)` o `SetRotation(Vector3 radianes)` | Local |
| `Translate` / `Rotate` | Incremental local |
| `GetForward` / `GetRight` / `GetUp` | Ejes del transform |
| `GetModelMatrix` | Matriz local |
| `GameObject::GetWorldMatrix` | Cadena de padres |

## Color

`Math::Color` y, en varios componentes, `Math::Vector4` como RGBA. `LightComponent::color` es `Color`. `MeshRenderer::colorRef` es `Vector4`.

## Matrix4

La usa el renderer (view, projection, mundo) y el gizmo del editor. Para mover un objeto de juego preferible `Transform`. Descomponer una matriz mundo a mano solo hace falta si estás escribiendo una herramienta.
