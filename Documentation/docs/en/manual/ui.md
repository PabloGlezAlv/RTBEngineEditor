# User interface

Game UI is not ImGui. ImGui is only the editor. The game uses `Canvas` and elements in the hierarchy: `UIButton`, `UIText`, `UIImage`, `UIPanel`, `UIContainer`, plus slider, input field, joystick, and layout group.

UI objects carry `UIElement` and a rect, not the 3D transform. The Inspector switches to `RectTransform` fields when the object is UI.

## Creating

In the hierarchy, right-click:

- **Create Canvas**. Default mode: Screen Space Overlay.
- **Create UIButton**. If there is no canvas, it creates one and parents the button.
- **Create UIText**.

The canvas is drawn on top of the camera image in the Game View and in the player. `CanvasSystem::RenderAll` receives that view's size.

## Button

```cpp
auto* button = GetOwner()->GetComponent<RTBEngine::UI::UIButton>();
button->SetInteractable(true);
button->SetOnClick([this]() {
    RTBEngine::Scene::SceneManager::GetInstance()
        .RequestSceneLoad("Assets/Scenes/DefaultScene.lua");
});
```

Reflected colors: `normalColor`, `hoveredColor`, `pressedColor`, `disabledColor`. `interactable` false does not fire the click. `enableDefaultHoverVisuals` tints the button's image or panel.

States: `Normal`, `Hovered`, `Pressed`, `Disabled`.

The click goes through the event system (`OnPointerEnter`, `OnPointerDown`, `OnPointerClick`, …). For it to arrive, the element has to be `raycastTarget` and the cursor has to be over the game view, visible, and not captured.

## In the editor

During Play, the Game View forwards movement and left click to `CanvasSystem` only while the cursor is free. If your script puts the mouse in relative mode, buttons stop receiving clicks until you release it.

If you select an object with `UIElement`, the Game View draws the hit rectangles of `raycastTarget` elements in red. Use that to see why a button is not receiving the click.

## Menus in the sample project

| Component | What it does |
| --- | --- |
| `SceneChangeButton` | Loads `scenePath` on press, if the button is interactable |
| `ApplicationQuitButton` | In the player it quits. In editor Play it Stops; it does not kill the editor |
| `ButtonStyle` | Visual only. It does not load scenes |

`SceneChangeButton` stores the path in a reflected property. You do not have to write it in code when you place it.

The menu scene order is in [Online](../online/index.md).
