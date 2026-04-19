#include "HealthComponent.h"

#include <algorithm>
#include <cmath>

using ThisClass = HealthComponent;

RTB_REGISTER_COMPONENT(HealthComponent)
    RTB_PROPERTY_RANGE(maxHealth, 1.0f, 1000.0f)
    RTB_PROPERTY(currentHealth)
RTB_END_REGISTER(HealthComponent)

HealthComponent::HealthChangedSubscription::HealthChangedSubscription(
    const std::shared_ptr<ListenerState>& listenerState,
    std::uint64_t id)
    : state(listenerState)
    , listenerId(id)
{
}

HealthComponent::HealthChangedSubscription::HealthChangedSubscription(HealthChangedSubscription&& other) noexcept
    : state(std::move(other.state))
    , listenerId(other.listenerId)
{
    other.listenerId = 0;
}

HealthComponent::HealthChangedSubscription& HealthComponent::HealthChangedSubscription::operator=(HealthChangedSubscription&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    Reset();

    state = std::move(other.state);
    listenerId = other.listenerId;
    other.listenerId = 0;

    return *this;
}

void HealthComponent::HealthChangedSubscription::Reset()
{
    if (listenerId == 0) {
        state.reset();
        return;
    }

    if (std::shared_ptr<ListenerState> lockedState = state.lock()) {
        auto& listeners = lockedState->listeners;
        listeners.erase(
            std::remove_if(
                listeners.begin(),
                listeners.end(),
                [this](const auto& listenerEntry) {
                    return listenerEntry.first == listenerId;
                }),
            listeners.end());
    }

    state.reset();
    listenerId = 0;
}

bool HealthComponent::HealthChangedSubscription::IsValid() const
{
    return listenerId != 0 && !state.expired();
}

void HealthComponent::OnStart()
{
    currentHealth = maxHealth;
    ClampHealth();
    NotifyHealthChanged(true);
}

void HealthComponent::OnValidate()
{
    ClampHealth();
    NotifyHealthChanged(false);
}

void HealthComponent::OnDestroy()
{
    if (listenerState) {
        listenerState->listeners.clear();
        listenerState.reset();
    }

    hasLastNotifiedEvent = false;
}

void HealthComponent::SetMaxHealth(float value)
{
    maxHealth = value;
    ClampHealth();
    NotifyHealthChanged(false);
}

void HealthComponent::SetCurrentHealth(float value)
{
    currentHealth = value;
    ClampHealth();
    NotifyHealthChanged(false);
}

void HealthComponent::Heal(float amount)
{
    if (amount <= 0.0f) {
        return;
    }

    currentHealth += amount;
    ClampHealth();
    NotifyHealthChanged(false);
}

void HealthComponent::TakeDamage(float amount)
{
    if (amount <= 0.0f || IsDead()) {
        return;
    }

    currentHealth -= amount;
    ClampHealth();
    NotifyHealthChanged(false);
}

HealthComponent::HealthChangedSubscription HealthComponent::SubscribeToHealthChanged(HealthChangedCallback callback)
{
    if (!callback) {
        return {};
    }

    if (!listenerState) {
        listenerState = std::make_shared<ListenerState>();
    }

    const std::uint64_t listenerId = listenerState->nextListenerId++;
    listenerState->listeners.emplace_back(listenerId, std::move(callback));

    return HealthChangedSubscription(listenerState, listenerId);
}

HealthComponent::HealthChangedEvent HealthComponent::GetHealthChangedEvent() const
{
    HealthChangedEvent eventData;
    eventData.currentHealth = currentHealth;
    eventData.maxHealth = maxHealth;
    eventData.normalizedHealth = GetHealthNormalized();
    return eventData;
}

float HealthComponent::GetHealthNormalized() const
{
    if (maxHealth <= 0.0f) {
        return 0.0f;
    }

    return std::clamp(currentHealth / maxHealth, 0.0f, 1.0f);
}

bool HealthComponent::IsDead() const
{
    return currentHealth <= 0.0f;
}

void HealthComponent::ClampHealth()
{
    maxHealth = std::max(1.0f, maxHealth);
    currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);
}

void HealthComponent::NotifyHealthChanged(bool forceNotify)
{
    if (!listenerState) {
        return;
    }

    if (!forceNotify && listenerState->listeners.empty()) {
        return;
    }

    const HealthChangedEvent eventData = GetHealthChangedEvent();
    static constexpr float kFloatEpsilon = 0.0001f;
    const bool stateChanged =
        !hasLastNotifiedEvent ||
        std::fabs(lastNotifiedEvent.currentHealth - eventData.currentHealth) > kFloatEpsilon ||
        std::fabs(lastNotifiedEvent.maxHealth - eventData.maxHealth) > kFloatEpsilon ||
        std::fabs(lastNotifiedEvent.normalizedHealth - eventData.normalizedHealth) > kFloatEpsilon;

    if (!forceNotify && !stateChanged) {
        return;
    }

    lastNotifiedEvent = eventData;
    hasLastNotifiedEvent = true;

    auto listeners = listenerState->listeners;
    for (const auto& listenerEntry : listeners) {
        listenerEntry.second(eventData);
    }
}
