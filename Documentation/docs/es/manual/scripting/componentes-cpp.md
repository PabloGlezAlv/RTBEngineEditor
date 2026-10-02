# Componentes C++

Los scripts de juego son componentes C++ en `Assets/Scripts/`. Se compilan en `GameScripts.dll`. El editor carga esa DLL al arrancar y cada vez que pulsas **Compile Scripts**.

El registro es ABI-safe: el script manda descriptores planos y el motor construye su propia metadata. Si cambias macros, el bridge o headers públicos, regenera el SDK antes de recompilar `GameScripts`.

## Plantilla

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

`using ThisClass = Spinner` es obligatorio: las macros de propiedad miden el campo sobre `ThisClass`.

## Crear el archivo desde el editor

Content Browser → clic derecho → **New → C++ Component**. Genera el par `.h` / `.cpp` y el proyecto lo recoge solo.

## Compilar

**Compile Scripts** llama a MSBuild sobre el `GameScripts.vcxproj` del proyecto activo, configuración Debug, plataforma x64. Mientras corre, el botón queda en *Compiling...* y no puedes entrar en Play.

Al terminar bien, el editor hace `FreeLibrary` + `LoadLibrary`. Los inicializadores estáticos vuelven a registrar los tipos. Si la DLL está bloqueada porque Play la tiene cargada, para Play antes.

También puedes compilar a mano:

```bat
RTBEngineEditor\RTBEngineEditor\GameScripts\build.bat
```

## Carpetas del juego de ejemplo

| Carpeta | Contenido |
| --- | --- |
| `Character/` | Definición, catálogo, stats |
| `Player/Gameplay/` | Peón de partida, ataques, munición |
| `Player/Preview/` | Vista del menú |
| `Enemy/` | IA y animación de enemigos |
| `Combat/` | Proyectiles, vida, números de daño |
| `Session/` | Ronda, sesión, ids de mensaje |
| `Online/` | Lobby |
| `UI/` | Menús y HUD |
| `VFX/` | Haces y efectos |

## Referencias tipadas

```cpp
RTBEngine::Animation::Animator* animator = nullptr;
std::string idleAnimationFbx;

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_FBX(idleAnimationFbx)
RTB_END_REGISTER(ThirdPersonCharacterController)
```

`animator` se asigna arrastrando el GameObject que tiene el `Animator`. El FBX solo acepta `.fbx` y guarda la ruta lógica `Assets/...`.

La lista completa de macros está en [Propiedades](reflexion.md). Las reglas de memoria entre DLLs, en [Frontera DLL](frontera-dll.md).
