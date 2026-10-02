# OpenGL and Vulkan

The RHI picks the backend when the window is created. It does not change mid-process: SDL flags, ImGui backends, and GPU singletons are born with the API.

Values in the `.rtbproj`:

```ini
GraphicsAPI=Vulkan
```

or `GraphicsAPI=OpenGL`.

## In the editor

**Window → Project Settings → Rendering** shows this session's API and the selector. If you pick another one:

1. `GraphicsAPI=` is saved in the `.rtbproj`.
2. If you were in Play, it Stops.
3. If the scene or the prefab is dirty, the unsaved-changes dialog appears.
4. The editor launches another `RTBEngineEditor.exe` and closes this one.

The button is **Apply & Restart Editor**. The new process reads the API with `Project::PeekGraphicsAPI` before creating the device.

CI can force the backend with the environment variable `RTB_GRAPHICS_API=OpenGL` or `Vulkan`, without touching the project.

## What does not change

The exported player does not use this restart. Its backend is the one from the build. Assets, scenes, and components are the same on both backends: meshes, lights, shadows, particles, camera and lighting UBOs, and the DDGI volume have an OpenGL path and a Vulkan path inside the engine.

If an effect shows on one backend and not the other, the fault is in that backend's pass, not in the `GameObject`.
