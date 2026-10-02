# Input

`RTBEngine::Input::InputManager::GetInstance()`. Do not include SDL from a script. The application calls `Update` and `ProcessEvent` every frame; a component only reads state.

`KeyCode` includes `A`–`Z`, `Num0`–`Num9`, `F1`–`F12`, `Escape`, `Tab`, `Space`, `Enter`, `LeftShift`, `LeftControl`, `LeftAlt`, `Up`, `Down`, `Left`, `Right`, and the numpad.

```cpp
auto& in = RTBEngine::Input::InputManager::GetInstance();
if (in.IsKeyJustPressed(RTBEngine::Input::KeyCode::Escape)) {
    Close();
}
```

## IsKeyPressed

```cpp
bool IsKeyPressed(KeyCode key) const;
```

True for every frame the key stays down. For a one-shot action use `IsKeyJustPressed`.

- `key`: a value of the enum. A key that did not arrive from SDL this frame reads as up.

**Returns** true if it is down now.

## IsKeyJustPressed

```cpp
bool IsKeyJustPressed(KeyCode key) const;
```

True only on the frame the key goes from up to down. On the next frame, if it is still held, this is false and `IsKeyPressed` is true.

- `key`: key.

**Returns** true on the press edge frame.

## IsKeyJustReleased

```cpp
bool IsKeyJustReleased(KeyCode key) const;
```

True only on the frame the key goes from down to up.

- `key`: key.

**Returns** true on the release edge frame.

## IsMouseButtonPressed

```cpp
bool IsMouseButtonPressed(MouseButton button) const;
```

Same as `IsKeyPressed`, for a mouse button. It stays true while the button is down.

- `button`: a `MouseButton` value.

**Returns** true if it is down now.

## IsMouseButtonJustPressed

```cpp
bool IsMouseButtonJustPressed(MouseButton button) const;
```

True only on the frame the button becomes pressed.

- `button`: button.

**Returns** true on that frame.

## IsMouseButtonJustReleased

```cpp
bool IsMouseButtonJustReleased(MouseButton button) const;
```

True only on the frame the button is released.

- `button`: button.

**Returns** true on that frame.

## GetMouseX

```cpp
int GetMouseX() const;
```

**Returns** the cursor X in window pixels. With relative mode on, the absolute position is no longer a visible desktop pointer.

## GetMouseY

```cpp
int GetMouseY() const;
```

**Returns** the cursor Y in window pixels.

## GetMouseDeltaX

```cpp
int GetMouseDeltaX() const;
```

**Returns** how many pixels the cursor moved in X since the previous frame. A still frame is 0. This is the value a relative-mode camera uses.

## GetMouseDeltaY

```cpp
int GetMouseDeltaY() const;
```

**Returns** the Y movement since the previous frame. 0 if it did not move.

## GetScrollDelta

```cpp
int GetScrollDelta() const;
```

**Returns** the wheel step this frame. 0 if there was no scroll. The sign follows this frame's window event.

## GetTextInput

```cpp
const std::string& GetTextInput() const;
```

Characters the system injected this frame (`UIInputField` reads it). Empty if the user typed nothing. It does not accumulate the whole field: it is this frame's batch.

**Returns** this frame's characters, or an empty string.

## SetMouseRelativeMode

```cpp
void SetMouseRelativeMode(bool enabled);
```

`true` hides the cursor and reports deltas, the first-person camera mode. `false` restores the absolute cursor. The editor only gives the mouse to the Game View when the cursor is inside that view and no other panel has captured it.

- `enabled`: true for relative mode.

## IsMouseRelativeModeEnabled

```cpp
bool IsMouseRelativeModeEnabled() const;
```

**Returns** true if relative mode is on. The default stays off until something turns it on.

## SetMousePosition

```cpp
void SetMousePosition(int x, int y);
```

Places the cursor in window pixels. In relative mode the desktop does not show the cursor; the next frame's delta is measured from this origin.

- `x`: pixels from the left.
- `y`: pixels from the top.
