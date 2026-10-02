# Instalación

El motor y el editor se compilan en Windows con **Visual Studio 2026** (toolset MSVC v145), **C++17** y plataforma **x64**. Configuraciones: `Debug` y `Release`.

## Dependencias del motor

En el repositorio del motor, una sola vez:

```bat
SetupDeps.bat
```

Descarga y compila en `ThirdParty/`: SDL2 2.32.10, GLEW 2.1.0, Bullet 3.25, Assimp 5.4.3, Lua 5.4.8, LuaBridge, ImGui 1.92.5 y stb_image. **FMOD** se instala aparte con el instalador de FMOD Studio y se espera en `ThirdParty/fmod/`.

## SDK

```bat
BuildSDK.bat
```

1. Compila `RTBEngine.dll` y `RTBEngine.lib` en Debug y Release.
2. Copia los headers públicos a `RTBEngine_SDK/Include/RTBEngine/`.
3. Copia la `.lib` a `RTBEngine_SDK/Lib/`.

El editor referencia ese SDK, no el árbol de fuentes del motor. `RTB_EXPORTS` activa `__declspec(dllexport)` al compilar la DLL. Quien consume el SDK ve `RTB_API` como `dllimport`.

## Editor y scripts

Solución: `RTBEngineEditor.sln`.

| Proyecto | Salida |
| --- | --- |
| Editor | `RTBEngineEditor.exe` |
| `GameScripts` | `GameScripts.dll` |
| Player de export | `RTBPlayer.exe` |

`GameScripts` descubre solo `Assets/Scripts/**/*.h` y `*.cpp`. No hace falta editar el `.vcxproj` para un componente nuevo.

Orden cuando cambia la API pública:

1. `BuildSDK.bat`
2. Compilar `GameScripts` (`GameScripts\build.bat` o el botón **Compile Scripts**)
3. Abrir o recompilar el editor

## Librerías

| Librería | Para qué |
| --- | --- |
| SDL2 | Ventana, contexto, eventos |
| GLEW | Extensiones OpenGL |
| Vulkan SDK | Backend Vulkan del RHI |
| Bullet | Cuerpos rígidos y colisión |
| Assimp | FBX, OBJ, glTF, DAE |
| FMOD | Audio espacial |
| ImGui + ImGuizmo 1.83 | UI del editor y gizmos |
| Lua + LuaBridge | Escenas y prefabs |

## Relay

El relay es .NET 10. En local, sin Docker:

```powershell
cd RTBOnlineRelay
dotnet run
```

API: `http://localhost:8080/api/v1`. UDP de gameplay: `27100`. Con Docker, `docker compose up -d --build` después de copiar `.env.example` a `.env`. Detalle en [Relay de Internet](../online/relay.md).
