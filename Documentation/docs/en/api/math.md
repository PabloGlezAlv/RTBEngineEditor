# Math

Namespace `RTBEngine::Math`. These types are POD: they can be passed by value between `GameScripts` and the engine. `Vector3` has public fields `x`, `y`, `z`. `Vector2` uses `x`, `y`. `Vector4` adds `w`.

## Vector3

```cpp
Vector3(float x, float y, float z);
```

Builds the vector from those components. A default `Vector3` with no arguments is zero on all three axes.

- `x`, `y`, `z`: components.

## Dot

```cpp
float Dot(const Vector3& other) const;
```

Dot product. 0 when they are perpendicular. The sign says whether `other` faces the same hemisphere.

- `other`: the other vector. It is not normalized inside.

**Returns** `x*other.x + y*other.y + z*other.z`.

## Cross

```cpp
Vector3 Cross(const Vector3& other) const;
```

Cross product. The direction is perpendicular to both. Parallel vectors produce zero.

- `other`: the other vector.

**Returns** the perpendicular vector. It is not normalized.

## Length

```cpp
float Length() const;
```

**Returns** the length. Zero if the vector is zero. To compare distances without the square root, use `LengthSquared`.

## LengthSquared

```cpp
float LengthSquared() const;
```

**Returns** the squared length. Zero if the vector is zero. Cheaper than `Length` when you only compare against a squared threshold.

## Normalized

```cpp
Vector3 Normalized() const;
```

**Returns** a copy of length 1. It does not modify this vector. A zero vector has no direction: the result is not a useful axis.

## FromEulerAngles

```cpp
static Quaternion FromEulerAngles(float pitch, float yaw, float roll);
static Quaternion FromEulerAngles(const Vector3& euler);
```

Builds a quaternion from pitch, yaw, and roll in **radians**, YXZ convention, forward +Z at identity. Passing degrees rotates about 57 times too far. The `Vector3` overload uses `x` as pitch, `y` as yaw, and `z` as roll. `Transform::SetRotation(Vector3)` calls this function.

- `pitch`, `yaw`, `roll`: radians.
- `euler`: the three angles in radians.

**Returns** the quaternion. Zero on all three angles is identity.

```cpp
constexpr float deg = 3.14159265f / 180.0f;
auto q = RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, 90.0f * deg, 0.0f);
```

## FromMatrix

```cpp
static Quaternion FromMatrix(const Matrix4& mat);
```

Extracts the rotation from a matrix. Translation and scale do not enter the quaternion. A matrix without a clean rotation (strong non-uniform scale) does not yield a stable quaternion.

- `mat`: matrix whose rotation is read.

**Returns** the quaternion of that rotation.

## Slerp

```cpp
static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
```

Spherical interpolation from `a` toward `b`. `t` 0 returns `a`, `t` 1 returns `b`. In between the turn has constant speed. This is the one to use for rotations.

- `a`: start.
- `b`: end.
- `t`: 0 at `a`, 1 at `b`.

**Returns** the interpolated quaternion.

## Lerp

```cpp
static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
```

Linear interpolation of the components, then a normalize. Cheaper than `Slerp` and less uniform on large turns. `t` 0 is `a` and `t` 1 is `b`.

- `a`, `b`: endpoints.
- `t`: blend.

**Returns** the normalized quaternion.

## Color

```cpp
Color(float r, float g, float b, float a);
```

Color used by `LightComponent`, with components in 0–1. Several renderers store color as an RGBA `Vector4` instead of `Color`. Alpha 0 is transparent and 1 is opaque.

- `r`, `g`, `b`, `a`: channels. Outside 0–1 the shader receives the value as given.
