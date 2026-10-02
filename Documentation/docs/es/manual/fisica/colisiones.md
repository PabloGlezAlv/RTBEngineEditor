# Colisiones

Los contactos llegan a **cualquier** componente del mismo `GameObject` que tenga el cuerpo y el collider.

```cpp
void Bumper::OnCollisionEnter(const RTBEngine::Physics::CollisionInfo& hit) {
    const char* name = hit.otherObject ? hit.otherObject->GetNameCStr() : "?";
    char msg[256];
    snprintf(msg, sizeof(msg), "Choque con %s", name);
    RTB_INFO(msg);
}
```

`CollisionInfo`:

| Campo | Tipo | Contenido |
| --- | --- | --- |
| `otherObject` | `GameObject*` | El otro objeto de escena |
| `contactPoint` | `Vector3` | Punto de contacto en mundo |
| `contactNormal` | `Vector3` | Normal del contacto |
| `penetrationDepth` | `float` | Penetración |

`OnCollisionStay` repite mientras el contacto dura. `OnCollisionExit` avisa al separarse.

## Triggers

`BoxColliderComponent::SetIsTrigger(true)` (campo reflejado `isTrigger`) no resuelve empuje. Dispara:

- `OnTriggerEnter`
- `OnTriggerStay`
- `OnTriggerExit`

El argumento es el mismo `CollisionInfo`. Sirve para zonas de daño, pickups y volúmenes de gameplay. El objeto trigger suele ser `Kinematic` o `Static`.

## Caja y esfera

`BoxColliderComponent`:

```cpp
collider->SetSize(RTBEngine::Math::Vector3(1.0f, 2.0f, 1.0f));
collider->FitToOwnerMesh();
bool trigger = collider->IsTrigger();
```

`size` es el tamaño completo reflejado (default `1,1,1`). `FitToOwnerMesh` ajusta tamaño y centro a los bounds del `MeshRenderer` del dueño, en espacio local.

`SphereColliderComponent` sigue el mismo patrón de trigger y de vida atada al `PhysicsWorld` de la escena.

## Qué tiene que estar en el objeto

| Quieres | En el mismo GameObject |
| --- | --- |
| Contacto sólido | `RigidBodyComponent` + collider con `isTrigger = false` |
| Zona | `RigidBodyComponent` + collider con `isTrigger = true` |
| Callbacks | Cualquier script hermano. No hace falta que el script sea el collider |

Si el otro objeto no tiene collider, `otherObject` no aparecerá: Bullet no inventa contactos.

## Capas

Dos objetos solo se prueban si la matriz de capas lo permite. La capa del objeto se cambia con `SetCollisionLayer` o `SetCollisionLayerByName`. La matriz se edita en [Capas](capas.md).
