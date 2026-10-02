# MeshRenderer

`RTBEngine::Scene::MeshRenderer` dibuja la malla del dueño. Asigna malla y textura en el Inspector o en las propiedades reflejadas. El componente resuelve esas rutas a punteros en `OnValidate` y `OnAwake`.

Campos reflejados: `meshRef` (null), `textureRef` (null), `colorRef` `(1, 1, 1, 1)`, `shaderRef` `"basic"`, `shaderPropertyOverrides` vacío, `meshIndex` `0`, `multiMesh` false.

## GetActiveAnimator

```cpp
Animation::Animator* GetActiveAnimator();
```

Sube por el dueño y sus padres hasta encontrar un `Animator`, y cachea el resultado hasta que la jerarquía cambie (`GameObject::GetHierarchyVersion`). El pase de geometría y el de sombras lo usan para skinning. Si no hay animator en la cadena, devuelve null.

**Devuelve** el `Animator` de este objeto o de un antecesor, o null.

## OnAwake

```cpp
void OnAwake() override;
```

Al entrar en la escena resuelve las rutas reflejadas (malla, textura, shader) a los punteros que el renderer usa para dibujar. Un `meshRef` null deja el renderer sin malla hasta que se asigne.

## OnValidate

```cpp
void OnValidate() override;
```

El Inspector y el loader lo llaman al cambiar propiedades. Vuelve a sincronizar malla, textura, color y shader con el objeto de render, para que el cambio se vea sin esperar a Play.

## OnDestroy

```cpp
void OnDestroy() override;
```

Suelta lo que este renderer retenía al quitarse el componente o destruirse el dueño. Después de esto no dibuja.

## ResetRenderStats

```cpp
static void ResetRenderStats();
```

Pone a cero los contadores de draw calls, triángulos y objetos descartados del frame. Lo llama el motor al empezar un frame de render, no un script de gameplay.

## GetDrawCallCount

```cpp
static uint32_t GetDrawCallCount();
```

**Devuelve** los draw calls acumulados desde el último `ResetRenderStats`.

## GetTriangleCount

```cpp
static uint32_t GetTriangleCount();
```

**Devuelve** los triángulos enviados desde el último `ResetRenderStats`.

## GetCulledObjectCount

```cpp
static uint32_t GetCulledObjectCount();
```

**Devuelve** cuántos objetos se descartaron por culling desde el último reset.

## IncrementCulledCount

```cpp
static void IncrementCulledCount();
```

Suma uno al contador de objetos descartados. Lo llama el pase de render cuando un renderer no entra en la vista. Un script no lo necesita.

## AddInstancedDrawStats

```cpp
static void AddInstancedDrawStats(uint32_t indexCount, uint32_t instanceCount);
```

Suma a las estadísticas un draw instanciado: los índices del mesh por el número de instancias. Lo llama el renderer de instancias, no el gameplay.

- `indexCount`: índices del mesh.
- `instanceCount`: copias dibujadas en esa llamada.
