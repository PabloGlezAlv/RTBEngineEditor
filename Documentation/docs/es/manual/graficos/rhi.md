# OpenGL y Vulkan

El RHI elige el backend al crear la ventana. No se cambia a mitad de proceso: los flags de SDL, los backends de ImGui y los singletons de GPU nacen con la API.

Valores en el `.rtbproj`:

```ini
GraphicsAPI=Vulkan
```

o `GraphicsAPI=OpenGL`.

## En el editor

**Window → Project Settings → Rendering** muestra la API de esta sesión y el selector. Si eliges otra:

1. Se guarda `GraphicsAPI=` en el `.rtbproj`.
2. Si estabas en Play, se hace Stop.
3. Si la escena o el prefab están dirty, sale el diálogo de cambios sin guardar.
4. El editor lanza otro `RTBEngineEditor.exe` y cierra este.

El botón es **Apply & Restart Editor**. El proceso nuevo lee la API con `Project::PeekGraphicsAPI` antes de crear el dispositivo.

CI puede forzar el backend con la variable de entorno `RTB_GRAPHICS_API=OpenGL` o `Vulkan`, sin tocar el proyecto.

## Qué no cambia

El player exportado no usa este reinicio. Su backend es el de la build. Los assets, las escenas y los componentes son los mismos en los dos backends: mallas, luces, sombras, partículas, UBOs de cámara y de iluminación, y el volumen DDGI tienen camino OpenGL y camino Vulkan dentro del motor.

Si un efecto se ve en un backend y no en el otro, el fallo está en el pase de ese backend, no en el `GameObject`.
