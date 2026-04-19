#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

class HealthComponent : public RTBEngine::ECS::Component
{
private:
    struct ListenerState;

public:
    struct HealthChangedEvent {
        float currentHealth = 0.0f;
        float maxHealth = 0.0f;
        float normalizedHealth = 0.0f;
    };

    class HealthChangedSubscription {
    public:
        HealthChangedSubscription() = default;
        HealthChangedSubscription(const HealthChangedSubscription&) = delete;
        HealthChangedSubscription& operator=(const HealthChangedSubscription&) = delete;

        HealthChangedSubscription(HealthChangedSubscription&& other) noexcept;
        HealthChangedSubscription& operator=(HealthChangedSubscription&& other) noexcept;

        ~HealthChangedSubscription() = default;

        void Reset();
        bool IsValid() const;

    private:
        std::weak_ptr<ListenerState> state;
        std::uint64_t listenerId = 0;

        HealthChangedSubscription(const std::shared_ptr<ListenerState>& listenerState, std::uint64_t id);

        friend class HealthComponent;
    };

    using HealthChangedCallback = std::function<void(const HealthChangedEvent&)>;

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

    HealthChangedSubscription SubscribeToHealthChanged(HealthChangedCallback callback);
    HealthChangedEvent GetHealthChangedEvent() const;

    float GetHealthNormalized() const;
    bool IsDead() const;

private:
    struct ListenerState {
        std::vector<std::pair<std::uint64_t, HealthChangedCallback>> listeners;
        std::uint64_t nextListenerId = 1;
    };

    std::shared_ptr<ListenerState> listenerState = std::make_shared<ListenerState>();
    HealthChangedEvent lastNotifiedEvent{};
    bool hasLastNotifiedEvent = false;

    void ClampHealth();
    void NotifyHealthChanged(bool forceNotify);
};
