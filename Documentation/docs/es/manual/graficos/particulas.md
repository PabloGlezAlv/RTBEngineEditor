# Partículas

`ParticleSystem` es un componente de escena. En el Inspector tiene botones **Play**, **Pause**, **Stop** y **Burst**, y luego los campos reflejados.

| Campo | Default | Uso |
| --- | --- | --- |
| `maxParticles` | `256` | Tope de partículas vivas |
| `emissionRate` | `40` | Partículas por segundo |
| `playOnAwake` | `true` | Empieza al activarse |
| `simulateInEditMode` | `true` | Se ve en Scene View sin Play |
| `burstCount` | `10` | Cuántas suelta el botón Burst |

```cpp
auto* fx = GetOwner()->GetComponent<RTBEngine::Scene::ParticleSystem>();
fx->Play();
fx->Emit(24);
fx->Pause();
fx->Stop();
```

Crear una desde la jerarquía: clic derecho → **Create Particle System**. El objeto nace con un emisor en cono, `playOnAwake` y `simulateInEditMode`.

En Edit, `EditorApplication` llama al preview de los sistemas que tienen simulación en edición y están en reproducción. Los scripts de juego siguen sin ejecutarse.

La textura del sistema se asigna como el resto de assets, arrastrando desde el Content Browser.

## Estelas

`TrailRenderer` dibuja la cinta detrás del objeto. Combínalo con un proyectil del pool: al `Release`, la estela tiene que resetearse para no arrastrar el viaje anterior. El puente de proyectiles lo hace en `OnLateUpdate`, cuando el vuelo ECS ya terminó.

## Orden en el frame

Las partículas opacas entran en el paso de geometría. Si tu componente de juego quiere un pase transparente propio, devuelve true en `WantsTransparentRender` e implementa `OnTransparentRender`. El motor no tiene un caso especial por nombre de clase.
