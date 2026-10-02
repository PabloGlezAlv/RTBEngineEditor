# Reflection

Macros in `RTBEngine/Reflection/PropertyMacros.h`. You need `using ThisClass = YourClass;` before the registration. The block builds a `TypeInfo` and puts it in `TypeRegistry`. The Inspector lists `GetRegisteredTypes()` under **Add Component**. A type without this block does not appear and is not saved in the `.lua`.

## RTB_COMPONENT

```cpp
RTB_COMPONENT(ClassName)
```

Inside the class. It implements `GetTypeName`, `GetTypeInfo`, and the factory `AddComponent` uses to create and destroy the object in the right module. Without this macro the type cannot be added by name.

- `ClassName`: the class name, the same one the `.lua` will see.

## RTB_INHERITS

```cpp
RTB_INHERITS(Base)
RTB_INHERITS(A, B)
RTB_INHERITS(A, B, C)
```

Declares which components this one inherits, so `GetComponent<Base>()` finds the derived type. One, two, or three bases. It sits in the class, next to `RTB_COMPONENT`.

- `Base`, `A`, `B`, `C`: base classes that are also `Component`s.

## RTB_DATA_ASSET

```cpp
RTB_DATA_ASSET(ClassName)
```

Marks a data class (not a scene component) so the asset registry knows it. It does not add it to the **Add Component** menu.

- `ClassName`: data-asset name.

## RTB_REGISTER_COMPONENT

```cpp
RTB_REGISTER_COMPONENT(ClassName)
```

Opens the registration in the `.cpp`. Until `RTB_END_REGISTER`, the `RTB_PROPERTY_*` macros in between are fields of this type. Without this block the header is not enough: `AddComponentOfType` logs that the registration is missing and returns null.

- `ClassName`: the same class as `RTB_COMPONENT`.

## RTB_PROPERTY

```cpp
RTB_PROPERTY(field)
```

Publishes a field in the Inspector and in the `.lua`. The field's type picks the widget (float, int, bool, string, vector).

- `field`: a member of `ThisClass`.

## RTB_PROPERTY_RANGE

```cpp
RTB_PROPERTY_RANGE(field, min, max)
```

Same as `RTB_PROPERTY`, with a slider between `min` and `max`. The loader can still write a value outside the range if the `.lua` has one; the range is for the Inspector.

- `field`: numeric member.
- `min`, `max`: slider ends.

## RTB_PROPERTY_ENUM

```cpp
RTB_PROPERTY_ENUM(field, A, B, C)
```

A combo with those names. The field is an enum or an int the Inspector treats as an index into that list.

- `field`: member.
- `A`, `B`, `C`: labels. As many as you need, not only three.

## RTB_PROPERTY_COLOR

```cpp
RTB_PROPERTY_COLOR(field)
```

Color picker. The field is an RGBA `Vector4` or `Color`.

- `field`: color member.

## RTB_PROPERTY_TEXTURE

```cpp
RTB_PROPERTY_TEXTURE(field)
```

Texture slot. The `.lua` stores the path; the component resolves it to a pointer in `OnValidate` / `OnAwake`.

- `field`: texture path or reference.

## RTB_PROPERTY_MESH

```cpp
RTB_PROPERTY_MESH(field)
```

Mesh slot. Same as the texture: on disk it is a path.

- `field`: mesh path or reference.

## RTB_PROPERTY_AUDIOCLIP

```cpp
RTB_PROPERTY_AUDIOCLIP(field)
```

Audio-clip slot. The path is resolved when the scene loads.

- `field`: clip path or reference.

## RTB_PROPERTY_FONT

```cpp
RTB_PROPERTY_FONT(field)
```

Font slot for UI.

- `field`: font path or reference.

## RTB_PROPERTY_FBX

```cpp
RTB_PROPERTY_FBX(field)
```

FBX slot. `Animator::LoadClipFromFbx` and `modelRef` use this kind of path (`Assets/...fbx`).

- `field`: FBX path.

