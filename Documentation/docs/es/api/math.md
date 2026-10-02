# Math

Namespace `RTBEngine::Math`. Son POD: se pueden pasar por valor entre `GameScripts` y el motor. `Vector3` tiene campos públicos `x`, `y`, `z`. `Vector2` usa `x`, `y`. `Vector4` añade `w`.

## Vector3

```cpp
Vector3(float x, float y, float z);
```

Construye el vector con esos componentes. El default de un `Vector3` sin argumentos es cero en los tres ejes.

- `x`, `y`, `z`: componentes.

## Dot

```cpp
float Dot(const Vector3& other) const;
```

Producto escalar. 0 si son perpendiculares. El signo dice si `other` mira al mismo hemisferio.

- `other`: el otro vector. No se normaliza dentro.

**Devuelve** `x*other.x + y*other.y + z*other.z`.

## Cross

```cpp
Vector3 Cross(const Vector3& other) const;
```

Producto vectorial. La dirección es perpendicular a los dos. Si son paralelos, el resultado es cero.

- `other`: el otro vector.

**Devuelve** el vector perpendicular. No está normalizado.

## Length

```cpp
float Length() const;
```

**Devuelve** la longitud. Cero si el vector es cero. Para comparar distancias sin la raíz, usa `LengthSquared`.

## LengthSquared

```cpp
float LengthSquared() const;
```

**Devuelve** la longitud al cuadrado. Cero si el vector es cero. Más barato que `Length` cuando solo comparas contra un umbral al cuadrado.

## Normalized

```cpp
Vector3 Normalized() const;
```

**Devuelve** una copia de longitud 1. No modifica este vector. Un vector cero no tiene dirección: el resultado no es un eje útil.

## FromEulerAngles

```cpp
static Quaternion FromEulerAngles(float pitch, float yaw, float roll);
static Quaternion FromEulerAngles(const Vector3& euler);
```

Construye un quaternion con pitch, yaw y roll en **radianes**, convención YXZ, forward +Z en identidad. Pasar grados gira unas 57 veces de más. La sobrecarga con `Vector3` usa `x` como pitch, `y` como yaw y `z` como roll. `Transform::SetRotation(Vector3)` llama a esta función.

- `pitch`, `yaw`, `roll`: radianes.
- `euler`: los tres ángulos en radianes.

**Devuelve** el quaternion. Cero en los tres ángulos es la identidad.

```cpp
constexpr float deg = 3.14159265f / 180.0f;
auto q = RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, 90.0f * deg, 0.0f);
```

## FromMatrix

```cpp
static Quaternion FromMatrix(const Matrix4& mat);
```

Extrae la rotación de una matriz. La traslación y la escala no entran en el quaternion. Una matriz sin una rotación limpia (escala no uniforme fuerte) no da un quaternion estable.

- `mat`: matriz de la que se lee la rotación.

**Devuelve** el quaternion de esa rotación.

## Slerp

```cpp
static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
```

Interpolación esférica de `a` hacia `b`. `t` 0 devuelve `a`, `t` 1 devuelve `b`. Entre medias el giro es a velocidad constante. Es la que hay que usar para rotaciones.

- `a`: origen.
- `b`: destino.
- `t`: 0 en `a`, 1 en `b`.

**Devuelve** el quaternion interpolado.

## Lerp

```cpp
static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
```

Interpolación lineal de los componentes y luego normaliza. Más barata que `Slerp` y menos uniforme en giros grandes. `t` 0 es `a` y `t` 1 es `b`.

- `a`, `b`: extremos.
- `t`: mezcla.

**Devuelve** el quaternion normalizado.

## Color

```cpp
Color(float r, float g, float b, float a);
```

Color que usa `LightComponent`, con componentes en 0–1. Varios renderers guardan el color como `Vector4` RGBA en lugar de `Color`. Alfa 0 es transparente y 1 es opaco.

- `r`, `g`, `b`, `a`: canales. Fuera de 0–1 el shader recibe el valor tal cual.
