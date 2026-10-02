# Math

Types in `RTBEngine::Math` are structs of floats. They cross the DLL boundary without trouble.

## Vector3

```cpp
RTBEngine::Math::Vector3 a(1.0f, 0.0f, 0.0f);
RTBEngine::Math::Vector3 b(0.0f, 0.0f, 1.0f);
float d = a.Dot(b);
RTBEngine::Math::Vector3 c = a.Cross(b);
float len = a.Length();
RTBEngine::Math::Vector3 n = a.Normalized();
```

`Vector2` and `Vector4` exist in the same style (`x`, `y`, `z`, `w`).

## Quaternion and Euler

`Quaternion::FromEulerAngles(pitch, yaw, roll)` and `FromEulerAngles(Vector3)` work in **radians**. Order is YXZ, with forward +Z when yaw and pitch are 0. Yaw is negated inside the function so the default forward looks toward +Z.

`Transform::SetRotation(Vector3)` and `Rotate(Vector3)` pass that vector to `FromEulerAngles` without converting. They are radians too.

The Inspector and the scene Lua speak in **degrees**. The Lua binding multiplies by `pi/180` before calling the quaternion. If you copy a `90` from the Inspector into C++, convert it:

```cpp
constexpr float kDegToRad = 3.14159265f / 180.0f;
transform.SetRotation(RTBEngine::Math::Vector3(0.0f, 90.0f * kDegToRad, 0.0f));
```

`Quaternion::Slerp` and `Lerp` interpolate. `FromMatrix` extracts the rotation from a `Matrix4`.

## Transform

| Method | Space |
| --- | --- |
| `SetPosition` / `GetPosition` | Local |
| `SetScale` / `GetScale` | Local |
| `SetRotation(Quaternion)` or `SetRotation(Vector3 radianes)` | Local |
| `Translate` / `Rotate` | Local increment |
| `GetForward` / `GetRight` / `GetUp` | Transform axes |
| `GetModelMatrix` | Local matrix |
| `GameObject::GetWorldMatrix` | Parent chain |

## Color

`Math::Color` and, on several components, `Math::Vector4` as RGBA. `LightComponent::color` is `Color`. `MeshRenderer::colorRef` is `Vector4`.

## Matrix4

The renderer uses it (view, projection, world), and so does the editor gizmo. To move a game object, prefer `Transform`. Decomposing a world matrix by hand is only needed if you are writing a tool.
