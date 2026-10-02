# UI

Namespace `RTBEngine::UI`. Elements are `Component`s and also implement pointer handlers. `CanvasSystem` routes the pointer and draws canvases at the view size. In the editor that happens only when the cursor is inside the Game View and is not captured. An object with `UIElement` shows `RectTransform` in the Inspector, not the 3D transform.

| Type | Role |
| --- | --- |
| `Canvas` | Root. Screen Space Overlay by default |
| `UIElement` | Base: raycast and visibility |
| `UIText` | Text |
| `UIImage` | Image |
| `UIPanel` | Panel |
| `UIContainer` | Groups children |
| `UISlider` | Continuous value |
| `UIInputField` | User text. Reads `InputManager::GetTextInput` |
| `UIJoystick` | Virtual axis |
| `UILayoutGroup` | Places children |

## SetNormalColor

```cpp
void UIButton::SetNormalColor(const Math::Vector4& color);
```

Rest color, RGBA. The field default is opaque white `(1, 1, 1, 1)`. Storing the color does not repaint immediately: the tint is applied when the state changes or when `SetInteractable` refreshes visuals, and only if `enableDefaultHoverVisuals` is true.

- `color`: RGBA. `w` is alpha.

## SetHoveredColor

```cpp
void UIButton::SetHoveredColor(const Math::Vector4& color);
```

Color while the pointer is over the button and it is interactable. The default is `(0.9, 0.9, 0.9, 1)`. Storing the color does not repaint immediately: it is applied the next time the button enters hover, and only if `enableDefaultHoverVisuals` is true.

- `color`: hover RGBA.

## SetPressedColor

```cpp
void UIButton::SetPressedColor(const Math::Vector4& color);
```

Color while the button is pressed. The default is `(0.7, 0.7, 0.7, 1)`. Storing the color does not repaint immediately: it is applied the next time the state becomes pressed, and only if `enableDefaultHoverVisuals` is true.

- `color`: pressed RGBA.

## SetDisabledColor

```cpp
void UIButton::SetDisabledColor(const Math::Vector4& color);
```

Color when `SetInteractable(false)`. The default is `(0.5, 0.5, 0.5, 0.5)`, half transparent. Storing the color does not repaint immediately: it is applied the next time the button becomes disabled, and only if `enableDefaultHoverVisuals` is true.

- `color`: disabled RGBA.

## SetOnClick

```cpp
void UIButton::SetOnClick(std::function<void()> callback);
```

Replaces the click callback. The button calls it from `OnPointerClick` when it is interactable. An empty callback clears the previous one: the click does nothing. Do not capture a `GameObject` name as `std::string` across the DLL; capture `this` on the component.

- `callback`: a function with no arguments, or empty to clear it.

```cpp
button->SetOnClick([this]() {
    RTBEngine::Scene::SceneManager::GetInstance()
        .RequestSceneLoad("Assets/Scenes/LobbyScene.lua");
});
```

## SetInteractable

```cpp
void UIButton::SetInteractable(bool interactable);
```

`false` moves the state to `Disabled`, uses the disabled color, and does not call the click. `true` returns it to `Normal` when the pointer is not over it. The field default is true.

- `interactable`: false to ignore the pointer.

## IsInteractable

```cpp
bool UIButton::IsInteractable() const;
```

**Returns** the flag. The default is true. It is not the same as `GetState`: an interactable button can be `Hovered` or `Pressed`.

## GetState

```cpp
ButtonState UIButton::GetState() const;
```

`ButtonState`: `Normal`, `Hovered`, `Pressed`, `Disabled`. It starts at `Normal`. `Disabled` when it is not interactable. The handlers the button already implements (`OnPointerEnter`, `OnPointerExit`, `OnPointerDown`, `OnPointerUp`, `OnPointerClick`) move the other states; a script does not call them.

**Returns** the visual state this frame.
