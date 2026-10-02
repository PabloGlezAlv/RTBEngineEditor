# Ventanas opcionales

**Window** abre paneles que nacen cerrados. Cerrar con la X no deja una pestaña vacía: el panel deja de hacer `Begin`. Al reabrir, se acopla a un dock que ya exista (otra ventana opcional, Inspector, jerarquía, consola, navegador, Scene o Game).

Se recuerdan en `%LOCALAPPDATA%\RTBEngineEditor\EditorWindowPrefs.json`.

| Menú | Para qué |
| --- | --- |
| Online | Puertos LAN, URL del relay, lanzar una segunda instancia |
| Physics Layers | Nombres y matriz de colisión |
| Navigation Debug | Overlay del grid en la Scene View |
| Project Settings | API gráfica e iluminación |

El menú Online **no** elige LAN u Online para la partida. Eso lo hace el menú del juego. Aquí solo preparas puertos y la URL. El flujo está en [Partida en LAN](../online/lan.md).

Navigation Debug no hornea. El bake está en el `NavGridComponent`. Physics Layers sí escribe el proyecto al pulsar Save.

La visibilidad de cada panel y los toggles del nav debug se guardan al cerrar el editor, al usar el menú Window y al editar el overlay.
