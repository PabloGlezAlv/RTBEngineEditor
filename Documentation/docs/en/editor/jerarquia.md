# Hierarchy

The hierarchy lists the root `GameObject`s of the scene you are editing. Inactive ones are dimmed. The selected one is highlighted.

The click that selects is the left-button release, and only if no drag is in progress. You can start dragging without changing the selection halfway through. A click on the panel background clears the selection.

## Reparent

Drag a node onto another. The editor checks that the destination is not a descendant of the dragged node and calls `SetParent`. The scene (or the prefab) is marked dirty.

## Create

Right-click empty space:

| Group | Entries |
| --- | --- |
| 3D | Sphere, Cube, Plane. Sphere and cube get `MeshRenderer` and `RigidBodyComponent`. The plane has no body |
| Effects | Particle System, with `playOnAwake` and `simulateInEditMode` |
| UI | Canvas, UIButton, UIText |

The new object is selected.

## Delete

`Del` deletes the selected object and its descendants, from the leaves toward the root, and marks dirty.

## Scene settings

The dropdown at the top toggles the skybox (on/off) and accepts a dropped `.cubemap`.

## Prefab

In prefab mode the list is the staging list, not the level's. The prefab bar, above the dock, shows the path, the dirty asterisk, **Save Prefab**, and **Back to Scene**.
