# Content Browser

The browser is rooted at `Assets/`. It shows an icon grid. Folders come first, then files, both in case-insensitive alphabetical order.

## Navigation

- Click a folder to enter it.
- The back arrow goes up one level.
- Breadcrumbs are clickable.
- A single click selects the file and the Inspector reacts.
- `F2` renames. Enter confirms.
- `Del` deletes, after confirmation. A folder is removed entirely.

## Double click

| Type | Action |
| --- | --- |
| Folder | Enter |
| `.lua` | Requests a scene load. Edit mode only, and only if it is not already open |
| `.prefab` | Opens prefab mode. If the current prefab is dirty, it asks first |
| Anything else | Does not launch an external app |

Loading another scene **discards unsaved changes**. Save first.

## Drag

| Icon | Payload |
| --- | --- |
| Image | Texture |
| Model | Mesh |
| Cubemap | Cubemap |
| FBX onto an FBX slot | FBX path |

Drop it on a compatible Inspector field.

## Right click

**New**: folder, C++ component, empty C++ class, `.lua` scene, cubemap.

**File**: rename, delete, show in Explorer.

The C++ component asks for a name and writes the `.h` / `.cpp` pair with `RTB_COMPONENT` and an empty registration. The full template is in [C++ components](../manual/scripting/componentes-cpp.md).
