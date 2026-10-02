# Audio

The subsystem is FMOD: `AudioSystem` when the application starts, `AudioSourceComponent` on the object.

| Field | Default |
| --- | --- |
| `volume` | `1` |
| `pitch` | `1` |
| `loop` | `false` |
| `playOnStart` | `false` |
| `audioClip` | Clip assigned in the Inspector |

Formats the browser treats as a clip: `.wav`, `.mp3`, `.ogg`, `.flac`.

```cpp
auto* source = GetOwner()->GetComponent<RTBEngine::Scene::AudioSourceComponent>();
source->SetVolume(0.8f);
source->SetPitch(1.0f);
source->SetLoop(false);
source->Play();
source->PlayOneShot();
source->Stop();
```

`PlayOneShot(clip)` uses another clip without replacing the assigned one. `IsPlaying()` queries the FMOD channel.

`OnEnable` / `OnStart` honor `playOnStart`. `OnDisable` cuts the channel so a deactivated object does not keep playing.

The listener follows the active camera. The same `Play` therefore sounds different if the object is far from the game camera. In Edit, scripts do not call `Play`; enter Play to hear gameplay.

If FMOD is not installed in `ThirdParty/fmod/`, the engine has no audio system. `SetupDeps.bat` does not download FMOD: you need the FMOD Studio installer.
