# Animator

`RTBEngine::Animation::Animator`. `LoadClipFromFbx` stores an alias and `Play` uses that alias. Paths are `Assets/...fbx`. An empty alias or an empty path makes `LoadClipFromFbx` return false.

```cpp
anim->LoadClipFromFbx("Run", "Assets/Characters/Knight/Run.fbx");
anim->Play("Run", true);
```

## ReloadClipLibrary

```cpp
void ReloadClipLibrary();
```

Reloads the skeleton, meshes, and clips that come from `modelRef` and `additionalModels`. Aliases you added with `LoadClipFromFbx` that do not come from those files are kept. The current clip is dropped during the reload and, if it was playing, the animator tries to resume it by name.

## AddClip

```cpp
void AddClip(const std::string& name, std::shared_ptr<AnimationClip> clip);
```

Puts an already built clip in the library under that name. `Play` finds it by `name` after `NormalizeClipName` (it strips legacy prefixes such as `mixamo.com|`).

- `name`: alias you later pass to `Play`.
- `clip`: clip in memory. An empty shared pointer does not leave a playable clip.

## LoadClipFromFbx

```cpp
bool LoadClipFromFbx(const std::string& alias, const std::string& sourceFbx);
```

Loads clips from the FBX and stores them under `alias`. If `sourceFbx` contains `path|clipName`, it splits the file and the clip inside it. An empty alias or an empty path returns false and loads nothing.

- `alias`: name for `Play`.
- `sourceFbx`: an `Assets/...fbx` path, or `path|clip` for one clip in the file.

**Returns** true if the clip landed in the library.

## ClearClips

```cpp
void ClearClips();
```

Empties the clip library. `Play` after this finds no names until you load again.

## Play

```cpp
void Play(const std::string& clipName, bool loop = true);
```

Looks up the clip by normalized name, sets time to 0, and marks it playing. If the name is not in the library, the current clip stays as it was. `loop` true (the default) repeats at the end; false stays on the last frame as the component advances.

- `clipName`: alias from `LoadClipFromFbx` or `AddClip`.
- `loop`: true to repeat. The default is true.

## Stop

```cpp
void Stop();
```

Cuts playback: `playing`, pause, and the held pose go false, time returns to 0, and the current clip is forgotten. If there is a skeleton, the bones go back to the bind pose. It does not clear the library: `Play` can pick an alias again.

## Pause

```cpp
void Pause();
```

If `IsPlaying` is true, it freezes clip time: `IsPaused` becomes true and `IsPlaying` stays true. If it was not playing, it does nothing.

## Resume

```cpp
void Resume();
```

If the clip is still playing (`IsPlaying`), it clears pause and time continues from `GetCurrentTime`. If `Stop` already cleared the clip, nothing restarts: call `Play`.

## HoldCurrentPose

```cpp
void HoldCurrentPose();
```

Stops playback and leaves the bones at the current time, clamped to the clip duration. With no skeleton or no current clip, it does nothing. `ShouldSkinMesh` stays true while the pose is held.

## IsPlaying

```cpp
bool IsPlaying() const;
```

**Returns** true if `Play` left the playback flag set. Pause does not clear it: check `IsPaused`. The default is false.

## IsPaused

```cpp
bool IsPaused() const;
```

**Returns** true after `Pause` and before `Resume` or a new `Play`.

## IsHoldingPose

```cpp
bool IsHoldingPose() const;
```

**Returns** true if `HoldCurrentPose` locked the pose and nobody has called `Play` since.

## SetSpeed

```cpp
void SetSpeed(float spd);
```

Scales how fast clip time advances. 1 is real time, 0 freezes it without going through `Pause`, 2 doubles it. The field default is 1.

- `spd`: multiplier on the animation delta.

## GetSpeed

```cpp
float GetSpeed() const;
```

**Returns** the multiplier. The default is 1.

## GetCurrentTime

```cpp
float GetCurrentTime() const;
```

**Returns** seconds into the current clip. `Play` sets it to 0. With no clip it stays 0.

## SelectClip

```cpp
void SelectClip(const std::string& clipName, bool loop = true);
```

