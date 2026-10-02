# Reflection

Macros de `RTBEngine/Reflection/PropertyMacros.h`. Hace falta `using ThisClass = TuClase;` antes del registro. El bloque construye un `TypeInfo` y lo mete en `TypeRegistry`. El Inspector lista `GetRegisteredTypes()` en **Add Component**. Un tipo sin este bloque no aparece y no se guarda en el `.lua`.

## RTB_COMPONENT

```cpp
RTB_COMPONENT(ClassName)
```

Dentro de la clase. Implementa `GetTypeName`, `GetTypeInfo` y la fábrica que `AddComponent` usa para crear y destruir el objeto en el módulo correcto. Sin esta macro el tipo no se puede añadir por nombre.

- `ClassName`: el nombre de la clase, el mismo que verá el `.lua`.

## RTB_INHERITS

```cpp
RTB_INHERITS(Base)
RTB_INHERITS(A, B)
RTB_INHERITS(A, B, C)
```

Declara de qué componentes hereda, para que `GetComponent<Base>()` encuentre la derivada. Una, dos o tres bases. Va en la clase, junto a `RTB_COMPONENT`.

- `Base`, `A`, `B`, `C`: clases base que también son `Component`.

## RTB_DATA_ASSET

```cpp
RTB_DATA_ASSET(ClassName)
```

Marca una clase de datos (no un componente de escena) para que el registro de assets la conozca. No la añade al menú **Add Component**.

- `ClassName`: nombre del asset de datos.

## RTB_REGISTER_COMPONENT

```cpp
RTB_REGISTER_COMPONENT(ClassName)
```

Abre el registro en el `.cpp`. Hasta `RTB_END_REGISTER`, las macros `RTB_PROPERTY_*` de en medio son campos de este tipo. Sin este bloque el header no basta: `AddComponentOfType` escribe que falta el registro y devuelve null.

- `ClassName`: la misma clase que `RTB_COMPONENT`.

## RTB_PROPERTY

```cpp
RTB_PROPERTY(field)
```

Publica un campo en el Inspector y en el `.lua`. El tipo del campo decide el widget (float, int, bool, string, vector).

- `field`: miembro de `ThisClass`.

## RTB_PROPERTY_RANGE

```cpp
RTB_PROPERTY_RANGE(field, min, max)
```

Igual que `RTB_PROPERTY`, con un slider entre `min` y `max`. El loader puede seguir escribiendo un valor fuera de rango si el `.lua` lo trae; el rango es del Inspector.

- `field`: miembro numérico.
- `min`, `max`: extremos del slider.

## RTB_PROPERTY_ENUM

```cpp
RTB_PROPERTY_ENUM(field, A, B, C)
```

Combo con esos nombres. El campo es un enum o un int que el Inspector trata como índice de esa lista.

- `field`: miembro.
- `A`, `B`, `C`: etiquetas, las que quieras, no solo tres.

## RTB_PROPERTY_COLOR

```cpp
RTB_PROPERTY_COLOR(field)
```

Selector de color. El campo es un `Vector4` o un `Color` RGBA.

- `field`: miembro de color.

## RTB_PROPERTY_TEXTURE

```cpp
RTB_PROPERTY_TEXTURE(field)
```

Ranura de textura. El `.lua` guarda la ruta; el componente la resuelve a puntero en `OnValidate` / `OnAwake`.

- `field`: ruta o referencia de textura.

## RTB_PROPERTY_MESH

```cpp
RTB_PROPERTY_MESH(field)
```

Ranura de malla. Igual que la textura: en disco es una ruta.

- `field`: ruta o referencia de malla.

## RTB_PROPERTY_AUDIOCLIP

```cpp
RTB_PROPERTY_AUDIOCLIP(field)
```

Ranura de clip de audio. La ruta se resuelve al cargar la escena.

- `field`: ruta o referencia del clip.

## RTB_PROPERTY_FONT

```cpp
RTB_PROPERTY_FONT(field)
```

Ranura de fuente para la UI.

- `field`: ruta o referencia de la fuente.

## RTB_PROPERTY_FBX

```cpp
RTB_PROPERTY_FBX(field)
```

Ranura de FBX. `Animator::LoadClipFromFbx` y `modelRef` usan este tipo de ruta (`Assets/...fbx`).

- `field`: ruta del FBX.

## RTB_PROPERTY_ASSET_PATH

