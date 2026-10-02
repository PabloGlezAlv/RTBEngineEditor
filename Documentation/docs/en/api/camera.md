# Camera

`RTBEngine::Scene::CameraComponent` is the game camera. The editor Scene View camera is a different class. `Scene::GetActiveCamera()` returns the `Rendering::Camera` of the component with `isMainCamera`, or the first camera if none is marked.

Fields: `fov` 45, `nearClip` 0.1, `farClip` 1000, `projectionType` `Perspective`, `orthographicSize` 10, `syncWithTransform` true, `isMainCamera` false.

## GetCamera

```cpp
Rendering::Camera* GetCamera() const;
```

The render object that actually projects. It can be null before the component creates the camera during startup.

**Returns** the render camera, or null if it does not exist yet.

## SetFOV

```cpp
void SetFOV(float fov);
```

Vertical field of view in degrees for perspective projection, pushed to the render camera when it already exists. The reflected default is 45. It does not affect orthographic projection: that uses `orthographicSize`.

- `fov`: vertical angle in degrees.

## GetFOV

```cpp
float GetFOV() const;
```

**Returns** the fov stored on the component, in degrees. The default is 45.

## SetNearPlane

```cpp
void SetNearPlane(float nearPlane);
```

Near-plane distance, and updates the render camera when it exists. The default is 0.1. A value greater than or equal to the far plane makes the projection unusable.

- `nearPlane`: near distance.

## GetNearPlane

```cpp
float GetNearPlane() const;
```

**Returns** the near plane. The default is 0.1.

## SetFarPlane

```cpp
void SetFarPlane(float farPlane);
```

Far-plane distance, and updates the render camera when it exists. The default is 1000.

- `farPlane`: far distance.

## GetFarPlane

```cpp
float GetFarPlane() const;
```

**Returns** the far plane. The default is 1000.

## SetProjectionType

```cpp
void SetProjectionType(Rendering::ProjectionType type);
```

`Perspective` uses `fov`. `Orthographic` uses `orthographicSize`. If the render camera already exists, the change applies immediately.

- `type`: `Rendering::ProjectionType::Perspective` or `Orthographic`.

## GetProjectionType

```cpp
Rendering::ProjectionType GetProjectionType() const;
```

**Returns** the projection type. The default is perspective.

## SetOrthographicSize

```cpp
void SetOrthographicSize(float size);
```

Half of the visible height in world units when projection is orthographic. The default is 10. While perspective is selected, storing it does not change the picture until you switch the type.

- `size`: orthographic half-height.

## GetOrthographicSize

```cpp
float GetOrthographicSize() const;
```

**Returns** the orthographic size. The default is 10.

## SetAspectRatio

```cpp
void SetAspectRatio(float aspectRatio);
```

Width/height ratio used by the projection. If the render camera does not exist yet, there is nowhere to write it: the value lives on `Rendering::Camera`, not on a reflected field of the component. The engine updates it to the view size; change it only when you need a fixed ratio.

- `aspectRatio`: width divided by height.
