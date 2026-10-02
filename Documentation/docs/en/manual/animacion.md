# Animation

`RTBEngine::Animation::Animator` plays clips on a skeleton imported from FBX (Assimp).

## Assets

| Field / method | Use |
| --- | --- |
| `modelRef` | Rig FBX |
| `additionalModels` | Extra packs (KayKit, Mixamo, …) |
| `ReloadClipLibrary()` | Rebuilds the library after the FBX files change |
| `LoadClipFromFbx(alias, ruta)` | Registers a clip under a name |
| `Play(nombre, loop)` | Plays |
| `Stop` / `Pause` / `Resume` | Control |
| `SetSpeed` / `GetSpeed` | Scales clip time |
| `IsPlaying` | A clip is in progress |

```cpp
auto* anim = GetOwner()->GetComponent<RTBEngine::Animation::Animator>();
anim->LoadClipFromFbx("Idle", "Assets/Characters/Knight/Idle.fbx");
anim->Play("Idle", true);
anim->SetSpeed(1.0f);
```

## Clips by key

So gameplay does not scatter file names:

```cpp
anim->SetKeyClip("Idle", "Assets/Characters/Knight/Idle.fbx", true);
anim->PlayKey("Idle");
bool idle = anim->IsPlayingKey("Idle");
```

`PlayKey(key, loop)` forces the loop for that call. `HasKey` checks that the key exists. `NotifyKeyFinished` signals when a non-looping clip ends.

## Bones

`HasBones()` is true if the skeleton has bones. `ShouldSkinMesh()` is true when there are also matrices and the animator is playing or holding a pose (`HoldCurrentPose`).

`CreateBoneGameObjects(scene)` creates one `GameObject` per bone. Those objects are marked as animator bones. `SyncBoneGameObjects` copies the pose. In prefab mode, those bones still look up the active scene: that is a known limitation. Do not use them as authored anchors inside the prefab.

## In a controller

The character pattern:

1. The `Animator` has `modelRef` and, if needed, `additionalModels`.
2. The controller declares `RTB_PROPERTY_COMPONENT(animator, Animator)` and one `RTB_PROPERTY_FBX` per clip.
3. In `OnStart`, `LoadClipFromFbx` or `SetKeyClip`, then `Play`.
4. In `OnUpdate`, pick idle / walk / run from speed.

Change the FBX files in the Inspector and the component calls `ReloadClipLibrary()` before `OnValidate`.

The Scene View can preview the animator in Edit. Controller logic does not run until Play.
