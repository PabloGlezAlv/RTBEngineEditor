# ParticleSystem

`RTBEngine::Scene::ParticleSystem`. Campos: `maxParticles` 256, `emissionRate` 40, `playOnAwake` true, `simulateInEditMode` true, `burstCount` 10.

Con `simulateInEditMode`, la Scene View avanza el efecto sin entrar en Play.

## Play

```cpp
void Play();
```

Reanuda la emisión sin borrar las partículas que ya están vivas. Si el sistema estaba parado con `Stop`, vuelve a emitir. No reinicia contadores: para vaciar y empezar de cero está `Stop` y luego `Play`, o el `Restart` interno del componente.

## Stop

```cpp
void Stop();
```

Corta la emisión, quita la pausa y vacía el pool de partículas. Marca el sistema como parado por el usuario, así `playOnAwake` no lo rearranca hasta el próximo validate o la próxima carga.

## Pause

```cpp
void Pause();
```

Congela la simulación y deja las partículas visibles donde están. Si no estaba en Play, no hace nada: no pausa un sistema ya parado.

## Restart

```cpp
void Restart();
```

Vacía el pool y empieza la reproducción desde cero, a diferencia de `Play`, que conserva las partículas vivas. `playOnAwake` vuelve a poder arrancar el sistema.

## Emit

```cpp
void Emit(int count);
```

Suelta `count` partículas de golpe, además de la tasa continua, y fuerza la reproducción: `playing` pasa a true y la pausa se quita. `count` menor o igual que 0 no hace nada. El tope sigue siendo `maxParticles`; las que no quepan no nacen.

- `count`: partículas a crear ahora.
