# Building the game

**File → Build** or `Ctrl+B`.

| Field | Effect |
| --- | --- |
| Game Name | The `.exe` name |
| Output Directory | Destination folder. Browse opens the Windows folder picker |
| Start Scene | Scene the player boots into. Also stored on the project |
| Window | Width, height, fullscreen |

## What gets copied

1. Creates the folder.
2. Copies `RTBPlayer.exe` as `{Nombre}.exe`.
3. Copies the DLLs from `RTBEngine_SDK/Bin/`.
4. Copies `Default/` and `Assets/` (including `GameScripts.dll`).
5. Writes `game.cfg`:

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

The scene path uses `/` and the `Assets/` prefix. The player does not substitute another scene if that one is missing: it fails on the path you wrote.

## Errors

| Result | Cause |
| --- | --- |
| Success | The folder is ready |
| NoProjectLoaded | No active project |
| InvalidOutputDirectory | Empty path, or the folder could not be created |
| PlayerNotFound | `RTBPlayer.exe` is not next to the editor |
| CopyFailed | A file copy failed |
| ConfigWriteFailed | `game.cfg` could not be written |

Compile scripts before the build if you touched `Assets/Scripts`. The DLL that is copied is the one in the project, not a half-written compiler output.
