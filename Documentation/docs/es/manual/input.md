# Input

`RTBEngine::Input::InputManager` es un singleton alimentado por SDL cada frame, antes del update de la escena. En el editor, el ratón de la UI de juego solo se reenvía si el cursor está dentro de la Game View.

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

| Consulta | True cuando |
| --- | --- |
| `IsKeyPressed` | La tecla está abajo |
| `IsKeyJustPressed` | Este frame ha pasado de arriba a abajo |
| `IsKeyJustReleased` | Este frame se ha soltado |

Lo mismo con `IsMouseButtonPressed`, `JustPressed` y `JustReleased`.

## Teclas

`KeyCode` incluye `A`–`Z`, `Num0`–`Num9`, `F1`–`F12`, `Escape`, `Tab`, `Space`, `Enter`, `LeftShift`, `LeftControl`, `LeftAlt`, flechas y el teclado numérico. No uses el código SDL: el script no debe incluir SDL.

## Ratón capturado

```cpp
input.SetMouseRelativeMode(true);
bool captured = input.IsMouseRelativeModeEnabled();
input.SetMousePosition(x, y);
```

El modo relativo es el de una cámara que gira con el delta y esconde el cursor. En el editor, si el juego captura u oculta el cursor, la Game View deja de reenviar clics a la UI hasta que se suelta. `Escape` en la Game View solo libera el cursor si el juego lo había capturado.

## Texto

`GetTextInput()` devuelve el texto introducido ese frame. Sirve para un `UIInputField`, no para movimiento.

Lee el input en `OnUpdate`, no en `OnAwake`. El frame todavía no ha procesado eventos cuando el componente nace.
