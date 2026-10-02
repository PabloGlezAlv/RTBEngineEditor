# Compilar el juego

**File → Build** o `Ctrl+B`.

| Campo | Efecto |
| --- | --- |
| Game Name | Nombre del `.exe` |
| Output Directory | Carpeta destino. El botón Browse abre el selector de carpetas de Windows |
| Start Scene | Escena con la que arranca el player. Se guarda también en el proyecto |
| Window | Ancho, alto, fullscreen |

## Qué copia

1. Crea la carpeta.
2. Copia `RTBPlayer.exe` como `{Nombre}.exe`.
3. Copia las DLL de `RTBEngine_SDK/Bin/`.
4. Copia `Default/` y `Assets/` (incluye `GameScripts.dll`).
5. Escribe `game.cfg`:

```ini
[Game]
name=MyGame

[Window]
width=1280
height=720
fullscreen=0

[Scene]
startScene=Assets/Scenes/Main.lua
```

La ruta de escena usa barras `/` y el prefijo `Assets/`. El player no inventa otra escena si esa falta: falla con la que escribiste.

## Errores

| Resultado | Causa |
| --- | --- |
| Success | La carpeta está lista |
| NoProjectLoaded | No hay proyecto activo |
| InvalidOutputDirectory | Ruta vacía o no se pudo crear |
| PlayerNotFound | No está `RTBPlayer.exe` junto al editor |
| CopyFailed | Un archivo no se copió |
| ConfigWriteFailed | No se pudo escribir `game.cfg` |

Compila los scripts antes del build si has tocado `Assets/Scripts`. La DLL que se copia es la que está en el proyecto, no un objeto a medias del compilador.
