# Game View

The Game View shows the main `CameraComponent`, with no grid, no colliders, and no nav debug.

| State | Image |
| --- | --- |
| Edit | The last frame, or a notice that you are not in Play |
| Play / Pause | The game camera frame, plus the canvas |

The game camera aspect follows the panel size.

## UI

In Play, with the cursor visible and not captured, the view forwards:

- movement → `CanvasSystem::OnMouseMove`
- left click → `OnMouseDown` / `OnMouseUp`

If the game hides or captures the cursor, those events stop. `Escape` here releases the cursor only when the game had captured it. Otherwise Escape does nothing at the editor level.

With a `UIElement` selected, the view draws `raycastTarget` regions in red.

## What it does not do

It does not move the editor camera. It does not apply gizmos. It does not bake navigation. Frame lights and colliders in the Scene View; watch the game here.
