# Playback

The top bar controls editor state and script compilation.

| Button | Edit | Play | Pause |
| --- | --- | --- | --- |
| Play | Enters Play | Disabled | Resumes |
| Pause | Disabled | Pauses | The same button resumes |
| Stop | Disabled | Returns to Edit and reloads | Same |
| Compile Scripts | Compiles | Disabled | Disabled |

Scripts are not compiled in Play: the DLL is loaded.

While MSBuild runs, a modal that cannot be closed shows an indeterminate bar. If it fails, the result lands in the console (`MSBuildNotFound`, a compile error, or a failure to start the process). The MSBuild path the editor uses is Visual Studio 2026 Community.

Compile Scripts builds on another thread. The button reads *Compiling...* until that thread unloads the old DLL and loads the new one.

If Play does nothing and you are inside a prefab, prefab mode is blocking it: return to the scene with **Back to Scene**.
