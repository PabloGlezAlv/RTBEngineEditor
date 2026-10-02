# Bucle de frame

`Application::Run()` repite este orden. El editor lo envuelve: en Edit no llama al update de juego, y pinta la escena dos veces (cámara del editor y cámara de juego).

```text
deltaTime = ComputeDelta()
ProcessInput()                         SDL → InputManager
SceneManager::ProcessPendingSceneLoad()
Update(deltaTime)
  Scene::Update                        OnUpdate de componentes
  ECS::Tick(Simulation)                vuelo denso, queries
  FixedUpdate + Physics                Bullet
  Scene::LateUpdate                    impacto, trail, pool
  ECS::Tick(Presentation)              LocalTransform → GameObject
RenderShadowPass()
RenderGeometryPass()
postproceso (bloom, niebla, volúmenes)
presentación / SwapBuffers
```

## Tiempo

| Callback | Cuándo |
| --- | --- |
| `OnUpdate` | Cada frame, con `deltaTime` escalado |
| `OnFixedUpdate` | Paso fijo de física |
| `OnLateUpdate` | Después de física y del tick de simulación ECS |

`ComponentTimeMode::Unscaled` saca a ese componente de la escala de tiempo. `SetUpdateTickEnabled(false)` deja el componente vivo pero fuera del tick.

## Cambio de escena

`LoadScene` puede usarse cuando no estás a mitad de un update. Desde un componente, pide el cambio y deja que el motor lo aplique en el punto seguro:

```cpp
RTBEngine::Scene::SceneManager::GetInstance()
    .RequestSceneLoad("Assets/Scenes/MainMenu.lua");
```

La ruta es lógica, con barras `/`, relativa al proyecto. El player lee la escena inicial de `game.cfg`.

## Qué hace el editor con el mismo bucle

| Estado | Update de juego | Qué se ve |
| --- | --- | --- |
| Edit | No | Scene View: cámara del editor, grid, colliders, nav debug |
| Play | Sí | Scene View y Game View |
| Pause | Tiempo parado | El estado de runtime se conserva |

En Edit, partículas con `simulateInEditMode` y animators pueden previsualizarse. Los scripts de juego no ejecutan `OnUpdate`.

Al pasar a Play, el editor reinicia la física de la escena activa. Al parar, descarta el runtime y vuelve a cargar el archivo.

## Orden de un proyectil híbrido

El `OnUpdate` del componente de escena no paga el vuelo. El sistema ECS integra y hace el `SphereCast`. `OnLateUpdate` es el sitio donde el juego reacciona al impacto, porque la simulación de ese frame ya terminó y el transform visual ya puede sincronizarse en el tick de presentación.
