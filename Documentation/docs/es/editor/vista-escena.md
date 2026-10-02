# Vista de escena

La Scene View pinta la escena de edición con una cámara que no se guarda. El framebuffer nace a 1280×720 y se redimensiona con el panel.

## Cámara

| Entrada | Efecto |
| --- | --- |
| Botón derecho + ratón | Mirar |
| Botón derecho + WASD | Mover en el plano de la mirada |
| Q / E con el botón derecho | Bajar / subir |
| Shift | Velocidad ×3 |
| Rueda | Acercar |

Atajos de gizmo, solo con la vista enfocada y sin un campo de texto activo:

| Tecla | Gizmo |
| --- | --- |
| `W` | Trasladar |
| `E` | Rotar |
| `R` | Escalar |

La barra de la vista repite esos modos y el espacio **Local / World**.

## Seleccionar

Clic izquierdo, sin estar encima del gizmo y sin haber arrastrado, lanza un rayo y elige el `MeshRenderer` cuyo AABB corte más cerca. Un objeto sin malla no se pickea aquí; selecciónalo en la jerarquía.

## Gizmo

ImGuizmo 1.83 aplica la matriz mundo del objeto. Al soltar, el editor descompone traslación, rotación y escala en el `Transform` local y marca dirty.

## View cube

Cubo de unos 80×80 en la esquina superior derecha. Cada cara alinea la cámara (arriba, abajo, frente, atrás, izquierda, derecha) conservando la distancia al origen.

## Overlays

Solo en esta vista:

- Grid en el plano XZ y ejes RGB (X rojo, Y verde, Z azul). El grid se recentra con la cámara para parecer infinito.
- Wireframe verde del collider del objeto seleccionado.
- Nav debug si **Window → Navigation Debug** está activo.

La rejilla no escribe profundidad, para que la geometría quede por encima. Los ejes sí.
