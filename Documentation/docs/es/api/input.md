# Input

`RTBEngine::Input::InputManager::GetInstance()`. No incluyas SDL en el script. `Update` y `ProcessEvent` los llama la aplicación cada frame; un componente solo lee el estado.

`KeyCode` incluye `A`–`Z`, `Num0`–`Num9`, `F1`–`F12`, `Escape`, `Tab`, `Space`, `Enter`, `LeftShift`, `LeftControl`, `LeftAlt`, `Up`, `Down`, `Left`, `Right` y el bloque numérico.

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

True mientras la tecla está abajo, en todos los frames que siga pulsada. Para un disparo de una sola vez usa `IsKeyJustPressed`.

- `key`: tecla del enum. Un valor que este frame no llegó de SDL se lee como suelta.

**Devuelve** true si está pulsada ahora.

## IsKeyJustPressed

```cpp
bool IsKeyJustPressed(KeyCode key) const;
```

True solo en el frame en que la tecla pasa de arriba a abajo. Al frame siguiente, si sigue pulsada, esto es false y `IsKeyPressed` es true.

- `key`: tecla.

**Devuelve** true en el frame del flanco de bajada.

## IsKeyJustReleased

```cpp
bool IsKeyJustReleased(KeyCode key) const;
```

True solo en el frame en que la tecla pasa de abajo a arriba.

- `key`: tecla.

**Devuelve** true en el frame del flanco de subida.

## IsMouseButtonPressed

```cpp
bool IsMouseButtonPressed(MouseButton button) const;
```

Igual que `IsKeyPressed`, para un botón del ratón. Sigue true mientras el botón esté abajo.

- `button`: botón del enum `MouseButton`.

**Devuelve** true si está pulsado ahora.

## IsMouseButtonJustPressed

```cpp
bool IsMouseButtonJustPressed(MouseButton button) const;
```

True solo en el frame en que el botón pasa a pulsado.

- `button`: botón.

**Devuelve** true en ese frame.

## IsMouseButtonJustReleased

```cpp
bool IsMouseButtonJustReleased(MouseButton button) const;
```

True solo en el frame en que el botón se suelta.

- `button`: botón.

**Devuelve** true en ese frame.

## GetMouseX

```cpp
int GetMouseX() const;
```

**Devuelve** la X del cursor en píxeles de la ventana. Con el modo relativo activo, la posición absoluta deja de ser un puntero visible en el escritorio.

## GetMouseY

```cpp
int GetMouseY() const;
```

**Devuelve** la Y del cursor en píxeles de la ventana.

## GetMouseDeltaX

```cpp
int GetMouseDeltaX() const;
```

**Devuelve** cuántos píxeles se movió el cursor en X desde el frame anterior. En un frame quieto es 0. Es el valor que usa una cámara en modo relativo.

## GetMouseDeltaY

```cpp
int GetMouseDeltaY() const;
```

**Devuelve** el desplazamiento en Y desde el frame anterior. 0 si no se movió.

## GetScrollDelta

```cpp
int GetScrollDelta() const;
```

**Devuelve** el paso de la rueda en este frame. 0 si no hubo scroll. El signo sigue el evento de la ventana de este frame.

## GetTextInput

```cpp
const std::string& GetTextInput() const;
```

Texto que el sistema inyectó este frame (lo lee `UIInputField`). Vacío si el usuario no escribió caracteres. No acumula el campo entero: es la tanda de este frame.

**Devuelve** los caracteres de este frame, o una cadena vacía.

## SetMouseRelativeMode

```cpp
void SetMouseRelativeMode(bool enabled);
```

`true` esconde el cursor y pasa a reportar delta, el modo de una cámara de primera persona. `false` devuelve el cursor absoluto. El editor solo entrega el ratón a la Game View si el cursor está dentro de esa vista y no está capturado por otro panel.

- `enabled`: true para modo relativo.

## IsMouseRelativeModeEnabled

```cpp
bool IsMouseRelativeModeEnabled() const;
```

**Devuelve** true si el modo relativo está activo. El default es apagado hasta que alguien lo enciende.

## SetMousePosition

```cpp
void SetMousePosition(int x, int y);
```

Coloca el cursor en píxeles de la ventana. En modo relativo el escritorio no enseña el cursor; el delta del frame siguiente sale de este origen.

- `x`: píxeles desde la izquierda.
- `y`: píxeles desde arriba.
