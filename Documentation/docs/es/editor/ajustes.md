# Ajustes del proyecto

**Window → Project Settings**.

| Sección | Archivo | Contenido |
| --- | --- | --- |
| Rendering | `.rtbproj` (`GraphicsAPI=`) | OpenGL o Vulkan, y la API de esta sesión |
| Lighting | `lighting.ini` | Ambiente, sombras, DDGI, niebla de distancia, niebla volumétrica |
| Save | Los dos | **Save Project Settings** no reinicia el editor |

## Cambiar de API gráfica

La ventana SDL y el dispositivo ya existen. El editor no cambia de backend en caliente. **Apply & Restart Editor**:

1. Escribe `GraphicsAPI` en el `.rtbproj`.
2. Hace Stop si estabas en Play.
3. Pregunta si hay cambios sin guardar.
4. Lanza otro proceso y cierra este.

El texto del panel lo dice cuando la selección no coincide con la sesión: el editor se reiniciará para aplicar la API. El player exportado no pasa por aquí.

`RTB_GRAPHICS_API=OpenGL` o `Vulkan` en el entorno pisa el valor al arrancar. Útil en CI.

## Iluminación

Guardar aquí no mueve luces de la escena. Es el default del proyecto: ambiente, sombra de la direccional, volumen DDGI y las dos nieblas. Un `VolumeComponent` puede pisar la niebla en una zona. Ver [Postproceso](../manual/graficos/postproceso.md).
