# Audio

El subsistema es FMOD: `AudioSystem` al arrancar la aplicación, `AudioSourceComponent` en el objeto.

| Campo | Default |
| --- | --- |
| `volume` | `1` |
| `pitch` | `1` |
| `loop` | `false` |
| `playOnStart` | `false` |
| `audioClip` | Clip asignado en el Inspector |

Formatos que el navegador trata como clip: `.wav`, `.mp3`, `.ogg`, `.flac`.

```cpp
auto* source = GetOwner()->GetComponent<RTBEngine::Scene::AudioSourceComponent>();
source->SetVolume(0.8f);
source->SetPitch(1.0f);
source->SetLoop(false);
source->Play();
source->PlayOneShot();
source->Stop();
```

`PlayOneShot(clip)` usa otro clip sin sustituir el asignado. `IsPlaying()` consulta el canal FMOD.

`OnEnable` / `OnStart` respetan `playOnStart`. `OnDisable` corta el canal para que un objeto desactivado no siga sonando.

El listener sigue a la cámara activa. Por eso el mismo `Play` se oye distinto si el objeto está lejos de la cámara de juego. En Edit los scripts no llaman `Play`; entra en Play para oír el gameplay.

Si FMOD no está instalado en `ThirdParty/fmod/`, el motor no tiene sistema de audio. `SetupDeps.bat` no descarga FMOD: hace falta el instalador de FMOD Studio.
