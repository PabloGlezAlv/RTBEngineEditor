# Properties

The Inspector and the scene Lua read metadata. They do not see your private fields by magic. Declare the component with `RTB_COMPONENT` and list the fields between `RTB_REGISTER_COMPONENT` and `RTB_END_REGISTER`.

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

## What the Inspector draws

| Macro | Widget |
| --- | --- |
| `RTB_PROPERTY` | Depends on the field type: bool, int, float, string, Vector2/3/4 |
| `RTB_PROPERTY_RANGE` | Slider between min and max |
| `RTB_PROPERTY_ENUM` | Combo of the names you pass |
| `RTB_PROPERTY_COLOR` | Color picker |
| `RTB_PROPERTY_TEXTURE` / `MESH` / `AUDIOCLIP` / `FONT` / `FBX` | Asset path, a `...` button, and drag from the Content Browser |
| `RTB_PROPERTY_GAMEOBJECT` | Drop an object from the hierarchy |
| `RTB_PROPERTY_COMPONENT` | Resolves that component on the dropped object |
| `RTB_PROPERTY_HIDDEN` | Serialized, not shown |
| `RTB_PROPERTY_READONLY` | Shown, not editable |

`RTB_PROPERTY_ASSET_PATH(campo, "Tipo")` is the generic form when the shortcut does not cover your extension. `RTB_PROPERTY_DATA_ASSET` points at a `.rtbasset`.

There are `RTB_PROPERTY_SERIALIZED` and `RTB_PROPERTY_SERIALIZED_RANGE` variants when you want to force the serialization path. For an ordinary script field, `RTB_PROPERTY` is enough.

## Types that travel well

The field has to be a POD or a type the bridge knows: `bool`, integers, `float`, `double`, `std::string` **inside** the component (the engine reads the offset; you do not return the `std::string` across the boundary), `Vector2/3/4`, `Quaternion`, `Color`, pointers to `GameObject` or to a component.

Do not register a raw `std::vector` that has no list macro. Use `RTB_PROPERTY_STRING_LIST`, `RTB_PROPERTY_GAMEOBJECT_LIST`, or `RTB_PROPERTY_COMPONENT_LIST`.

## Data assets

A data resource (a character definition, for example) is not a scene component. Use `RTB_DATA_ASSET`, `RTB_REGISTER_DATA_ASSET`, and `RTB_END_REGISTER_DATA_ASSET`. The file is `.rtbasset`, and the Inspector opens it when you select it in the browser.

## Inheritance

If your type extends another reflected component:

```cpp
RTB_INHERITS(BaseComponent)
```

Up to three bases: `RTB_INHERITS(A, B)` or `RTB_INHERITS(A, B, C)`. It sits next to `RTB_COMPONENT` in the class.

## After a change

The Inspector calls `OnValidate()` and marks the scene or the prefab dirty. In `OnValidate` you can copy proxies into internal state, but do not assume UUID references are already resolved if you are still in the middle of loading: that happens before `OnStart`.
