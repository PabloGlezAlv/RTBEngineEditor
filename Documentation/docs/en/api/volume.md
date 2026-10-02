# Volume

`RTBEngine::Scene::VolumeComponent` feeds distance fog and volumetric fog into the post-process stack while the camera is inside the zone, or always if the volume is global.

It has its own Inspector:

| Block | Fields |
| --- | --- |
| Header | Turns the volume on or off in the stack |
| Zone | Global, box size, blend, priority, weight |
| Distance Fog | `overrideDistanceFog` and parameters |
| Volumetric Fog | `overrideVolumetricFog` and parameters |
| Bloom | `overrideBloom`, `bloomEnabled`, `bloomThreshold`, `bloomIntensity` |

If the effect checkbox is off, that effect does not enter the blend even when the volume is active.

Project fog, with no volume, is edited in `lighting.ini` from Project Settings. The volume only adds local overrides. Two overlapping volumes are ordered by priority and scaled by weight.

You do not need to call it from a script for it to work: place it in the scene and adjust the box on the transform and in Zone.

## ToProfile

```cpp
Rendering::VolumeProfile ToProfile() const;
```

Copies this component's overrides into a `VolumeProfile`: distance fog, volumetric fog, and bloom, each with its `override*` flag. It does not read `isGlobal`, `size`, `priority`, `blendDistance`, or `weight`; the code that blends the stack applies those. The engine calls it when evaluating the volume. A script only needs it to read the profile the current fields would produce.

**Returns** the profile. Effects whose override checkbox is off still travel in the struct, and the blend ignores them.
