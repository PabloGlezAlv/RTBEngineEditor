#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class HealthComponent : public RTBEngine::ECS::Component
{
public:
    struct HealthChangedEvent {
        float currentHealth = 0.0f;
        float maxHealth = 0.0f;
        float normalizedHealth = 0.0f;
    };

    using HealthChangedCallback = RTBEngine::Core::Event<HealthChangedEvent>::Callback;

    HealthComponent() = default;
    ~HealthComponent() override = default;

    float maxHealth = 100.0f;
    float currentHealth = 100.0f;

    RTB_COMPONENT(HealthComponent)

public:
    void OnStart() override;
    void OnValidate() override;
    void OnDestroy() override;

    void SetMaxHealth(float value);
    void SetCurrentHealth(float value);
    void Heal(float amount);
    void TakeDamage(float amount);

    RTBEngine::Core::EventSubscription SubscribeToHealthChanged(HealthChangedCallback callback);
    HealthChangedEvent GetHealthChangedEvent() const;

    float GetHealthNormalized() const;
    bool IsDead() const;

private:
    RTBEngine::Core::Event<HealthChangedEvent> healthChangedEvent;
    HealthChangedEvent lastNotifiedEvent{};
    bool hasLastNotifiedEvent = false;

    void ClampHealth();
    void NotifyHealthChanged(bool forceNotify);
};