Picks the clip and sets time to 0, but does not raise the `playing` flag. Use it to leave a clip ready. If the name does not exist, nothing changes.

- `clipName`: normalized alias.
- `loop`: value stored in `looping`. The default is true.

## ReloadKeyClips

```cpp
void ReloadKeyClips();
```

Resolves the reflected `keyClips` list (key, FBX, loop) into loaded clips again. Call it if you change `keyClips` from code and want `PlayKey` to see the new entries.

## SetKeyClip

```cpp
bool SetKeyClip(const std::string& key, const std::string& clipFbxRef, bool loop);
```

Binds a gameplay key (`"Attack"`) to an FBX and to whether it should loop, then reloads keys. If the key already existed, it replaces the path and the loop. An empty key or an empty FBX returns false and does not touch the list.

- `key`: name you pass to `PlayKey`. Empty returns false.
- `clipFbxRef`: FBX path, with the same convention as `LoadClipFromFbx`. Empty returns false.
- `loop`: whether `PlayKey(key)` without the bool should repeat.

**Returns** true if the entry is in `keyClips` and a reload was requested. The reload can still fail to read the file; this function's bool is not the load's bool.

## HasKey

```cpp
bool HasKey(const std::string& key) const;
```

**Returns** true if `key` is in `keyClips`. An empty or unknown key returns false.

- `key`: entry name.

## PlayKey

```cpp
bool PlayKey(const std::string& key);
```

Finds the key and calls `Play` with the normalized alias and the loop stored on the entry. If the key does not exist, it returns false and does not touch the current clip.

- `key`: key from `SetKeyClip` or the Inspector.

**Returns** true if a current clip exists after `Play`.

## PlayKey

```cpp
bool PlayKey(const std::string& key, bool loop);
```

Same as `PlayKey(key)`, but this call's `loop` overrides the entry's loop. An unknown key returns false and does not change the clip.

- `key`: key.
- `loop`: whether this playback repeats.

**Returns** true if a clip was left playing.

## IsPlayingKey

```cpp
bool IsPlayingKey(const std::string& key) const;
```

**Returns** true if `IsPlaying` is true and the current clip name, once normalized, is that key. If it is not playing, or the key is empty, it returns false. Pause does not clear it: `IsPlaying` stays true.

- `key`: key to test.

## NotifyKeyFinished

```cpp
void NotifyKeyFinished(const std::string& key);
```

The animator calls this when a non-looping key clip ends, and it fires the key-finished event. It sits in the private section of the class: a script does not call it. The component already publishes the event; there is no need to invoke this method.

- `key`: key treated as finished.

## HasBones

```cpp
bool HasBones() const;
```

**Returns** true if there is a skeleton and at least one bone. With no FBX loaded it is false.

## ShouldSkinMesh

```cpp
bool ShouldSkinMesh() const;
```

**Returns** true if there are bones, the final matrices are not empty, and the animator is playing or holding a pose. The renderer uses it to decide whether to upload skinning. Stopped and without `HoldCurrentPose`, it is false.

## CreateBoneGameObjects

```cpp
void CreateBoneGameObjects(Scene::Scene* scene);
```

Creates one `GameObject` per bone and marks it with `SetAnimatorBone`. With no scene, there is nowhere to insert them. Calling it twice should not duplicate once `AreBoneGOsCreated` is true: check that first.

- `scene`: scene that will own the bones. Null creates nothing.

## SyncBoneGameObjects

```cpp
void SyncBoneGameObjects();
```

Copies the skeleton's current pose onto the bone `GameObject`s. With no bones created, there are no transforms to move. The animator already does this on its update when the bones exist; call it if you just created them and want this frame's pose.

## AreBoneGOsCreated

```cpp
bool AreBoneGOsCreated() const;
```

**Returns** true after a `CreateBoneGameObjects` that actually built the list. The default is false.

## IsBoneGameObject

```cpp
bool IsBoneGameObject(const Scene::GameObject* go) const;
```

**Returns** true if `go` is one of this animator's bones. Null returns false.

- `go`: object to test.
