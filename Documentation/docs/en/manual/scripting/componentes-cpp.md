# C++ components

Game scripts are C++ components in `Assets/Scripts/`. They compile into `GameScripts.dll`. The editor loads that DLL at startup and every time you press **Compile Scripts**.

Registration is ABI-safe: the script sends flat descriptors and the engine builds its own metadata. If you change macros, the bridge, or public headers, regenerate the SDK before rebuilding `GameScripts`.

## Template

`Spinner.h`:

```cpp
#pragma once
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class Spinner : public RTBEngine::Scene::Component {
public:
    Spinner();
    ~Spinner() override;

    Spinner(const Spinner&) = delete;
    Spinner& operator=(const Spinner&) = delete;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnDestroy() override;

    float speedRef = 90.0f;

    RTB_COMPONENT(Spinner)

private:
    float speed = 90.0f;
};
```

`Spinner.cpp`:

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

void Spinner::OnAwake() {}

void Spinner::OnStart() {
    speed = speedRef;
}

void Spinner::OnUpdate(float deltaTime) {
    const float radians = speed * deltaTime * 3.14159265f / 180.0f;
    GetOwner()->GetTransform().Rotate(
        RTBEngine::Math::Vector3(0.0f, radians, 0.0f));
}

void Spinner::OnFixedUpdate(float) {}
void Spinner::OnDestroy() {}
```

`using ThisClass = Spinner` is required: the property macros measure the field on `ThisClass`.

## Creating the file from the editor

Content Browser → right-click → **New → C++ Component**. It generates the `.h` / `.cpp` pair and the project picks them up on its own.

## Building

**Compile Scripts** runs MSBuild on the active project's `GameScripts.vcxproj`, Debug configuration, x64 platform. While it runs, the button stays on *Compiling...* and you cannot enter Play.

When it succeeds, the editor does `FreeLibrary` + `LoadLibrary`. Static initializers register the types again. If the DLL is locked because Play has it loaded, stop Play first.

You can also build by hand:

```bat
RTBEngineEditor\RTBEngineEditor\GameScripts\build.bat
```

## Folders in the sample game

| Folder | Contents |
| --- | --- |
| `Character/` | Definition, catalog, stats |
| `Player/Gameplay/` | Match pawn, attacks, ammo |
| `Player/Preview/` | Menu view |
| `Enemy/` | Enemy AI and animation |
| `Combat/` | Projectiles, health, damage numbers |
| `Session/` | Round, session, message ids |
| `Online/` | Lobby |
| `UI/` | Menus and HUD |
| `VFX/` | Beams and effects |

## Typed references

```cpp
RTBEngine::Animation::Animator* animator = nullptr;
std::string idleAnimationFbx;

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_FBX(idleAnimationFbx)
RTB_END_REGISTER(ThirdPersonCharacterController)
```

`animator` is assigned by dragging the GameObject that has the `Animator`. The FBX slot only accepts `.fbx` and stores the logical path `Assets/...`.

The full macro list is in [Properties](reflexion.md). Memory rules across DLLs are in [DLL boundary](frontera-dll.md).
