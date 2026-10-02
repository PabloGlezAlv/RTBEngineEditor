# Audio

`RTBEngine::Scene::AudioSourceComponent`. `OnDisable` para el canal. Asigna el clip en el Inspector o con `SetClip` cuando ya tengas el puntero que cargó el motor.

Campos: `volume` 1, `pitch` 1, `loop` false, `playOnStart` false, `audioClip` null.

## SetClip

```cpp
void SetClip(Audio::AudioClip* clip);
```

Guarda el clip que usarán `Play` y `PlayOneShot()` sin argumentos. Null deja la fuente sin clip: `Play` vuelve sin sonar.

- `clip`: clip ya cargado, o null.

## GetClip

```cpp
Audio::AudioClip* GetClip() const;
```

**Devuelve** el clip guardado, o null si no hay.

## Play

```cpp
void Play();
```

Arranca el clip en el canal de esta fuente, con el volumen, el pitch y el loop actuales. Si no hay clip, el clip no está cargado, o FMOD no tiene sistema, no hace nada. Un segundo `Play` pide otro `playSound` y se queda con el canal nuevo.

## PlayOneShot

```cpp
void PlayOneShot();
```

Dispara el clip guardado en un canal aparte, sin loop y sin sustituir el canal de `Play` / `Stop`. Si no hay clip o no está cargado, no suena. Equivale a `PlayOneShot(GetClip())`.

## PlayOneShot

```cpp
void PlayOneShot(Audio::AudioClip* clipOverride);
```

Igual, con otro clip. Null o un clip sin cargar no suena. El volumen y el pitch salen de esta fuente. El modo es siempre sin loop, aunque `loop` esté a true.

- `clipOverride`: clip a disparar. Null no hace nada.

## Stop

```cpp
void Stop();
```

Para el canal de `Play` y lo suelta. Si no había canal, no hace nada. No corta un one-shot que ya salió por otro canal.

## Pause

```cpp
void Pause();
```

Pausa el canal de `Play`. Sin canal, no hace nada.

## Resume

```cpp
void Resume();
```

Quita la pausa del canal de `Play`. Sin canal, no hace nada.

## IsPlaying

```cpp
bool IsPlaying() const;
```

**Devuelve** true si el canal de `Play` existe y FMOD lo reporta sonando. Sin canal es false. Un one-shot no cuenta.

## SetVolume

```cpp
void SetVolume(float volume);
```

Volumen lineal de la fuente. `Play` y `PlayOneShot` lo aplican al arrancar. El default es 1.

- `volume`: 0 silencio, 1 el nivel del clip.

## GetVolume

```cpp
float GetVolume() const;
```

**Devuelve** el volumen guardado. El default es 1.

## SetPitch

```cpp
void SetPitch(float pitch);
```

Tono que se aplica al arrancar `Play` o un one-shot. El default es 1.

- `pitch`: 1 es el tono del clip.

## GetPitch

```cpp
float GetPitch() const;
```

**Devuelve** el pitch guardado. El default es 1.

## SetLoop

```cpp
void SetLoop(bool loop);
```

Si el próximo `Play` debe repetir. No cambia un one-shot: esos salen siempre sin loop. El default es false.

- `loop`: true para repetir en `Play`.

## IsLooping

```cpp
bool IsLooping() const;
```

**Devuelve** el flag de loop. El default es false.

## SetPlayOnStart

```cpp
void SetPlayOnStart(bool playOnStart);
```

Si `OnStart` debe llamar a `Play`. El default es false. Ponerlo a true después de que `OnStart` ya corrió no dispara el sonido: hay que llamar a `Play`.

- `playOnStart`: true para sonar al empezar.

## GetPlayOnStart

```cpp
bool GetPlayOnStart() const;
```

**Devuelve** el flag. El default es false.
