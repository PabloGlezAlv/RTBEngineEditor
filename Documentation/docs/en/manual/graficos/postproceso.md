# Post-processing

After geometry, the frame can apply bloom, distance fog, and volumetric fog. Project values live in `lighting.ini`. Local overrides live on `VolumeComponent`.

## VolumeComponent

A volume is a box in the world. If the camera is inside it (or in the blend band), its effects enter the stack.

In the Inspector the component does not use the generic list. The custom drawer shows:

- A header checkbox that enables the whole volume
- **Zone**: global, box size, blend distance, priority, weight
- **Distance Fog** and **Volumetric Fog**, each with its own checkbox

Those checkboxes map to `overrideDistanceFog` and `overrideVolumetricFog`. With the effect off, its parameters are not applied.

A **global** volume ignores the box and affects the whole scene. Priority breaks ties when several volumes overlap. Weight scales the contribution.

## Project

**Window → Project Settings** edits ambient, shadows, the DDGI volume, distance fog, and volumetric fog. **Save Project Settings** writes `lighting.ini` and does not change the graphics API.

DDGI is probe-based global illumination. The volume is configured in that panel, not per GameObject. Changing the graphics API (OpenGL / Vulkan) is a separate setting and requires restarting the editor. See [OpenGL and Vulkan](rhi.md).

## Game View

Post-processing on the game view is that of the main camera's frame. Editor overlays (grid, colliders, navigation cells) are drawn only in the Scene View, after geometry and before the framebuffer is released.
