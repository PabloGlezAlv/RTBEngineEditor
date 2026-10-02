# Installation

The engine and the editor build on Windows with **Visual Studio 2026** (MSVC v145), **C++17**, and **x64**. Configurations: `Debug` and `Release`.

## Engine dependencies

In the engine repository, once:

```bat
SetupDeps.bat
```

It downloads and builds into `ThirdParty/`: SDL2 2.32.10, GLEW 2.1.0, Bullet 3.25, Assimp 5.4.3, Lua 5.4.8, LuaBridge, ImGui 1.92.5, and stb_image. **FMOD** is installed separately with the FMOD Studio installer and is expected at `ThirdParty/fmod/`.

## SDK

```bat
BuildSDK.bat
```

1. Builds `RTBEngine.dll` and `RTBEngine.lib` in Debug and Release.
2. Copies public headers to `RTBEngine_SDK/Include/RTBEngine/`.
3. Copies the `.lib` to `RTBEngine_SDK/Lib/`.

The editor references that SDK, not the engine source tree. `RTB_EXPORTS` turns `RTB_API` into `__declspec(dllexport)` while compiling the DLL. Consumers see `dllimport`.

## Editor and scripts

Solution: `RTBEngineEditor.sln`.

| Project | Output |
| --- | --- |
| Editor | `RTBEngineEditor.exe` |
| `GameScripts` | `GameScripts.dll` |
| Exported player | `RTBPlayer.exe` |

`GameScripts` picks up `Assets/Scripts/**/*.h` and `*.cpp` on its own. A new component does not require a `.vcxproj` edit.

When the public API changes:

1. `BuildSDK.bat`
2. Build `GameScripts` (`GameScripts\build.bat` or the **Compile Scripts** button)
3. Rebuild or restart the editor

## Libraries

| Library | Role |
| --- | --- |
| SDL2 | Window, context, events |
| GLEW | OpenGL extensions |
| Vulkan SDK | Vulkan RHI backend |
| Bullet | Rigid bodies and collision |
| Assimp | FBX, OBJ, glTF, DAE |
| FMOD | Spatial audio |
| ImGui + ImGuizmo 1.83 | Editor UI and gizmos |
| Lua + LuaBridge | Scenes and prefabs |

## Relay

The relay is .NET 10. Locally, without Docker:

```powershell
cd RTBOnlineRelay
dotnet run
```

API: `http://localhost:8080/api/v1`. Gameplay UDP: `27100`. With Docker, `docker compose up -d --build` after copying `.env.example` to `.env`. Details in [Internet relay](../online/relay.md).
