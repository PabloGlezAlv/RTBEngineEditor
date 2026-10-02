# Cuerpos rígidos

La física es Bullet 3.25, envuelta por `PhysicsWorld` y `PhysicsSystem`. En el editor no simula en Edit. En Play, el editor llama `ResetPhysics` e `InitializePhysicsForScene` al entrar.

El componente de autor es `RigidBodyComponent`.

| Campo | Significado |
| --- | --- |
| `mass` | Masa. Default `1` |
| `friction` | Fricción. Default `0.5` |
| `restitution` | Rebote. Default `0` |
| `bodyType` | `Static`, `Dynamic` o `Kinematic` |
| `freezeRotationX/Y/Z` | Bloquea el giro en ese eje |

```cpp
auto* body = GetOwner()->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
if (body && body->HasRigidBody()) {
    RTBEngine::Physics::RigidBody* rb = body->GetRigidBody();
}
```

## Tipos

| Tipo | Se mueve por |
| --- | --- |
| `Static` | Nada. Suelos, muros |
| `Dynamic` | Fuerzas y gravedad |
| `Kinematic` | Tú mueves el transform. Empuja a los dinámicos, la gravedad no lo tira |

Un visual sin `RigidBodyComponent` no participa. Un dinámico sin collider cae pero no genera contactos útiles contra el mundo.

## Suelo que sí sostiene

1. Plano o cubo con `MeshRenderer`.
2. `RigidBodyComponent`, `bodyType = Static`.
3. `BoxColliderComponent` con un `size` que cubra el plano.

El cubo que cae lleva `Dynamic`, masa > 0 y su propio collider.

## Ciclo

Los cuerpos Bullet se crean cuando la escena ya está cargada (`InitializePhysicsForScene`), no en el constructor del componente. En `OnAwake` el `GetRigidBody()` puede ser nulo. En `OnStart`, después de esa inicialización, ya está.

Al cambiar de escena se destruye el mundo físico anterior. No guardes `btRigidBody*` ni punteros crudos de Bullet en tus scripts. Quédate en `RigidBody*` del motor y suéltalo en `OnDestroy` si guardaste una copia.

## Depuración en el editor

Selecciona el objeto en Play. La Scene View dibuja el wireframe del collider. La Game View no lo dibuja. Si el objeto no cae, mira que `bodyType` sea `Dynamic` y que Play esté realmente en marcha (en Pause el tiempo está parado).
