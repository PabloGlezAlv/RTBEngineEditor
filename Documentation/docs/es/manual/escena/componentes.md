# Componentes

Un componente es un trozo de comportamiento o de datos colgado de un `GameObject`. Los built-in (malla, luz, cámara, cuerpo, collider, audio, partículas, navegación, red, UI) y los tuyos en `GameScripts` heredan de `RTBEngine::Scene::Component`.

## Superficie

```cpp
class RTB_API Component {
public:
    virtual void OnAwake() {}
    virtual void OnEnable() {}
    virtual void OnStart() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnFixedUpdate(float fixedDeltaTime) {}
    virtual void OnLateUpdate(float deltaTime) {}
    virtual void OnDisable() {}
    virtual void OnDestroy() {}
    virtual void OnValidate() {}

    virtual void OnCollisionEnter(const Physics::CollisionInfo& collision) {}
    virtual void OnCollisionStay(const Physics::CollisionInfo& collision) {}
    virtual void OnCollisionExit(const Physics::CollisionInfo& collision) {}
    virtual void OnTriggerEnter(const Physics::CollisionInfo& collision) {}
    virtual void OnTriggerStay(const Physics::CollisionInfo& collision) {}
    virtual void OnTriggerExit(const Physics::CollisionInfo& collision) {}

    GameObject* GetOwner() const;
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
};
```

`GetOwner()` es el `GameObject` dueño. `SetEnabled(false)` dispara `OnDisable` sin quitar el componente ni destruir el objeto.

## Built-in que vas a añadir en el Inspector

| Componente | Para qué |
| --- | --- |
| `MeshRenderer` | Malla, textura, color, shader |
| `CameraComponent` | Cámara de juego. `isMainCamera` alimenta la Game View |
| `LightComponent` | Directional, Point o Spot |
| `RigidBodyComponent` | Static, Dynamic o Kinematic |
| `BoxColliderComponent` / `SphereColliderComponent` | Volumen. `isTrigger` no empuja, avisa |
| `AudioSourceComponent` | Clip FMOD, volumen, pitch, loop |
| `ParticleSystem` | Emisor. `Play`, `Stop`, `Pause`, `Emit` |
| `TrailRenderer` | Estela |
| `VolumeComponent` | Niebla de distancia y volumétrica local |
| `Animator` | Clips FBX y pose de huesos |
| `NavGridComponent` / `NavAgentComponent` | Grid horneado y agente |
| `NetworkIdentity` / `NetworkTransform` | Identidad de red y pose replicada |
| `Canvas`, `UIButton`, `UIText`, `UIImage`, `UIPanel` | UI |

Cada uno tiene página en la [Scripting API](../../api/index.md).

## Activar y desactivar

Un objeto inactivo en jerarquía no recibe update. Un componente desactivado tampoco. `OnEnable` / `OnDisable` emparejan esas transiciones. No hace falta comprobar `IsEnabled()` al principio de `OnUpdate`: si estás ahí, el tick está activo.

`OnValidate` corre cuando el Inspector o el loader cambian una propiedad reflejada. Sirve para clampear rangos y refrescar cachés. También corre en Edit.

## Dibujo y preview opcionales

Si `WantsTransparentRender()` devuelve true, la escena llama `OnTransparentRender` en el paso de transparencias. El motor no conoce tu efecto: solo pregunta.

Si `WantsEditModeSimulate()` devuelve true, la Scene View llama `OnEditModeSimulate` sin entrar en Play. `ParticleSystem` y el preview del `Animator` usan esa vía.

El orden exacto de las llamadas está en [Ciclo de vida](../scripting/ciclo-de-vida.md).
