# RigidBody

`RTBEngine::Scene::RigidBodyComponent`. El script no incluye cabeceras de Bullet: se queda en `Physics::RigidBody*`.

Campos: `mass` 1, `friction` 0.5, `restitution` 0, `bodyType` `Physics::RigidBodyType::Dynamic`, `freezeRotationX/Y/Z` false.

`RigidBodyType`: `Static` no se mueve por fuerzas, `Dynamic` sí, `Kinematic` lo mueves tú y empuja a los dinámicos.

## SetRigidBody

```cpp
void SetRigidBody(std::unique_ptr<Physics::RigidBody> rb);
```

Entrega el cuerpo que la física de la escena construyó. El componente pasa a poseerlo. Lo llama `InitializePhysicsForScene`, no el gameplay. Un unique vacío deja el componente sin cuerpo.

- `rb`: cuerpo nuevo. Se mueve; el llamador ya no lo posee.

## GetRigidBody

```cpp
Physics::RigidBody* GetRigidBody() const;
```

El cuerpo vivo. Es null hasta que la escena inicializa la física, así que en `OnAwake` todavía no está. Léelo desde `OnStart`.

**Devuelve** el `Physics::RigidBody`, o null.

## HasRigidBody

```cpp
bool HasRigidBody() const;
```

**Devuelve** true cuando `GetRigidBody()` no es null. False hasta `InitializePhysicsForScene`. En `OnAwake` es false aunque el componente esté en la escena.
