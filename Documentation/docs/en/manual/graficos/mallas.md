# Meshes and materials

`MeshRenderer` draws a mesh on the `GameObject`. The fields you edit in the Inspector are the reflected proxies:

| Field | Use |
| --- | --- |
| `meshRef` | Loaded mesh (OBJ, FBX, glTF, editor primitives) |
| `textureRef` | Albedo texture |
| `colorRef` | `Vector4` RGBA. Default opaque white |
| `shaderRef` | Shader name. Default `"basic"` |
| `shaderPropertyOverrides` | Shader property overrides |
| `meshIndex` | Submesh when the asset has several |
| `multiMesh` | Draws every submesh |

Drag a model from the Content Browser onto the mesh field, and an image onto the texture field. The drag payload is the logical path `Assets/...`.

## Several submeshes

A character FBX usually brings more than one mesh. With `multiMesh` the renderer stores one material per submesh. `meshIndex` picks a single one when you do not want all of them.

## Skinning

If there is an `Animator` on the same object or on a parent, `GetActiveAnimator()` finds it and caches it until the hierarchy changes. The geometry pass and the shadow pass use that animator to skin. Without bones, the mesh is drawn rigid with the object's `GetWorldMatrix()`.

## Statistics

```cpp
RTBEngine::Scene::MeshRenderer::ResetRenderStats();
uint32_t draws = RTBEngine::Scene::MeshRenderer::GetDrawCallCount();
uint32_t tris = RTBEngine::Scene::MeshRenderer::GetTriangleCount();
uint32_t culled = RTBEngine::Scene::MeshRenderer::GetCulledObjectCount();
```

The frustum drops what is not visible and adds it to `GetCulledObjectCount`. Instancing adds through `AddInstancedDrawStats`.

## Shaders and textures

User shaders live in `Assets/Shaders/` (`.vert`, `.frag`, `.glsl`). Common textures are `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`. The engine loads them with stb_image through the `ResourceManager`.

An object without a `MeshRenderer` is not picked in the Scene View: the editor ray tests the mesh AABBs.
