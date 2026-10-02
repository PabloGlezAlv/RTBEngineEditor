# ParticleSystem

`RTBEngine::Scene::ParticleSystem`. Fields: `maxParticles` 256, `emissionRate` 40, `playOnAwake` true, `simulateInEditMode` true, `burstCount` 10.

With `simulateInEditMode`, the Scene View advances the effect without entering Play.

## Play

```cpp
void Play();
```

Resumes emission without clearing particles that are already alive. If the system was stopped with `Stop`, it emits again. It does not reset counters: to empty and start over, `Stop` and then `Play`, or the component's internal `Restart`.

## Stop

```cpp
void Stop();
```

Cuts emission, clears pause, and empties the particle pool. It marks the system as stopped by the user, so `playOnAwake` does not start it again until the next validate or the next load.

## Pause

```cpp
void Pause();
```

Freezes the simulation and leaves the visible particles where they are. If it was not playing, it does nothing: it does not pause a system that is already stopped.

## Restart

```cpp
void Restart();
```

Empties the pool and starts playback from scratch, unlike `Play`, which keeps particles that are already alive. `playOnAwake` can start the system again.

## Emit

```cpp
void Emit(int count);
```

Spawns `count` particles at once, on top of the continuous rate, and forces playback: `playing` becomes true and pause is cleared. A `count` less than or equal to 0 does nothing. The cap is still `maxParticles`; particles that do not fit are not born.

- `count`: particles to create now.
