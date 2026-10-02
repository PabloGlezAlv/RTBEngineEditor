# Application

`RTBEngine::Core::Application` posee la ventana, el dispositivo y el bucle. El editor la envuelve. Un script de juego no la construye: el player y el editor ya lo hacen. Desde un componente, el cambio de escena es `SceneManager::RequestSceneLoad`, no `Application::Run`.

## ApplicationConfig

Se pasa al construir. No tiene métodos.

| Campo | Default |
| --- | --- |
| `title` | `"RTBEngine App"` |
| `width` | `1280` |
| `height` | `720` |
| `fullscreen` | `false` |
| `vsync` | `true` |
| `targetFPS` | `60` |
| `startScene` | `""` |

El player rellena estos campos desde `game.cfg`. El editor no usa `startScene` de aquí: abre la escena del `.rtbproj`.

## Run

```cpp
void Run();
```

El bucle del player: input, `Update`, render y present hasta que alguien pide salir. Llamarlo desde un componente reentra en el frame que ya se está ejecutando. El orden que ven tus componentes está en [Bucle de frame](../manual/bucle.md).

## ResetPhysics

```cpp
void ResetPhysics();
```

Tira los objetos del mundo Bullet actual y reinicia el sistema de física, si existen. No crea cuerpos nuevos. El editor lo llama al salir de Play. No lo llames desde `OnUpdate`: el paso de física de ese frame todavía está usando esos cuerpos.

## InitializePhysicsForScene

```cpp
void InitializePhysicsForScene(Scene::Scene* scene);
```

Recorre los objetos de la escena y crea el cuerpo Bullet de cada uno que tenga rigidbody y colliders. Después de esto `RigidBodyComponent::HasRigidBody()` puede ser true; en `OnAwake`, antes de esta llamada, es false. Una escena null, o un sistema de física que aún no existe, vuelve sin hacer nada. El editor lo llama al entrar en Play. Un script no lo llama en mitad del frame.

- `scene`: escena cuyos objetos deben recibir cuerpo. Null no hace nada.

## RenderShadowPass

```cpp
void RenderShadowPass(Scene::Scene* scene);
```

Dibuja el mapa de sombras de las luces direccionales que tienen sombras, si el proyecto las tiene encendidas. Si las sombras están apagadas en los ajustes, o no hay shader `"shadow"`, vuelve sin dibujar. Ocurre antes del pase de geometría, fuera de tus componentes. La escena tiene que ser la activa: el pase lee sus luces.

- `scene`: escena a sombrear. La llama el bucle, no un script.

Para un pase transparente propio implementa `WantsTransparentRender` y `OnTransparentRender` en el componente. Ver [Component](component.md).