```cpp
RTB_PROPERTY_ASSET_PATH(field, "TypeName")
```

Ruta de un asset cuyo tipo de editor es `TypeName`. El Inspector filtra el selector con ese nombre.

- `field`: string de ruta.
- `"TypeName"`: tipo que el selector acepta.

## RTB_PROPERTY_DATA_ASSET

```cpp
RTB_PROPERTY_DATA_ASSET(field)
```

Referencia a un data asset registrado con `RTB_DATA_ASSET`.

- `field`: referencia o ruta del asset.

## RTB_PROPERTY_GAMEOBJECT

```cpp
RTB_PROPERTY_GAMEOBJECT(field)
```

Referencia a otro `GameObject`, guardada por UUID. En `OnAwake` el puntero puede seguir null; en `OnStart` el loader ya lo resolvió. Un UUID que no está en la escena queda null.

- `field`: `GameObject*`.

## RTB_PROPERTY_COMPONENT

```cpp
RTB_PROPERTY_COMPONENT(field, ComponentType)
```

Referencia a un componente de ese tipo, también por UUID del dueño. Misma regla: léela en `OnStart`.

- `field`: puntero al componente.
- `ComponentType`: clase del componente.

## RTB_PROPERTY_STRING_LIST

```cpp
RTB_PROPERTY_STRING_LIST(field)
```

Lista de strings editable en el Inspector. Vacía por defecto si el campo se construye vacío.

- `field`: `std::vector<std::string>` u otra lista de texto que la macro acepte.

## RTB_PROPERTY_GAMEOBJECT_LIST

```cpp
RTB_PROPERTY_GAMEOBJECT_LIST(field)
```

Lista de `GameObject*`, cada uno por UUID. Los que no se resuelven quedan null en su hueco.

- `field`: lista de objetos.

## RTB_PROPERTY_COMPONENT_LIST

```cpp
RTB_PROPERTY_COMPONENT_LIST(field, ComponentType)
```

Lista de componentes de ese tipo.

- `field`: lista.
- `ComponentType`: clase de cada elemento.

## RTB_PROPERTY_HIDDEN

```cpp
RTB_PROPERTY_HIDDEN(field)
```

Se serializa en el `.lua` y no se enseña en el Inspector.

- `field`: miembro oculto.

## RTB_PROPERTY_READONLY

```cpp
RTB_PROPERTY_READONLY(field)
```

Se enseña en el Inspector y no se edita. El loader sí puede escribirlo al cargar.

- `field`: miembro de solo lectura en el Inspector.

## RTB_END_REGISTER

```cpp
RTB_END_REGISTER(ClassName)
```

Cierra el bloque abierto por `RTB_REGISTER_COMPONENT` y publica el `TypeInfo`. El nombre tiene que ser el mismo. Sin este cierre el `.cpp` no compila.

- `ClassName`: la clase del registro.

## RTB_REGISTER_DATA_ASSET

```cpp
RTB_REGISTER_DATA_ASSET(ClassName)
```

Abre el registro de un data asset, el equivalente de `RTB_REGISTER_COMPONENT` para datos que no van pegados a un `GameObject`. Se cierra con `RTB_END_REGISTER_DATA_ASSET`.

- `ClassName`: clase del asset.

## RTB_END_REGISTER_DATA_ASSET

```cpp
RTB_END_REGISTER_DATA_ASSET(ClassName)
```

Cierra el registro del data asset.

- `ClassName`: la misma clase.

## RTB_PROPERTY_SERIALIZED

```cpp
RTB_PROPERTY_SERIALIZED(field)
```

Fuerza la serialización cuando el atajo `RTB_PROPERTY` no basta (un tipo que el registro no reconoce solo).

- `field`: miembro a guardar.

## RTB_PROPERTY_SERIALIZED_RANGE

```cpp
RTB_PROPERTY_SERIALIZED_RANGE(field, min, max)
```

Igual, con rango de Inspector.

- `field`, `min`, `max`: miembro y extremos.

## RTB_PROPERTY_SERIALIZED_HIDDEN

```cpp
RTB_PROPERTY_SERIALIZED_HIDDEN(field)
```

Se guarda en el `.lua` y no se muestra, para el caso en que hace falta la variante serializada y no `RTB_PROPERTY_HIDDEN`.

- `field`: miembro oculto y serializado.
