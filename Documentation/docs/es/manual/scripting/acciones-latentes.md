# Acciones latentes

`Component` puede pedir trabajo diferido al scheduler del motor. Se cancela solo en `OnDestroy`. Respeta `ComponentTimeMode`: `Scaled` sigue la escala de tiempo, `Unscaled` no.

```cpp
Scripting::LatentActionHandle Invoke(float delaySeconds, std::function<void()> callback);

Scripting::LatentActionHandle InvokeRepeating(
    float initialDelaySeconds,
    float intervalSeconds,
    std::function<void()> callback);

Scripting::LatentActionHandle StartSequence(Scripting::LatentSequence sequence);

void CancelInvoke(Scripting::LatentActionHandle handle);
void CancelAllInvokes();
```

## Ejemplo

```cpp
void Door::OnStart() {
    handle = Invoke(1.5f, [this]() {
        GetOwner()->GetTransform().Translate(
            RTBEngine::Math::Vector3(0.0f, 2.0f, 0.0f));
    });
}

void Door::OnDestroy() {
    CancelAllInvokes();
}
```

El `this` capturado es válido mientras el componente viva. Si desactivas el objeto, no destruyas el handle a mano salvo que quieras cortar el callback: al destruir el componente el motor ya cancela.

No uses `Invoke` para lógica que debe ir clavada al paso de física. Eso es `OnFixedUpdate`. `Invoke` es para esperas de gameplay: un cooldown, un diálogo, una puerta que sube un segundo después.

## Tiempo

Si el componente está en `ComponentTimeMode::Unscaled`, el retraso cuenta tiempo real. Útil para UI de pausa. El modo por defecto es `Scaled`.

```cpp
SetTimeMode(RTBEngine::Scene::ComponentTimeMode::Unscaled);
```
