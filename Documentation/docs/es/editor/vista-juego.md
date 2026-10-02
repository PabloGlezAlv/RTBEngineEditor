# Vista de juego

La Game View muestra la `CameraComponent` principal, sin grid, sin colliders y sin nav debug.

| Estado | Imagen |
| --- | --- |
| Edit | Último frame o el aviso de que no estás en Play |
| Play / Pause | Frame de la cámara de juego, más el canvas |

El aspect de la cámara de juego sigue el tamaño del panel.

## UI

En Play, con el cursor visible y sin captura, la vista reenvía:

- movimiento → `CanvasSystem::OnMouseMove`
- clic izquierdo → `OnMouseDown` / `OnMouseUp`

Si el juego oculta o captura el cursor, esos eventos se cortan. `Escape` aquí solo lo suelta cuando el juego lo había capturado. Si no, Escape no hace nada a nivel de editor.

Con un `UIElement` seleccionado, la vista dibuja en rojo las zonas `raycastTarget`.

## Qué no hace

No mueve la cámara del editor. No aplica gizmos. No hornea navegación. Para encuadrar luces y colliders usa la Scene View; para ver el juego, esta.
