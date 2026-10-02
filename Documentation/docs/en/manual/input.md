# Input

`RTBEngine::Input::InputManager` is a singleton fed by SDL every frame, before the scene update. In the editor, the game UI mouse is forwarded only if the cursor is inside the Game View.

```cpp
auto& input = RTBEngine::Input::InputManager::GetInstance();

if (input.IsKeyJustPressed(RTBEngine::Input::KeyCode::Space)) {
    Jump();
}
if (input.IsKeyPressed(RTBEngine::Input::KeyCode::W)) {
    MoveForward();
}

if (input.IsMouseButtonJustPressed(RTBEngine::Input::MouseButton::Left)) {
    Fire();
}

int x = input.GetMouseX();
int y = input.GetMouseY();
int dx = input.GetMouseDeltaX();
int dy = input.GetMouseDeltaY();
int wheel = input.GetScrollDelta();
```

| Query | True when |
| --- | --- |
| `IsKeyPressed` | The key is down |
| `IsKeyJustPressed` | This frame it went from up to down |
| `IsKeyJustReleased` | This frame it was released |

The same applies to `IsMouseButtonPressed`, `JustPressed`, and `JustReleased`.

## Keys

`KeyCode` includes `A`–`Z`, `Num0`–`Num9`, `F1`–`F12`, `Escape`, `Tab`, `Space`, `Enter`, `LeftShift`, `LeftControl`, `LeftAlt`, arrows, and the numeric keypad. Do not use the SDL code: the script must not include SDL.

## Captured mouse

```cpp
input.SetMouseRelativeMode(true);
bool captured = input.IsMouseRelativeModeEnabled();
input.SetMousePosition(x, y);
```

Relative mode is for a camera that turns with the delta and hides the cursor. In the editor, if the game captures or hides the cursor, the Game View stops forwarding clicks to the UI until it is released. `Escape` in the Game View only frees the cursor if the game had captured it.

## Text

`GetTextInput()` returns the text entered that frame. It is for a `UIInputField`, not for movement.

Read input in `OnUpdate`, not in `OnAwake`. The frame has not processed events yet when the component is born.
