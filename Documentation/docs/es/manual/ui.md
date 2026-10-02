# Interfaz de usuario

La UI de juego no es ImGui. ImGui es solo el editor. El juego usa `Canvas` y elementos en la jerarquía: `UIButton`, `UIText`, `UIImage`, `UIPanel`, `UIContainer`, más slider, input field, joystick y layout group.

Los objetos de UI llevan `UIElement` y un rect, no el transform 3D. El Inspector cambia a campos de `RectTransform` cuando el objeto es de UI.

## Crear

En la jerarquía, clic derecho:

- **Create Canvas**. Modo por defecto: Screen Space Overlay.
- **Create UIButton**. Si no hay canvas, crea uno y cuelga el botón.
- **Create UIText**.

El canvas se dibuja encima de la imagen de la cámara en la Game View y en el player. `CanvasSystem::RenderAll` recibe el tamaño de esa vista.

## Botón

```cpp
auto* button = GetOwner()->GetComponent<RTBEngine::UI::UIButton>();
button->SetInteractable(true);
button->SetOnClick([this]() {
    RTBEngine::Scene::SceneManager::GetInstance()
        .RequestSceneLoad("Assets/Scenes/DefaultScene.lua");
});
```

Colores reflejados: `normalColor`, `hoveredColor`, `pressedColor`, `disabledColor`. `interactable` en false no dispara el click. `enableDefaultHoverVisuals` tintea la imagen o el panel del botón.

Estados: `Normal`, `Hovered`, `Pressed`, `Disabled`.

El click pasa por el event system (`OnPointerEnter`, `OnPointerDown`, `OnPointerClick`, …). Para que llegue, el elemento tiene que ser `raycastTarget` y el cursor tiene que estar sobre la vista de juego, visible, sin captura.

## En el editor

Durante Play, la Game View reenvía movimiento y clic izquierdo al `CanvasSystem` solo con el cursor libre. Si tu script pone el ratón en modo relativo, los botones dejan de recibir clics hasta soltarlo.

Si seleccionas un objeto con `UIElement`, la Game View dibuja en rojo los rectángulos de hit de los `raycastTarget`. Sirve para ver por qué un botón no recibe el clic.

## Menús del proyecto de ejemplo

| Componente | Hace |
| --- | --- |
| `SceneChangeButton` | Carga `scenePath` al pulsar, si el botón es interactuable |
| `ApplicationQuitButton` | En el player cierra. En Play del editor hace Stop, no mata el editor |
| `ButtonStyle` | Solo visual. No carga escenas |

`SceneChangeButton` guarda la ruta en una propiedad reflejada. No hace falta escribirla en código al colocarlo.

El orden de las escenas de menú está en [Online](../online/index.md).
