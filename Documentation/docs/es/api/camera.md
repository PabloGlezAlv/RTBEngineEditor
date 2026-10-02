# Camera

`RTBEngine::Scene::CameraComponent` es la cámara de juego. La de la Scene View del editor es otra clase. `Scene::GetActiveCamera()` devuelve el `Rendering::Camera` de la componente con `isMainCamera`, o la primera cámara si ninguna está marcada.

Campos: `fov` 45, `nearClip` 0.1, `farClip` 1000, `projectionType` `Perspective`, `orthographicSize` 10, `syncWithTransform` true, `isMainCamera` false.

## GetCamera

```cpp
Rendering::Camera* GetCamera() const;
```

El objeto de render que realmente proyecta. Puede ser null antes de que el componente cree la cámara en su arranque.

**Devuelve** la cámara de render, o null si aún no existe.

## SetFOV

```cpp
void SetFOV(float fov);
```

Grados del campo de visión vertical en proyección perspectiva, y lo empuja a la cámara de render si ya existe. El default reflejado es 45. No afecta a la proyección ortográfica: ahí manda `orthographicSize`.

- `fov`: ángulo vertical en grados.

## GetFOV

```cpp
float GetFOV() const;
```

**Devuelve** el fov guardado en el componente, en grados. El default es 45.

## SetNearPlane

```cpp
void SetNearPlane(float nearPlane);
```

Distancia del plano cercano y, si la cámara de render existe, la actualiza. El default es 0.1. Un valor mayor o igual que el plano lejano deja la proyección inutilizable.

- `nearPlane`: distancia cercana.

## GetNearPlane

```cpp
float GetNearPlane() const;
```

**Devuelve** el plano cercano. El default es 0.1.

## SetFarPlane

```cpp
void SetFarPlane(float farPlane);
```

Distancia del plano lejano y, si la cámara de render existe, la actualiza. El default es 1000.

- `farPlane`: distancia lejana.

## GetFarPlane

```cpp
float GetFarPlane() const;
```

**Devuelve** el plano lejano. El default es 1000.

## SetProjectionType

```cpp
void SetProjectionType(Rendering::ProjectionType type);
```

`Perspective` usa `fov`. `Orthographic` usa `orthographicSize`. Si la cámara de render ya existe, el cambio se aplica en el acto.

- `type`: `Rendering::ProjectionType::Perspective` o `Orthographic`.

## GetProjectionType

```cpp
Rendering::ProjectionType GetProjectionType() const;
```

**Devuelve** el tipo de proyección. El default es perspectiva.

## SetOrthographicSize

```cpp
void SetOrthographicSize(float size);
```

Media altura visible en unidades de mundo cuando la proyección es ortográfica. El default es 10. Con perspectiva guardado, no cambia la imagen hasta que cambies el tipo.

- `size`: media altura ortográfica.

## GetOrthographicSize

```cpp
float GetOrthographicSize() const;
```

**Devuelve** el tamaño ortográfico. El default es 10.

## SetAspectRatio

```cpp
void SetAspectRatio(float aspectRatio);
```

Relación ancho/alto que usa la proyección. Si la cámara de render aún no existe, no tiene dónde escribirlo: el valor vive en `Rendering::Camera`, no en un campo reflejado del componente. El motor lo actualiza al tamaño de la vista; cámbialo solo si necesitas una relación fija.

- `aspectRatio`: ancho dividido por alto.
