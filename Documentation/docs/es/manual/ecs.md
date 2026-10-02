# ECS

`RTBEngine::ECS::World` es un registro de entidades con sparse sets. Convive con la escena. No tiene jerarquía ni Inspector.

```cpp
RTBEngine::ECS::World* world = RTBEngine::ECS::World::GetActive();
RTBEngine::ECS::Entity entity = world->Create();

world->Add<LocalTransform>(entity, local);
bool alive = world->IsAlive(entity);
LocalTransform* tr = world->TryGet<LocalTransform>(entity);
bool has = world->Has<LocalTransform>(entity);

world->Remove<LocalTransform>(entity);
world->Destroy(entity);
world->Clear();
```

`Add` hace emplace y devuelve la referencia al componente dentro del storage. `TryGet` devuelve null si esa entidad no lo tiene. `Clear` corre al descargar la escena; los sistemas registrados se quedan.

## Cuándo usarlo

Úsalo para muchos objetos del mismo tipo y vida corta, donde un `OnUpdate` virtual por unidad sale caro. El caso que ya trae el motor es el vuelo de proyectiles:

| Pieza ECS | Rol |
| --- | --- |
| `LocalTransform` | Pose plana, sin padre |
| Estado de vuelo | Integración en el sistema |
| Consulta de impacto | Sphere cast en el tick de simulación |
| `VisualLink` | Qué `GameObject` representa a la entidad |

El componente de escena sigue existiendo. Su `OnUpdate` no integra mientras la entidad vive. `OnLateUpdate` lee el impacto y hace daño, partículas y audio. `Tick(Presentation)` copia la pose al transform visual.

## Quién registra los sistemas

El motor ofrece `World`, el scheduler y el renderer instanciado. Los sistemas de juego (proyectiles, bancos) se registran desde `GameScripts` con `RTBScripts_InitializeEcs`. Si añades un sistema, va en la DLL de scripts, no dentro de un `GameObject`.

## Tick

`Application` llama `Tick(Simulation)` después de `Scene::Update` y antes de resolver el puente en `LateUpdate`. `Tick(Presentation)` va después, para que el dibujo vea la pose nueva.

No muevas un `GameObject` de personaje a una entidad para “hacerlo ECS”. Pierdes prefab, reflexión, red y callbacks de Bullet, y el personaje no era el cuello de botella.
