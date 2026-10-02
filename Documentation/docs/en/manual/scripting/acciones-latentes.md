# Latent actions

`Component` can ask the engine scheduler for deferred work. It cancels itself in `OnDestroy`. It respects `ComponentTimeMode`: `Scaled` follows time scale, `Unscaled` does not.

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

## Example

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

The captured `this` is valid for as long as the component lives. If you deactivate the object, do not destroy the handle by hand unless you want to cut the callback short: when the component is destroyed the engine already cancels it.

Do not use `Invoke` for logic that must stay locked to the physics step. That is `OnFixedUpdate`. `Invoke` is for gameplay waits: a cooldown, a line of dialogue, a door that rises a second later.

## Time

If the component is in `ComponentTimeMode::Unscaled`, the delay counts real time. That is useful for pause UI. The default mode is `Scaled`.

```cpp
SetTimeMode(RTBEngine::Scene::ComponentTimeMode::Unscaled);
```
