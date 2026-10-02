# Audio

`RTBEngine::Scene::AudioSourceComponent`. `OnDisable` stops the channel. Assign the clip in the Inspector or with `SetClip` once you have the pointer the engine loaded.

Fields: `volume` 1, `pitch` 1, `loop` false, `playOnStart` false, `audioClip` null.

## SetClip

```cpp
void SetClip(Audio::AudioClip* clip);
```

Stores the clip `Play` and argument-less `PlayOneShot()` will use. Null leaves the source with no clip: `Play` returns without a sound.

- `clip`: an already loaded clip, or null.

## GetClip

```cpp
Audio::AudioClip* GetClip() const;
```

**Returns** the stored clip, or null if there is none.

## Play

```cpp
void Play();
```

Starts the clip on this source's channel, with the current volume, pitch, and loop. If there is no clip, the clip is not loaded, or FMOD has no system, it does nothing. A second `Play` asks for another `playSound` and keeps the new channel.

## PlayOneShot

```cpp
void PlayOneShot();
```

Fires the stored clip on a separate channel, without looping and without replacing the channel `Play` / `Stop` use. If there is no clip or it is not loaded, nothing plays. This is `PlayOneShot(GetClip())`.

## PlayOneShot

```cpp
void PlayOneShot(Audio::AudioClip* clipOverride);
```

Same, with another clip. Null or an unloaded clip does not play. Volume and pitch come from this source. The mode is always non-looping, even when `loop` is true.

- `clipOverride`: clip to fire. Null does nothing.

## Stop

```cpp
void Stop();
```

Stops the `Play` channel and releases it. If there was no channel, it does nothing. It does not cut a one-shot that already went out on another channel.

## Pause

```cpp
void Pause();
```

Pauses the `Play` channel. With no channel, it does nothing.

## Resume

```cpp
void Resume();
```

Clears the pause on the `Play` channel. With no channel, it does nothing.

## IsPlaying

```cpp
bool IsPlaying() const;
```

**Returns** true if the `Play` channel exists and FMOD reports it playing. With no channel it is false. A one-shot does not count.

## SetVolume

```cpp
void SetVolume(float volume);
```

Linear volume of the source. `Play` and `PlayOneShot` apply it when they start. The default is 1.

- `volume`: 0 is silence, 1 is the clip's level.

## GetVolume

```cpp
float GetVolume() const;
```

**Returns** the stored volume. The default is 1.

## SetPitch

```cpp
void SetPitch(float pitch);
```

Pitch applied when `Play` or a one-shot starts. The default is 1.

- `pitch`: 1 is the clip's pitch.

## GetPitch

```cpp
float GetPitch() const;
```

**Returns** the stored pitch. The default is 1.

## SetLoop

```cpp
void SetLoop(bool loop);
```

Whether the next `Play` should repeat. It does not change a one-shot: those always go out without looping. The default is false.

- `loop`: true to repeat on `Play`.

## IsLooping

```cpp
bool IsLooping() const;
```

**Returns** the loop flag. The default is false.

## SetPlayOnStart

```cpp
void SetPlayOnStart(bool playOnStart);
```

Whether `OnStart` should call `Play`. The default is false. Setting it true after `OnStart` already ran does not fire the sound: call `Play`.

- `playOnStart`: true to play when the component starts.

## GetPlayOnStart

```cpp
bool GetPlayOnStart() const;
```

**Returns** the flag. The default is false.
