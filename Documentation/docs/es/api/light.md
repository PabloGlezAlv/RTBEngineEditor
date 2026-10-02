# Light

`RTBEngine::Scene::LightComponent`. `LightType`: `Directional`, `Point`, `Spot`.

Campos: `lightType` `Point`, `color` blanco, `intensity` 1, `range` 10, `spotAngle` 45, `spotInnerAngle` 30, `syncPosition` true, `syncDirection` true. `range` no afecta a `Directional`. Los ángulos del foco están en grados.

## GetLight

```cpp
Rendering::Light* GetLight() const;
```

El objeto de iluminación que el pase de luces lee. Puede ser null antes de que el componente lo cree en el arranque.

**Devuelve** la luz de render, o null si todavía no existe.

## SetLight

```cpp
void SetLight(std::unique_ptr<Rendering::Light> light);
```

Sustituye el objeto de render. El componente se queda dueño del puntero. Pasar un unique vacío deja la componente sin luz hasta el siguiente arranque. El gameplay normal no lo llama: asigna los campos reflejados y `SyncProperties`.

- `light`: luz nueva. El unique se mueve; el llamador ya no lo posee.

## SyncProperties

```cpp
void SyncProperties();
```

Copia tipo, color, intensidad, alcance, ángulos del foco y la sincronía con el transform al objeto de render. Sin esto, escribir `lightType` o `spotAngle` en C++ cambia el campo reflejado y la luz dibujada sigue con el valor anterior hasta el próximo `OnValidate`.

```cpp
auto* light = GetOwner()->GetComponent<RTBEngine::Scene::LightComponent>();
light->lightType = RTBEngine::Rendering::LightType::Spot;
light->spotAngle = 35.0f;
light->SyncProperties();
```
