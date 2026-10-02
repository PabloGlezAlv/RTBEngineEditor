# Frame loop

`Application::Run()` repeats this order. The editor wraps it: in Edit it skips the game update, and it draws the scene twice (the editor camera and the game camera).

```text
deltaTime = ComputeDelta()
ProcessInput()                         SDL → InputManager
SceneManager::ProcessPendingSceneLoad()
Update(deltaTime)
  Scene::Update                        component OnUpdate
  ECS::Tick(Simulation)                dense flight, queries
  FixedUpdate + Physics                Bullet
  Scene::LateUpdate                    impact, trail, pool
  ECS::Tick(Presentation)              LocalTransform → GameObject
RenderShadowPass()
RenderGeometryPass()
post-process (bloom, fog, volumes)
present / SwapBuffers
```

## Time

| Callback | When |
| --- | --- |
| `OnUpdate` | Every frame, with scaled `deltaTime` |
| `OnFixedUpdate` | The fixed physics step |
| `OnLateUpdate` | After physics and the ECS simulation tick |

`ComponentTimeMode::Unscaled` pulls that component out of time scale. `SetUpdateTickEnabled(false)` leaves the component alive but out of the tick.

## Changing scenes

`LoadScene` is for when you are not halfway through an update. From a component, request the change and let the engine apply it at the safe point:

```cpp
RTBEngine::Scene::SceneManager::GetInstance()
    .RequestSceneLoad("Assets/Scenes/MainMenu.lua");
```

The path is logical, uses `/`, and is relative to the project. The player reads the startup scene from `game.cfg`.

## What the editor does with the same loop

| State | Game update | What you see |
| --- | --- | --- |
| Edit | No | Scene View: editor camera, grid, colliders, nav debug |
| Play | Yes | Scene View and Game View |
| Pause | Time stopped | Runtime state is kept |

In Edit, particles with `simulateInEditMode` and animators can preview. Game scripts do not run `OnUpdate`.

Entering Play resets physics for the active scene. Stopping Play throws away the runtime and reloads the file.

## Order for a hybrid projectile

The scene component's `OnUpdate` does not pay for the flight. The ECS system integrates motion and runs the `SphereCast`. `OnLateUpdate` is where the game reacts to the impact: that frame's simulation is already finished, and the visual transform can be synced on the presentation tick.
