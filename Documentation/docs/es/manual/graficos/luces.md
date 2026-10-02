# Luces y sombras

`LightComponent` crea una luz y, si `syncPosition` / `syncDirection` están activos, copia el transform cada frame.

| Campo | Default | Efecto |
| --- | --- | --- |
| `lightType` | `Point` | `Directional`, `Point` o `Spot` |
| `color` | Blanco | Color de la luz |
| `intensity` | `1` | Intensidad |
| `range` | `10` | Alcance de point y spot |
| `spotAngle` | `45` | Cono exterior, grados |
| `spotInnerAngle` | `30` | Cono interior |

```cpp
auto* light = GetOwner()->GetComponent<RTBEngine::Scene::LightComponent>();
light->lightType = RTBEngine::Rendering::LightType::Directional;
light->intensity = 1.2f;
light->SyncProperties();
```

## Cómo orientarlas

- **Directional**: la rotación del transform define la dirección. No usa `range`. Es la luz de sol y la que abre el shadow map.
- **Point**: la posición ilumina en todas direcciones hasta `range`.
- **Spot**: posición + dirección del transform, recortada por `spotInnerAngle` y `spotAngle`.

`GetLight()` devuelve el objeto de render (`Rendering::Light`) cuando el componente ya lo ha creado. Igual que el cuerpo rígido, no lo asumas en `OnAwake`.

## Sombras

El frame hace `RenderShadowPass` antes de la geometría. El mapa lo alimenta la direccional. Los objetos que el frustum y las flags estáticas dejan fuera no escriben sombra.

En el editor, la Scene View también ejecuta el shadow pass para que la vista de autor se parezca a la de juego.

## Ambiente y proyecto

La luz de ambiente, el volumen DDGI, las sombras y la niebla global no están en el `LightComponent`. Están en `lighting.ini`, editado desde **Window → Project Settings**. Ver [Ajustes del proyecto](../../editor/ajustes.md) y [Postproceso](postproceso.md).

El modo prefab crea una luz de editor (`__PrefabEditorLight`) solo para ver el asset. No se guarda en el `.prefab`.