## RTB_PROPERTY_ASSET_PATH

```cpp
RTB_PROPERTY_ASSET_PATH(field, "TypeName")
```

Path to an asset whose editor type is `TypeName`. The Inspector filters the picker with that name.

- `field`: path string.
- `"TypeName"`: type the picker accepts.

## RTB_PROPERTY_DATA_ASSET

```cpp
RTB_PROPERTY_DATA_ASSET(field)
```

Reference to a data asset registered with `RTB_DATA_ASSET`.

- `field`: asset reference or path.

## RTB_PROPERTY_GAMEOBJECT

```cpp
RTB_PROPERTY_GAMEOBJECT(field)
```

Reference to another `GameObject`, stored by UUID. In `OnAwake` the pointer can still be null; in `OnStart` the loader has resolved it. A UUID that is not in the scene stays null.

- `field`: `GameObject*`.

## RTB_PROPERTY_COMPONENT

```cpp
RTB_PROPERTY_COMPONENT(field, ComponentType)
```

Reference to a component of that type, also by the owner's UUID. Same rule: read it in `OnStart`.

- `field`: component pointer.
- `ComponentType`: component class.

## RTB_PROPERTY_STRING_LIST

```cpp
RTB_PROPERTY_STRING_LIST(field)
```

Editable string list in the Inspector. Empty by default if the field is constructed empty.

- `field`: a `std::vector<std::string>` or another text list the macro accepts.

## RTB_PROPERTY_GAMEOBJECT_LIST

```cpp
RTB_PROPERTY_GAMEOBJECT_LIST(field)
```

List of `GameObject*`, each by UUID. Ones that do not resolve stay null in their slot.

- `field`: object list.

## RTB_PROPERTY_COMPONENT_LIST

```cpp
RTB_PROPERTY_COMPONENT_LIST(field, ComponentType)
```

List of components of that type.

- `field`: list.
- `ComponentType`: class of each element.

## RTB_PROPERTY_HIDDEN

```cpp
RTB_PROPERTY_HIDDEN(field)
```

Serialized in the `.lua` and not shown in the Inspector.

- `field`: hidden member.

## RTB_PROPERTY_READONLY

```cpp
RTB_PROPERTY_READONLY(field)
```

Shown in the Inspector and not edited there. The loader can still write it on load.

- `field`: member that is read-only in the Inspector.

## RTB_END_REGISTER

```cpp
RTB_END_REGISTER(ClassName)
```

Closes the block opened by `RTB_REGISTER_COMPONENT` and publishes the `TypeInfo`. The name has to match. Without this close the `.cpp` does not compile.

- `ClassName`: the registered class.

## RTB_REGISTER_DATA_ASSET

```cpp
RTB_REGISTER_DATA_ASSET(ClassName)
```

Opens a data-asset registration, the equivalent of `RTB_REGISTER_COMPONENT` for data that is not attached to a `GameObject`. Close it with `RTB_END_REGISTER_DATA_ASSET`.

- `ClassName`: asset class.

## RTB_END_REGISTER_DATA_ASSET

```cpp
RTB_END_REGISTER_DATA_ASSET(ClassName)
```

Closes the data-asset registration.

- `ClassName`: the same class.

## RTB_PROPERTY_SERIALIZED

```cpp
RTB_PROPERTY_SERIALIZED(field)
```

Forces serialization when the `RTB_PROPERTY` shortcut is not enough (a type the registry does not recognize on its own).

- `field`: member to store.

## RTB_PROPERTY_SERIALIZED_RANGE

```cpp
RTB_PROPERTY_SERIALIZED_RANGE(field, min, max)
```

Same, with an Inspector range.

- `field`, `min`, `max`: member and ends.

## RTB_PROPERTY_SERIALIZED_HIDDEN

```cpp
RTB_PROPERTY_SERIALIZED_HIDDEN(field)
```

Stored in the `.lua` and not shown, for the case that needs the serialized variant rather than `RTB_PROPERTY_HIDDEN`.

- `field`: hidden serialized member.
