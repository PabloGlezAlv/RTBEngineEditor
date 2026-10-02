# Tu primer juego

Este recorrido usa solo el editor: un suelo, un cubo que cae y un componente que lo gira.

## 1. Cubo y suelo

1. Abre el editor. Carga la escena de inicio del `.rtbproj`.
2. En **Scene Hierarchy**, clic derecho en el vacío → **Create Cube**.
3. Otra vez clic derecho → **Create Plane** para el suelo.
4. Selecciona el plano. En el Inspector pon la escala en algo ancho, por ejemplo `10, 1, 10`.
5. En el cubo, **+ Add Component** → `RigidBodyComponent`. Deja `bodyType` en `Dynamic`.
6. **+ Add Component** → `BoxColliderComponent`.
7. En el plano: `RigidBodyComponent` con `bodyType` = `Static` y un `BoxColliderComponent`.

Pulsa **Play**. El cubo cae y se detiene en el suelo. **Stop** recarga la escena de disco y tira los cambios de runtime.

!!! warning "Si atraviesa el suelo"
    Un suelo visible no colisiona solo. Necesita `RigidBodyComponent` estático y un collider. Un dinámico sin collider tampoco genera contactos.

## 2. Un componente C++

En **Content Browser**, clic derecho → **New → C++ Component**. Nombre: `Spinner`.

El editor crea `Assets/Scripts/Spinner.h` y `Spinner.cpp`. Sustituye el cuerpo por esto. Los ángulos de `Transform` en C++ van en **radianes**.

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

En el header, `speedRef` es el campo reflejado y `speed` el valor de juego copiado en `OnStart`.

Pulsa **Compile Scripts** (solo en Edit). Selecciona el cubo, **+ Add Component**, busca `Spinner`. Ajusta `speedRef` a `90`. **Play**: el cubo gira mientras cae.

Guarda con `Ctrl+S`. La escena es un archivo `.lua` en `Assets/Scenes/`.

## 3. Exportar

`Ctrl+B` abre el diálogo de build. Nombre, carpeta de salida, escena inicial y tamaño de ventana. **Build** copia `RTBPlayer`, DLLs, `Assets/`, `Default/` y escribe `game.cfg`. El player arranca en la escena elegida.

El detalle de cada panel está en [La interfaz](../editor/interfaz.md). El contrato del componente, en [Componentes C++](scripting/componentes-cpp.md).
