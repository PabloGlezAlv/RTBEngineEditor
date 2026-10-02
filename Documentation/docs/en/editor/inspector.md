# Inspector

The Inspector edits the selected `GameObject` or, when no object is selected and an asset is, that asset.

## Object

1. Name. It is confirmed when you leave the field.
2. Transform: position, rotation in **degrees**, scale. On UI objects, the rect.
3. One block per component, with remove in the header context menu.
4. **+ Add Component**, with a name filter.

Rotation in degrees is cached by UUID while the object stays selected. Reading the quaternion again every frame would introduce drift. The value that lands in C++ is radians; the conversion belongs to the Inspector. See [Math](../manual/matematicas.md).

## Fields

| Type | Control |
| --- | --- |
| Bool | Checkbox |
| Int / float | Drag. Slider if the property has a range |
| String | Text |
| Vector2/3/4 | Drags |
| Color | Picker |
| Enum | Combo |
| Texture, mesh, audio, font, FBX | Text, `...` button, drop of the matching type |
| GameObject | Drop from the hierarchy. Shows the name or `(None)` |
| Component | Same, and resolves the component on that object |

Any change calls `OnValidate` and marks the scene or the prefab dirty.

## Custom drawers

| Component | Besides the fields |
| --- | --- |
| `Animator` | `modelRef`, extra models. When they change, `ReloadClipLibrary` |
| `ParticleSystem` | Play, Pause, Stop, Burst, and counters |
| `NavGridComponent` | Bake Grid, Clear Baked, walkable cells |
| `VolumeComponent` | Zone and fogs, in place of the generic list |

## Prefab instance

If the object comes from a prefab you see the **Prefab** header (name, Select Root, Revert All, Unlink) and the blue override labels. Right-click a label: Revert or Apply for that field. A component that is not in the asset shows up as **Added Component**.

## Assets

| Selection | Inspector |
| --- | --- |
| `.cubemap` | Six faces: Right, Left, Top, Bottom, Front, Back, and **Save** |
| `.h` / `.cpp` | The first lines and **Open in Editor** (opens `GameScripts.vcxproj`) |
| `.rtbasset` | Data asset properties |

**Open Prefab** on a selected `.prefab` enters edit mode.
