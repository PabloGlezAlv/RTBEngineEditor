# TrailRenderer

`RTBEngine::Scene::TrailRenderer` dibuja una cinta a partir de una lista de puntos en **espacio mundo**. No sigue al transform sola: el script añade los puntos. Hacen falta al menos dos puntos, el componente enabled, el dueño activo en la jerarquía y `visible == true`. El default de `visible` es `false`.

Úsalo en proyectiles y en previsualizaciones de ataque. Al devolver el objeto al [ObjectPool](objectpool.md), llama a `ClearPoints` y `SetVisible(false)`. Si no, el siguiente `Acquire` enseña el tramo anterior.

La escena llama a `Render` en el pase de estelas. Desde un script no hace falta.

Campos que el script escribe directo (no son métodos): `width` (default `0.15`), `startWidth` y `endWidth` (`-1` usa `width`), `color`, `fadeAlphaAlongLength`, `blendMode` (`Alpha` o `Additive`), `alignment` (`FlatXZ` o `CameraFacing`), `softEdge` (`0` arista dura, `1` caída suave en todo el ancho), `texture`, `uvScrollSpeed` y `uvTilesPerMeter`.

## SetPoints

```cpp
void SetPoints(const std::vector<Math::Vector3>& newPoints);
```

Sustituye toda la polilínea. La cinta se redibuja en el siguiente `Render` con esos vértices.

- `newPoints`: puntos en espacio mundo, en orden. Un vector vacío deja la estela vacía.

## SetPoints

```cpp
void SetPoints(const Math::Vector3* newPoints, std::size_t count);
```

Igual que la sobrecarga de vector, leyendo `count` puntos desde el puntero. Si `newPoints` es nulo o `count` es 0, vacía la lista y vuelve.

- `newPoints`: primer punto, o nulo para vaciar.
- `count`: cuántos puntos copiar.

## SetPoint

```cpp
bool SetPoint(std::size_t index, const Math::Vector3& point);
```

Reescribe un punto que ya existe. No agranda la lista.

- `index`: índice desde 0. Si es `>= GetPointCount()`, no cambia nada.
- `point`: nueva posición mundo.

**Devuelve** `true` si el índice existía.

## AddPoint

```cpp
void AddPoint(const Math::Vector3& point);
```

Añade un punto al final. El primer punto no dibuja nada: `Render` sale si hay menos de dos.

- `point`: posición mundo.

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

Borra todos los puntos. La cinta desaparece en el siguiente frame aunque `visible` siga en `true`.

## GetPoints

```cpp
const std::vector<Math::Vector3>& GetPoints() const;
```

**Devuelve** la lista actual, en el orden en que se añadieron. La referencia es la del componente: no la guardes más allá de la llamada si vas a mutar la estela.

## GetPointCount

```cpp
std::size_t GetPointCount() const;
```

**Devuelve** cuántos puntos hay. `0` es una estela vacía.

## SetVisible

```cpp
void SetVisible(bool isVisible);
```

Enciende o apaga el dibujo. No borra los puntos. Con `false`, `Render` no emite geometría.

- `isVisible`: `true` para dibujar. El default del componente es `false`.

## IsVisible

```cpp
bool IsVisible() const;
```

**Devuelve** el flag de visibilidad. No mira si hay puntos suficientes ni si el dueño está activo.

## SetGlobalAlphaScale

```cpp
void SetGlobalAlphaScale(float scale);
```

Multiplica el alfa de todos los vértices. Un valor negativo se guarda como `0` (la cinta queda invisible, los puntos siguen). El default es `1`.

- `scale`: factor de alfa. `0` apaga el color sin vaciar la polilínea.

## GetGlobalAlphaScale

```cpp
float GetGlobalAlphaScale() const;
```

**Devuelve** el factor de alfa. `1` es opacidad completa respecto a `color.w`.

## Render

```cpp
void Render(Rendering::Camera* camera);
```

La escena lo llama. Dibuja la cinta si el componente está enabled, `visible` es true, el dueño está activo, `camera` no es nulo y hay al menos dos puntos. Si falta cualquiera de eso, vuelve sin dibujar. `width` por debajo del mínimo interno se sube a ese mínimo.

- `camera`: cámara del frame. Nula no dibuja.
