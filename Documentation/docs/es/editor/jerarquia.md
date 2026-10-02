# Jerarquía

La jerarquía lista los `GameObject` raíz de la escena que estás editando. Los inactivos se ven atenuados. El seleccionado queda resaltado.

El clic que selecciona es el release del botón izquierdo, y solo si no hay un drag en curso. Así puedes empezar a arrastrar sin cambiar la selección a medias. Un clic en el fondo del panel quita la selección.

## Reparentar

Arrastra un nodo sobre otro. El editor comprueba que el destino no sea descendiente del arrastrado y llama `SetParent`. La escena (o el prefab) queda dirty.

## Crear

Clic derecho en el vacío:

| Grupo | Entradas |
| --- | --- |
| 3D | Sphere, Cube, Plane. Esfera y cubo llevan `MeshRenderer` y `RigidBodyComponent`. El plano no lleva cuerpo |
| Efectos | Particle System, con `playOnAwake` y `simulateInEditMode` |
| UI | Canvas, UIButton, UIText |

El objeto nuevo queda seleccionado.

## Borrar

`Supr` borra el seleccionado y sus descendientes, de las hojas hacia la raíz, y marca dirty.

## Ajustes de escena

El desplegable superior cambia el skybox (on/off) y acepta un `.cubemap` arrastrado.

## Prefab

En modo prefab la lista es la del staging, no la del nivel. La barra de prefab, encima del dock, muestra la ruta, el asterisco de dirty, **Save Prefab** y **Back to Scene**.
