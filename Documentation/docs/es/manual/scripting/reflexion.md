# Propiedades

El Inspector y el Lua de escena leen metadata, no tus campos privados por arte de magia. Declaras el componente con `RTB_COMPONENT` y listas los campos entre `RTB_REGISTER_COMPONENT` y `RTB_END_REGISTER`.

```cpp
using ThisClass = Spinner;

RTB_REGISTER_COMPONENT(Spinner)
    RTB_PROPERTY(speedRef)
    RTB_PROPERTY_RANGE(turnSpeed, -360.0f, 360.0f)
    RTB_PROPERTY_ENUM(bodyType, Static, Dynamic, Kinematic)
    RTB_PROPERTY_COLOR(tint)
    RTB_PROPERTY_TEXTURE(albedo)
    RTB_PROPERTY_MESH(mesh)
    RTB_PROPERTY_AUDIOCLIP(clip)
    RTB_PROPERTY_FONT(font)
    RTB_PROPERTY_FBX(idleAnimationFbx)
    RTB_PROPERTY_GAMEOBJECT(target)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_STRING_LIST(names)
    RTB_PROPERTY_GAMEOBJECT_LIST(waypoints)
    RTB_PROPERTY_HIDDEN(runtimeCache)
    RTB_PROPERTY_READONLY(debugCount)
RTB_END_REGISTER(Spinner)
```

## Qué pinta el Inspector

| Macro | Widget |
| --- | --- |
| `RTB_PROPERTY` | Según el tipo del campo: bool, int, float, string, Vector2/3/4 |
| `RTB_PROPERTY_RANGE` | Slider entre min y max |
| `RTB_PROPERTY_ENUM` | Combo con los nombres que pases |
| `RTB_PROPERTY_COLOR` | Selector de color |
| `RTB_PROPERTY_TEXTURE` / `MESH` / `AUDIOCLIP` / `FONT` / `FBX` | Ruta de asset, botón `...` y drag desde el Content Browser |
| `RTB_PROPERTY_GAMEOBJECT` | Suelta un objeto de la jerarquía |
| `RTB_PROPERTY_COMPONENT` | Resuelve ese componente en el objeto soltado |
| `RTB_PROPERTY_HIDDEN` | Se serializa, no se enseña |
| `RTB_PROPERTY_READONLY` | Se ve, no se edita |

`RTB_PROPERTY_ASSET_PATH(campo, "Tipo")` es la forma genérica si el atajo no cubre tu extensión. `RTB_PROPERTY_DATA_ASSET` apunta a un `.rtbasset`.

Hay variantes `RTB_PROPERTY_SERIALIZED` y `RTB_PROPERTY_SERIALIZED_RANGE` cuando quieres forzar la ruta de serialización. Para un campo normal de script, `RTB_PROPERTY` basta.

## Tipos que viajan bien

El campo tiene que ser un POD o un tipo que el bridge conozca: `bool`, enteros, `float`, `double`, `std::string` **dentro** del componente (el motor lee el offset; tú no devuelves el `std::string` por la frontera), `Vector2/3/4`, `Quaternion`, `Color`, punteros a `GameObject` o a un componente.

No registres un `std::vector` crudo que no tenga macro de lista. Usa `RTB_PROPERTY_STRING_LIST`, `RTB_PROPERTY_GAMEOBJECT_LIST` o `RTB_PROPERTY_COMPONENT_LIST`.

## Data assets

Un recurso de datos (definición de personaje, por ejemplo) no es un componente de escena. Usa `RTB_DATA_ASSET`, `RTB_REGISTER_DATA_ASSET` y `RTB_END_REGISTER_DATA_ASSET`. El archivo es `.rtbasset` y el Inspector lo abre al seleccionarlo en el navegador.

## Herencia

Si tu tipo extiende otro componente reflejado:

```cpp
RTB_INHERITS(BaseComponent)
```

Hasta tres bases: `RTB_INHERITS(A, B)` o `RTB_INHERITS(A, B, C)`. Va junto a `RTB_COMPONENT` en la clase.

## Después de un cambio

El Inspector llama `OnValidate()` y marca la escena o el prefab como dirty. En `OnValidate` puedes copiar proxies a estado interno, pero no asumas que las referencias por UUID ya están resueltas si todavía estás en medio de la carga: eso ocurre antes de `OnStart`.
