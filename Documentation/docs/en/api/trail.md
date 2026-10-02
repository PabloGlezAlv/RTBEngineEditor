# TrailRenderer

`RTBEngine::Scene::TrailRenderer` draws a ribbon from a list of points in **world space**. It does not follow the transform by itself: the script appends the points. Drawing needs at least two points, an enabled component, an owner that is active in the hierarchy, and `visible == true`. `visible` defaults to `false`.

Use it on projectiles and attack previews. When the object goes back to the [ObjectPool](objectpool.md), call `ClearPoints` and `SetVisible(false)`. Otherwise the next `Acquire` shows the previous segment.

The scene calls `Render` in the trail pass. A script does not need to.

Fields a script writes directly (they are not methods): `width` (default `0.15`), `startWidth` and `endWidth` (`-1` uses `width`), `color`, `fadeAlphaAlongLength`, `blendMode` (`Alpha` or `Additive`), `alignment` (`FlatXZ` or `CameraFacing`), `softEdge` (`0` is a hard edge, `1` is a soft falloff across the whole width), `texture`, `uvScrollSpeed`, and `uvTilesPerMeter`.

## SetPoints

```cpp
void SetPoints(const std::vector<Math::Vector3>& newPoints);
```

Replaces the whole polyline. The ribbon is redrawn on the next `Render` with those vertices.

- `newPoints`: world-space points, in order. An empty vector leaves the trail empty.

## SetPoints

```cpp
void SetPoints(const Math::Vector3* newPoints, std::size_t count);
```

Same as the vector overload, reading `count` points from the pointer. If `newPoints` is null or `count` is 0, the list is cleared and the call returns.

- `newPoints`: first point, or null to clear.
- `count`: how many points to copy.

## SetPoint

```cpp
bool SetPoint(std::size_t index, const Math::Vector3& point);
```

Rewrites a point that already exists. It does not grow the list.

- `index`: index from 0. If it is `>= GetPointCount()`, nothing changes.
- `point`: new world position.

**Returns** `true` when the index existed.

## AddPoint

```cpp
void AddPoint(const Math::Vector3& point);
```

Appends a point. The first point draws nothing: `Render` returns when there are fewer than two.

- `point`: world position.

```cpp
auto* trail = GetOwner()->GetComponent<RTBEngine::Scene::TrailRenderer>();
trail->ClearPoints();
trail->SetVisible(true);
trail->AddPoint(GetOwner()->GetWorldPosition());
```

## ClearPoints

```cpp
void ClearPoints();
```

Removes every point. The ribbon disappears on the next frame even if `visible` stays `true`.

## GetPoints

```cpp
const std::vector<Math::Vector3>& GetPoints() const;
```

**Returns** the current list, in the order the points were added. The reference belongs to the component: do not keep it past the call if you are about to mutate the trail.

## GetPointCount

```cpp
std::size_t GetPointCount() const;
```

**Returns** how many points there are. `0` is an empty trail.

## SetVisible

```cpp
void SetVisible(bool isVisible);
```

Turns drawing on or off. It does not clear the points. With `false`, `Render` emits no geometry.

- `isVisible`: `true` to draw. The component defaults to `false`.

## IsVisible

```cpp
bool IsVisible() const;
```

**Returns** the visibility flag. It does not check whether there are enough points or whether the owner is active.

## SetGlobalAlphaScale

```cpp
void SetGlobalAlphaScale(float scale);
```

Multiplies the alpha of every vertex. A negative value is stored as `0` (the ribbon becomes invisible; the points stay). The default is `1`.

- `scale`: alpha factor. `0` kills the color without clearing the polyline.

## GetGlobalAlphaScale

```cpp
float GetGlobalAlphaScale() const;
```

**Returns** the alpha factor. `1` is full opacity relative to `color.w`.

## Render

```cpp
void Render(Rendering::Camera* camera);
```

The scene calls this. It draws the ribbon when the component is enabled, `visible` is true, the owner is active, `camera` is not null, and there are at least two points. If any of that is missing, it returns without drawing. `width` below the internal minimum is raised to that minimum.

- `camera`: the frame camera. Null does not draw.
