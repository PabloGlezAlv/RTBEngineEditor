#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

class HealthComponent : public RTBEngine::ECS::Component
{
public:
    struct DamageContext {
        float amount = 0.0f;
        RTBEngine::ECS::GameObject* instigator = nullptr;
        RTBEngine::Math::Vector3 hitPoint = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 hitDirection = RTBEngine::Math::Vector3::Zero();
    };

    struct HealthChangedEvent {
        float currentHealth = 0.0f;
        float maxHealth = 0.0f;
        float normalizedHealth = 0.0f;
    };

    struct DamageTakenEvent {
        float previousHealth = 0.0f;
        float currentHealth = 0.0f;
        float maxHealth = 0.0f;
        float normalizedHealth = 0.0f;
        DamageContext damage;
    };

    struct DeathEvent {
        float previousHealth = 0.0f;
        float currentHealth = 0.0f;
        float maxHealth = 0.0f;
        DamageContext damage;
    };

    using HealthChangedCallback = RTBEngine::Core::Event<HealthChangedEvent>::Callback;
    using DamageTakenCallback = RTBEngine::Core::Event<DamageTakenEvent>::Callback;
    using DeathCallback = RTBEngine::Core::Event<DeathEvent>::Callback;

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
    void TakeDamage(float amount, const DamageContext& context);

    RTBEngine::Core::EventSubscription SubscribeToHealthChanged(HealthChangedCallback callback);
    RTBEngine::Core::EventSubscription SubscribeToDamageTaken(DamageTakenCallback callback);
    RTBEngine::Core::EventSubscription SubscribeToDeath(DeathCallback callback);
    HealthChangedEvent GetHealthChangedEvent() const;
    DamageTakenEvent GetLastDamageTakenEvent() const { return lastDamageTakenEvent; }
    DeathEvent GetLastDeathEvent() const { return lastDeathEvent; }

    float GetHealthNormalized() const;
    bool IsDead() const;

private:
    RTBEngine::Core::Event<HealthChangedEvent> healthChangedEvent;
    RTBEngine::Core::Event<DamageTakenEvent> damageTakenEvent;
    RTBEngine::Core::Event<DeathEvent> deathEvent;
    HealthChangedEvent lastNotifiedEvent{};
    bool hasLastNotifiedEvent = false;
    DamageTakenEvent lastDamageTakenEvent{};
    DeathEvent lastDeathEvent{};
    bool hasLastDamageTakenEvent = false;
    bool hasLastDeathEvent = false;

    void ClampHealth();
    void NotifyHealthChanged(bool forceNotify);
};
