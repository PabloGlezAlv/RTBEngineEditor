# Animación

`RTBEngine::Animation::Animator` reproduce clips sobre un esqueleto importado de FBX (Assimp).

## Assets

| Campo / método | Uso |
| --- | --- |
| `modelRef` | FBX del rig |
| `additionalModels` | Packs extra (KayKit, Mixamo, …) |
| `ReloadClipLibrary()` | Reconstruye la librería tras cambiar los FBX |
| `LoadClipFromFbx(alias, ruta)` | Registra un clip con un nombre |
| `Play(nombre, loop)` | Reproduce |
| `Stop` / `Pause` / `Resume` | Control |
| `SetSpeed` / `GetSpeed` | Escala el tiempo del clip |
| `IsPlaying` | Hay un clip en curso |

```cpp
auto* anim = GetOwner()->GetComponent<RTBEngine::Animation::Animator>();
anim->LoadClipFromFbx("Idle", "Assets/Characters/Knight/Idle.fbx");
anim->Play("Idle", true);
anim->SetSpeed(1.0f);
```

## Clips por clave

Para no esparcir nombres de archivo por el gameplay:

```cpp
anim->SetKeyClip("Idle", "Assets/Characters/Knight/Idle.fbx", true);
anim->PlayKey("Idle");
bool idle = anim->IsPlayingKey("Idle");
```

`PlayKey(key, loop)` fuerza el loop de esa llamada. `HasKey` comprueba que la clave existe. `NotifyKeyFinished` avisa al terminar un clip no cíclico.

## Huesos

`HasBones()` es true si el esqueleto tiene huesos. `ShouldSkinMesh()` es true cuando además hay matrices y el animator está en play o manteniendo pose (`HoldCurrentPose`).

`CreateBoneGameObjects(scene)` crea un `GameObject` por hueso. Esos objetos quedan marcados como hueso de animator. `SyncBoneGameObjects` copia la pose. En modo prefab, esos huesos todavía buscan la escena activa: es una limitación conocida, no los uses como anclaje de autor dentro del prefab.

## En un controlador

El patrón del personaje:

1. El `Animator` tiene `modelRef` y, si hace falta, `additionalModels`.
2. El controlador declara `RTB_PROPERTY_COMPONENT(animator, Animator)` y un `RTB_PROPERTY_FBX` por clip.
3. En `OnStart`, `LoadClipFromFbx` o `SetKeyClip`, luego `Play`.
4. En `OnUpdate`, elige idle / walk / run según la velocidad.

Cambia los FBX en el Inspector y el componente llama `ReloadClipLibrary()` antes de `OnValidate`.

La Scene View puede previsualizar el animator en Edit. La lógica del controlador no corre hasta Play.
