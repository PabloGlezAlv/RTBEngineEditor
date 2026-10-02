# Transform

`RTBEngine::Scene::Transform` stores the **local** pose. World pose lives on `GameObject`. The constructor leaves position at zero, rotation at identity, and scale at one. Angles for `SetRotation(Vector3)` and `Rotate(Vector3)` are radians. The Inspector and scene `.lua` show degrees and the loader converts them; script C++ does not.

## SetPosition

```cpp
void SetPosition(const Math::Vector3& position);
```

Replaces the local position and dirties the local matrix and the world matrix of the owner and its children.

- `position`: position in parent space. At the root, that is the world position.

## SetRotation

```cpp
void SetRotation(const Math::Quaternion& rotation);
```

Replaces the local rotation with that quaternion and dirties the matrices.

- `rotation`: local rotation. Identity leaves forward at +Z.

## SetRotation

```cpp
void SetRotation(const Math::Vector3& eulerAngles);
```

Replaces the local rotation by building a quaternion with `Quaternion::FromEulerAngles`. The vector is pitch, yaw, and roll in **radians**, YXZ convention. Passing degrees rotates about 57 times too far.

- `eulerAngles`: pitch (`x`), yaw (`y`), and roll (`z`) in radians.

```cpp
constexpr float deg = 3.14159265f / 180.0f;
auto& t = GetOwner()->GetTransform();
t.SetRotation(RTBEngine::Math::Vector3(0.0f, 90.0f * deg, 0.0f));
```

## SetScale

```cpp
void SetScale(const Math::Vector3& scale);
```

Replaces the local scale and dirties the matrices. A new transform defaults to `(1, 1, 1)`. A scale of 0 flattens that axis.

- `scale`: local scale per axis.

## GetPosition

```cpp
const Math::Vector3& GetPosition() const;
```

**Returns** the local position. It is not the world position when there is a parent: that is `GameObject::GetWorldPosition`.

## GetRotation

```cpp
const Math::Quaternion& GetRotation() const;
```

**Returns** the local rotation. There is no `GetEulerAngles`: if you need angles, start from the quaternion.

## GetScale

```cpp
const Math::Vector3& GetScale() const;
```

**Returns** the local scale.

## GetForward

```cpp
Math::Vector3 GetForward() const;
```

Local +Z after rotation, normalized. With identity rotation it points at world +Z when the object is at the root.

**Returns** the local forward vector, length 1.

## GetRight

```cpp
Math::Vector3 GetRight() const;
```

Local right direction, normalized. It follows the engine's negated-yaw convention, the same one the camera uses when reading the world matrix.

**Returns** the local right vector, length 1.

## GetUp

```cpp
Math::Vector3 GetUp() const;
```

Local up direction, normalized. With identity rotation it is +Y.

**Returns** the local up vector, length 1.

## Translate

```cpp
void Translate(const Math::Vector3& translation);
```

Adds `translation` to the local position. It does not rotate the offset: a world vector must already be in the space you want to move in. To move “forward”, multiply `GetForward()` by the distance.

- `translation`: delta in the same space as the local position.

```cpp
auto& t = GetOwner()->GetTransform();
t.Translate(t.GetForward() * (4.0f * deltaTime));
```

## Rotate

```cpp
void Rotate(const Math::Quaternion& rotation);
```

Prepends `rotation` to the local rotation (`rotation * current`). It accumulates; it does not replace the pose.

- `rotation`: rotation delta.

## Rotate

```cpp
void Rotate(const Math::Vector3& eulerAngles);
```

Same as `Rotate(Quaternion)`, building the delta with `FromEulerAngles`. Pitch, yaw, and roll are **radians**.

- `eulerAngles`: pitch/yaw/roll delta in radians.

```cpp
constexpr float deg = 3.14159265f / 180.0f;
t.Rotate(RTBEngine::Math::Vector3(0.0f, 90.0f * deg * deltaTime, 0.0f));
```

## GetModelMatrix

```cpp
Math::Matrix4 GetModelMatrix() const;
```

Local matrix: translation times rotation times scale. It is rebuilt only when the local pose changed. The world matrix, with parents, is `GameObject::GetWorldMatrix`.

**Returns** the local model matrix.
