# Your first game

This pass uses only the editor: a floor, a falling cube, and a component that spins it.

## 1. Cube and floor

1. Open the editor. It loads the start scene from the `.rtbproj`.
2. In **Scene Hierarchy**, right-click empty space → **Create Cube**.
3. Right-click again → **Create Plane** for the floor.
4. Select the plane. In the Inspector set the scale wide, for example `10, 1, 10`.
5. On the cube, **+ Add Component** → `RigidBodyComponent`. Leave `bodyType` as `Dynamic`.
6. **+ Add Component** → `BoxColliderComponent`.
7. On the plane: `RigidBodyComponent` with `bodyType` = `Static`, plus a `BoxColliderComponent`.

Press **Play**. The cube falls and stops on the floor. **Stop** reloads the scene from disk and drops runtime changes.

!!! warning "If it falls through the floor"
    A visible floor does not collide by itself. It needs a static `RigidBodyComponent` and a collider. A dynamic body without a collider does not produce useful contacts either.

## 2. A C++ component

In the **Content Browser**, right-click → **New → C++ Component**. Name: `Spinner`.

The editor creates `Assets/Scripts/Spinner.h` and `Spinner.cpp`. Replace the body with this. `Transform` angles in C++ are **radians**.

```cpp
#include "Spinner.h"
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Transform.h>

using ThisClass = Spinner;

RTB_REGISTER_COMPONENT(Spinner)
    RTB_PROPERTY_RANGE(speedRef, -360.0f, 360.0f)
RTB_END_REGISTER(Spinner)

Spinner::Spinner() {}
Spinner::~Spinner() {}

void Spinner::OnStart() {
    speed = speedRef;
}

void Spinner::OnUpdate(float deltaTime) {
    const float radians = speed * deltaTime * 3.14159265f / 180.0f;
    GetOwner()->GetTransform().Rotate(RTBEngine::Math::Vector3(0.0f, radians, 0.0f));
}
```

In the header, `speedRef` is the reflected field and `speed` is the gameplay value copied in `OnStart`.

Press **Compile Scripts** (Edit mode only). Select the cube, **+ Add Component**, find `Spinner`. Set `speedRef` to `90`. **Play**: the cube spins while it falls.

Save with `Ctrl+S`. The scene is a `.lua` file under `Assets/Scenes/`.

## 3. Export

`Ctrl+B` opens the build dialog. Name, output folder, start scene, and window size. **Build** copies `RTBPlayer`, the DLLs, `Assets/`, `Default/`, and writes `game.cfg`. The player starts in the scene you picked.

Each panel is described in [The interface](../editor/interfaz.md). The component contract is in [C++ components](scripting/componentes-cpp.md).
