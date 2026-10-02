# Project settings

**Window → Project Settings**.

| Section | File | Contents |
| --- | --- | --- |
| Rendering | `.rtbproj` (`GraphicsAPI=`) | OpenGL or Vulkan, and the API of this session |
| Lighting | `lighting.ini` | Ambient, shadows, DDGI, distance fog, volumetric fog |
| Save | Both | **Save Project Settings** does not restart the editor |

## Switching graphics API

The SDL window and the device already exist. The editor does not switch backend while it is running. **Apply & Restart Editor**:

1. Writes `GraphicsAPI` into the `.rtbproj`.
2. Stops if you were in Play.
3. Asks if there are unsaved changes.
4. Launches another process and closes this one.

The panel says so when the selection does not match the running session: the editor will restart to apply the API. The exported player does not go through this path.

`RTB_GRAPHICS_API=OpenGL` or `Vulkan` in the environment overrides the value at startup. Useful in CI.

## Lighting

Saving here does not move lights in the scene. It is the project default: ambient, the directional shadow, the DDGI volume, and both fogs. A `VolumeComponent` can override fog inside a zone. See [Post-processing](../manual/graficos/postproceso.md).
