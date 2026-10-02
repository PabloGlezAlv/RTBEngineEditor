# Colliders

Los callbacks de contacto están en [Component](component.md). Sin `RigidBodyComponent` en el mismo objeto, Bullet no entrega esos mensajes al script.

## CollisionInfo

`RTBEngine::Physics::CollisionInfo` viaja en `OnCollisionEnter` / `OnTriggerEnter` y sus Stay/Exit.

- `otherObject`: el otro `GameObject`. Puede ser null si el contacto no tiene objeto de escena.
- `contactPoint`: punto de contacto en mundo.
- `contactNormal`: normal del contacto.
- `penetrationDepth`: cuánto se solapan. En un trigger es el solape, no una respuesta de empuje.

## SetSize

```cpp
void BoxColliderComponent::SetSize(const Math::Vector3& size);
```

Tamaño de la caja en espacio local del dueño. El campo reflejado `size` arranca en `(1, 1, 1)`. Un eje a 0 deja esa dimensión plana.

- `size`: ancho, alto y fondo locales.

## GetSize

```cpp
Math::Vector3 BoxColliderComponent::GetSize() const;
```

**Devuelve** el tamaño actual de la caja. Tras `FitToOwnerMesh` es el tamaño ajustado a la malla, no el `(1, 1, 1)` inicial.

## GetCenterOffset

```cpp
Math::Vector3 BoxColliderComponent::GetCenterOffset() const;
```

Centro de la caja respecto al origen del dueño. Si el collider de física aún no existe, devuelve `(0, 0, 0)`.

**Devuelve** el offset del centro, o cero si no hay collider.

## FitToOwnerMesh

```cpp
void BoxColliderComponent::FitToOwnerMesh();
```

Ajusta tamaño y centro a los bounds locales del `MeshRenderer` del dueño. Sin dueño, sin collider de física, o sin `MeshRenderer`, no hace nada. Si la malla es un solo mesh y el puntero es null, tampoco. En un multi-mesh con bounds degenerados (mínimo igual a máximo) vuelve sin cambiar. Después escala el tamaño ajustado por la escala local del transform.

## SetIsTrigger

```cpp
void BoxColliderComponent::SetIsTrigger(bool trigger);
void SphereColliderComponent::SetIsTrigger(bool trigger);
```

Guarda el flag. Con `true` el volumen es trigger: no empuja, y el motor llama `OnTriggerEnter` / `Stay` / `Exit` en vez de los mensajes de colisión sólida cuando la física construye o sincroniza el cuerpo. El campo reflejado `isTrigger` arranca en false. La caja y la esfera escriben el mismo campo; no recrean el cuerpo Bullet en esta llamada.

- `trigger`: true para volumen de aviso.

## IsTrigger

```cpp
bool BoxColliderComponent::IsTrigger() const;
bool SphereColliderComponent::IsTrigger() const;
```

**Devuelve** true si el volumen es trigger. El default es false.

## SetRadius

```cpp
void SphereColliderComponent::SetRadius(float r);
```

Radio de la esfera en espacio local. Es el equivalente del `size` de la caja. Un radio 0 no genera un volumen útil.

- `r`: radio local.

## GetRadius

```cpp
float SphereColliderComponent::GetRadius() const;
```

**Devuelve** el radio local de la esfera.
