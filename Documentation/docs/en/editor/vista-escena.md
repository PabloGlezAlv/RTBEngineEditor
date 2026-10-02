# Scene View

The Scene View draws the editing scene with a camera that is not saved. The framebuffer starts at 1280×720 and resizes with the panel.

## Camera

| Input | Effect |
| --- | --- |
| Right button + mouse | Look |
| Right button + WASD | Move along the look plane |
| Q / E with the right button | Down / up |
| Shift | Speed ×3 |
| Wheel | Zoom in |

Gizmo shortcuts work only while the view is focused and no text field is active:

| Key | Gizmo |
| --- | --- |
| `W` | Translate |
| `E` | Rotate |
| `R` | Scale |

The view toolbar repeats those modes and the **Local / World** space.

## Picking

A left click, when the cursor is not on the gizmo and the mouse did not drag, casts a ray and picks the `MeshRenderer` whose AABB is hit closest. An object without a mesh is not picked here; select it in the Hierarchy.

## Gizmo

ImGuizmo 1.83 applies the object's world matrix. On release, the editor decomposes translation, rotation, and scale into the local `Transform` and marks the scene dirty.

## View cube

A cube of about 80×80 in the top-right corner. Each face aligns the camera (up, down, front, back, left, right) and keeps the distance to the origin.

## Overlays

Only in this view:

- Grid on the XZ plane and RGB axes (X red, Y green, Z blue). The grid recenters on the camera so it looks infinite.
- Green wireframe of the selected object's collider.
- Nav debug when **Window → Navigation Debug** is on.

The grid does not write depth, so geometry draws on top of it. The axes do write depth.
