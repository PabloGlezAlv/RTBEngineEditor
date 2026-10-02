# MeshRenderer

`RTBEngine::Scene::MeshRenderer` draws the owner's mesh. Assign the mesh and texture in the Inspector or through the reflected properties. The component resolves those paths to pointers in `OnValidate` and `OnAwake`.

Reflected fields: `meshRef` (null), `textureRef` (null), `colorRef` `(1, 1, 1, 1)`, `shaderRef` `"basic"`, `shaderPropertyOverrides` empty, `meshIndex` `0`, `multiMesh` false.

## GetActiveAnimator

```cpp
Animation::Animator* GetActiveAnimator();
```

Walks the owner and its parents until it finds an `Animator`, and caches the result until the hierarchy changes (`GameObject::GetHierarchyVersion`). The geometry pass and the shadow pass use it for skinning. If no animator is in the chain, it returns null.

**Returns** the `Animator` on this object or an ancestor, or null.

## OnAwake

```cpp
void OnAwake() override;
```

When the component enters the scene it resolves the reflected paths (mesh, texture, shader) to the pointers the renderer draws with. A null `meshRef` leaves the renderer without a mesh until one is assigned.

## OnValidate

```cpp
void OnValidate() override;
```

The Inspector and the loader call this when properties change. It syncs mesh, texture, color, and shader to the render object again, so the change shows up without waiting for Play.

## OnDestroy

```cpp
void OnDestroy() override;
```

Releases what this renderer was holding when the component is removed or the owner is destroyed. It does not draw after that.

## ResetRenderStats

```cpp
static void ResetRenderStats();
```

Zeroes the frame counters for draw calls, triangles, and culled objects. The engine calls it at the start of a render frame, not a gameplay script.

## GetDrawCallCount

```cpp
static uint32_t GetDrawCallCount();
```

**Returns** draw calls accumulated since the last `ResetRenderStats`.

## GetTriangleCount

```cpp
static uint32_t GetTriangleCount();
```

**Returns** triangles submitted since the last `ResetRenderStats`.

## GetCulledObjectCount

```cpp
static uint32_t GetCulledObjectCount();
```

**Returns** how many objects were culled since the last reset.

## IncrementCulledCount

```cpp
static void IncrementCulledCount();
```

Adds one to the culled-object counter. The render pass calls it when a renderer is outside the view. A script does not need it.

## AddInstancedDrawStats

```cpp
static void AddInstancedDrawStats(uint32_t indexCount, uint32_t instanceCount);
```

Adds one instanced draw to the stats: the mesh index count times the instance count. The instancing renderer calls it, not gameplay.

- `indexCount`: indices in the mesh.
- `instanceCount`: copies drawn by that call.
