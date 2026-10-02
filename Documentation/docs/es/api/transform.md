# Transform

`RTBEngine::Scene::Transform` guarda la pose **local**. La pose mundo está en `GameObject`. El constructor deja posición en cero, rotación en identidad y escala en uno. Los ángulos de `SetRotation(Vector3)` y `Rotate(Vector3)` son radianes. El Inspector y el `.lua` de escena muestran grados y el loader los convierte; el C++ de un script no.

## SetPosition

```cpp
void SetPosition(const Math::Vector3& position);
```

Sustituye la posición local y marca sucia la matriz local y la matriz mundo del dueño y de sus hijos.

- `position`: posición en el espacio del padre. En la raíz, es la posición mundo.

## SetRotation

```cpp
void SetRotation(const Math::Quaternion& rotation);
```

Sustituye la rotación local por ese quaternion y marca sucias las matrices.

- `rotation`: rotación local. La identidad deja el forward en +Z.

## SetRotation

```cpp
void SetRotation(const Math::Vector3& eulerAngles);
```

Sustituye la rotación local construyendo un quaternion con `Quaternion::FromEulerAngles`. El vector es pitch, yaw y roll en **radianes**, convención YXZ. Pasar grados gira unas 57 veces de más.

- `eulerAngles`: pitch (`x`), yaw (`y`) y roll (`z`) en radianes.

```cpp
constexpr float deg = 3.14159265f / 180.0f;
auto& t = GetOwner()->GetTransform();
t.SetRotation(RTBEngine::Math::Vector3(0.0f, 90.0f * deg, 0.0f));
```

## SetScale

```cpp
void SetScale(const Math::Vector3& scale);
```

Sustituye la escala local y marca sucias las matrices. El default de un transform nuevo es `(1, 1, 1)`. Escala 0 aplasta el eje.

- `scale`: escala local por eje.

## GetPosition

```cpp
const Math::Vector3& GetPosition() const;
```

**Devuelve** la posición local. No es la posición mundo si hay padre: esa es `GameObject::GetWorldPosition`.

## GetRotation

```cpp
const Math::Quaternion& GetRotation() const;
```

**Devuelve** la rotación local. No hay `GetEulerAngles`: si necesitas ángulos, parte del quaternion.

## GetScale

```cpp
const Math::Vector3& GetScale() const;
```

**Devuelve** la escala local.

## GetForward

```cpp
Math::Vector3 GetForward() const;
```

Dirección local +Z después de la rotación, normalizada. Con rotación identidad apunta a +Z mundo si el objeto está en la raíz.

**Devuelve** el vector forward local, longitud 1.

## GetRight

```cpp
Math::Vector3 GetRight() const;
```

Dirección derecha local, normalizada. Sigue la convención de yaw negado del motor, la misma que usa la cámara al leer la matriz mundo.

**Devuelve** el vector right local, longitud 1.

## GetUp

```cpp
Math::Vector3 GetUp() const;
```

Dirección arriba local, normalizada. Con rotación identidad es +Y.

**Devuelve** el vector up local, longitud 1.

## Translate

```cpp
void Translate(const Math::Vector3& translation);
```

Suma `translation` a la posición local. No rota el desplazamiento: un vector mundo hay que pasarlo ya en el espacio en el que quieres moverte. Si avanzas “hacia delante”, multiplica `GetForward()` por la distancia.

- `translation`: delta en el mismo espacio que la posición local.

```cpp
auto& t = GetOwner()->GetTransform();
t.Translate(t.GetForward() * (4.0f * deltaTime));
```

## Rotate

```cpp
void Rotate(const Math::Quaternion& rotation);
```

Antepone `rotation` a la rotación local (`rotation * actual`). No sustituye la pose: la acumula.

- `rotation`: delta de rotación.

## Rotate

```cpp
void Rotate(const Math::Vector3& eulerAngles);
```

Igual que `Rotate(Quaternion)`, construyendo el delta con `FromEulerAngles`. Pitch, yaw y roll van en **radianes**.

- `eulerAngles`: delta pitch/yaw/roll en radianes.

```cpp
constexpr float deg = 3.14159265f / 180.0f;
t.Rotate(RTBEngine::Math::Vector3(0.0f, 90.0f * deg * deltaTime, 0.0f));
```

## GetModelMatrix

```cpp
Math::Matrix4 GetModelMatrix() const;
```

Matriz local: traslación por rotación por escala. Se reconstruye solo si la pose local cambió. La matriz mundo, con padres, es `GameObject::GetWorldMatrix`.

**Devuelve** la matriz modelo local.
