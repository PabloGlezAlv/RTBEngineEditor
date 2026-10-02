# Assets

Todo lo que el juego carga en runtime vive en `Assets/` o en `Default/` (recursos de fábrica que copia el SDK). Las rutas que guardan las escenas son lógicas: `Assets/Textures/Stone.png`, con barras `/`.

## Extensiones

| Extensión | Qué es | Drag del Content Browser |
| --- | --- | --- |
| `.lua` | Escena | Doble clic carga la escena (solo en Edit) |
| `.prefab` | Prefab | Doble clic abre el modo prefab |
| `.rtbasset` | Data asset reflejado | El Inspector lo edita |
| `.rtbproj` | Proyecto del editor | No es un asset de escena |
| `.navmesh` | Grid horneado junto a la escena | Lo escribe el bake |
| `.cubemap` | Seis caras del skybox | Payload de cubemap |
| `.png` `.jpg` `.jpeg` `.bmp` `.tga` | Textura | Payload de textura |
| `.obj` `.fbx` `.dae` `.gltf` | Malla | Payload de malla |
| `.fbx` | También clip / rig | Slot `RTB_PROPERTY_FBX` |
| `.wav` `.mp3` `.ogg` `.flac` | Clip | Payload de audio |
| `.ttf` `.otf` | Fuente de UI | Payload de fuente |
| `.vert` `.frag` `.glsl` | Shader | — |
| `.h` `.cpp` | Script | El Inspector muestra un preview |

## Default

`Default/` trae shaders, mallas primitivas e iconos del editor (`Default/Icons/`). El build lo copia entero junto a `Assets/`. No metas ahí contenido de tu juego: el SDK puede regenerarlo.

## Navegador de assets del Inspector

El botón `...` de un campo abre un modal filtrado:

| Filtro | Extensiones |
| --- | --- |
| Texture | png, jpg, jpeg, bmp, tga |
| Mesh | obj, fbx, dae, gltf |
| AudioClip | wav, mp3, ogg, flac |
| Font | ttf, otf |
| Cubemap | cubemap |
| Fbx | fbx |

Elige el archivo y el campo guarda la ruta relativa. Puedes soltar el archivo encima del campo si el payload coincide.

## Proyecto

`MyProject.rtbproj` al lado del ejecutable del editor:

```ini
Name=My Game
StartScene=Assets/Scenes/Main.lua
LastOpenScene=Assets/Scenes/DefaultScene.lua
AssetDirectory=Assets
GraphicsAPI=Vulkan
```

`lighting.ini` va al lado y guarda ambiente, sombras, DDGI y niebla. No mezcles esos valores dentro del `.rtbproj`.
