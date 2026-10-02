# Application

`RTBEngine::Core::Application` owns the window, the device, and the loop. The editor wraps it. A game script does not construct it: the player and the editor already do. From a component, a scene change is `SceneManager::RequestSceneLoad`, not `Application::Run`.

## ApplicationConfig

Passed to the constructor. It has no methods.

| Field | Default |
| --- | --- |
| `title` | `"RTBEngine App"` |
| `width` | `1280` |
| `height` | `720` |
| `fullscreen` | `false` |
| `vsync` | `true` |
| `targetFPS` | `60` |
| `startScene` | `""` |

The player fills these fields from `game.cfg`. The editor does not use `startScene` from here: it opens the scene in the `.rtbproj`.

## Run

```cpp
void Run();
```

The player loop: input, `Update`, render, and present until something asks to quit. Calling it from a component re-enters the frame that is already running. The order your components see is in [Frame loop](../manual/bucle.md).

## ResetPhysics

```cpp
void ResetPhysics();
```

Drops the objects in the current Bullet world and resets the physics system, when those exist. It does not create new bodies. The editor calls it when leaving Play. Do not call it from `OnUpdate`: that frame's physics step is still using those bodies.

## InitializePhysicsForScene

```cpp
void InitializePhysicsForScene(Scene::Scene* scene);
```

Walks the scene's objects and creates the Bullet body for each one that has a rigid body and colliders. After this, `RigidBodyComponent::HasRigidBody()` can be true; in `OnAwake`, before this call, it is false. A null scene, or a physics system that does not exist yet, returns without doing anything. The editor calls it when entering Play. A script does not call it in the middle of a frame.

- `scene`: scene whose objects should receive a body. Null does nothing.

## RenderShadowPass

```cpp
void RenderShadowPass(Scene::Scene* scene);
```

Draws the shadow map of directional lights that cast shadows, when the project has shadows enabled. If shadows are off in the settings, or there is no `"shadow"` shader, it returns without drawing. It runs before the geometry pass, outside your components. The scene has to be the active one: the pass reads its lights.

- `scene`: scene to shadow. The loop calls this, not a script.

For a custom transparent pass, implement `WantsTransparentRender` and `OnTransparentRender` on the component. See [Component](component.md).
