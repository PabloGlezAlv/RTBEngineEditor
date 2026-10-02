# Mallas y materiales

`MeshRenderer` dibuja una malla en el `GameObject`. Los campos que editas en el Inspector son los proxies reflejados:

| Campo | Uso |
| --- | --- |
| `meshRef` | Malla cargada (OBJ, FBX, glTF, primitivas del editor) |
| `textureRef` | Textura albedo |
| `colorRef` | `Vector4` RGBA. Default blanco opaco |
| `shaderRef` | Nombre de shader. Default `"basic"` |
| `shaderPropertyOverrides` | Overrides de propiedades del shader |
| `meshIndex` | Submesh cuando el asset trae varias |
| `multiMesh` | Dibuja todas las submallas |

Arrastra un modelo desde el Content Browser al campo de malla, y una imagen al de textura. El payload del drag es la ruta lógica `Assets/...`.

## Varias submallas

Un FBX de personaje suele traer más de una malla. Con `multiMesh` el renderer guarda un material por submesh. `meshIndex` elige una sola cuando no quieres todas.

## Skinning

Si hay un `Animator` en el mismo objeto o en un padre, `GetActiveAnimator()` lo encuentra y lo cachea hasta que la jerarquía cambia. El paso de geometría y el de sombras usan ese animator para skinnear. Sin huesos, la malla se dibuja rígida con la `GetWorldMatrix()` del objeto.

## Estadísticas

```cpp
RTBEngine::Scene::MeshRenderer::ResetRenderStats();
uint32_t draws = RTBEngine::Scene::MeshRenderer::GetDrawCallCount();
uint32_t tris = RTBEngine::Scene::MeshRenderer::GetTriangleCount();
uint32_t culled = RTBEngine::Scene::MeshRenderer::GetCulledObjectCount();
```

El frustum descarta lo que no se ve y suma en `GetCulledObjectCount`. El instancing suma con `AddInstancedDrawStats`.

## Shaders y texturas

Los shaders de usuario viven en `Assets/Shaders/` (`.vert`, `.frag`, `.glsl`). Las texturas comunes son `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`. El motor las carga con stb_image a través del `ResourceManager`.

Un objeto sin `MeshRenderer` no se pickea en la Scene View: el rayo del editor prueba el AABB de las mallas.
