# UI

Namespace `RTBEngine::UI`. Los elementos son `Component` y además implementan handlers de puntero. `CanvasSystem` reparte el puntero y dibuja los canvas al tamaño de la vista. En el editor eso solo ocurre si el cursor está dentro de la Game View y no está capturado. Un objeto con `UIElement` enseña `RectTransform` en el Inspector, no el transform 3D.

| Tipo | Rol |
| --- | --- |
| `Canvas` | Raíz. Screen Space Overlay por defecto |
| `UIElement` | Base: raycast y visibilidad |
| `UIText` | Texto |
| `UIImage` | Imagen |
| `UIPanel` | Panel |
| `UIContainer` | Agrupa hijos |
| `UISlider` | Valor continuo |
| `UIInputField` | Texto del usuario. Lee `InputManager::GetTextInput` |
| `UIJoystick` | Eje virtual |
| `UILayoutGroup` | Coloca hijos |

## SetNormalColor

```cpp
void UIButton::SetNormalColor(const Math::Vector4& color);
```

Color del botón en reposo, RGBA. El default del campo es blanco opaco `(1, 1, 1, 1)`. Guardar el color no repinta en el acto: el tinte se aplica cuando el estado cambia o cuando `SetInteractable` refresca los visuals, y solo si `enableDefaultHoverVisuals` es true.

- `color`: RGBA. `w` es alfa.

## SetHoveredColor

```cpp
void UIButton::SetHoveredColor(const Math::Vector4& color);
```

Color mientras el puntero está encima y el botón es interactuable. El default es `(0.9, 0.9, 0.9, 1)`. Guardar el color no repinta en el acto: se aplica la próxima vez que el botón entra en hover, y solo si `enableDefaultHoverVisuals` es true.

- `color`: RGBA del hover.

## SetPressedColor

```cpp
void UIButton::SetPressedColor(const Math::Vector4& color);
```

Color mientras el botón está pulsado. El default es `(0.7, 0.7, 0.7, 1)`. Guardar el color no repinta en el acto: se aplica la próxima vez que el estado pasa a pulsado, y solo si `enableDefaultHoverVisuals` es true.

- `color`: RGBA de la pulsación.

## SetDisabledColor

```cpp
void UIButton::SetDisabledColor(const Math::Vector4& color);
```

Color cuando `SetInteractable(false)`. El default es `(0.5, 0.5, 0.5, 0.5)`, a media transparencia. Guardar el color no repinta en el acto: se aplica la próxima vez que el botón queda deshabilitado, y solo si `enableDefaultHoverVisuals` es true.

- `color`: RGBA del estado deshabilitado.

## SetOnClick

```cpp
void UIButton::SetOnClick(std::function<void()> callback);
```

Sustituye el callback del click. El botón lo llama desde `OnPointerClick` si es interactuable. Un callback vacío borra el anterior: el click no hace nada. No captures el `GameObject` por `std::string` a través de la DLL; captura `this` del componente.

- `callback`: función sin argumentos, o vacía para quitarla.

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

`false` pasa el estado a `Disabled`, usa el color deshabilitado y no llama al click. `true` lo devuelve a `Normal` si el puntero no está encima. El default del campo es true.

- `interactable`: false para ignorar el puntero.

## IsInteractable

```cpp
bool UIButton::IsInteractable() const;
```

**Devuelve** el flag. El default es true. No es lo mismo que `GetState`: un botón interactuable puede estar en `Hovered` o `Pressed`.

## GetState

```cpp
ButtonState UIButton::GetState() const;
```

`ButtonState`: `Normal`, `Hovered`, `Pressed`, `Disabled`. Arranca en `Normal`. `Disabled` cuando no es interactuable. Los handlers que ya implementa el botón (`OnPointerEnter`, `OnPointerExit`, `OnPointerDown`, `OnPointerUp`, `OnPointerClick`) son los que mueven el resto de estados; un script no los llama.

**Devuelve** el estado visual de este frame.
