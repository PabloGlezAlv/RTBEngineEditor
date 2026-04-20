#include "HealthComponent.h"

#include <algorithm>
#include <cmath>

using ThisClass = HealthComponent;

RTB_REGISTER_COMPONENT(HealthComponent)
    RTB_PROPERTY_RANGE(maxHealth, 1.0f, 1000.0f)
    RTB_PROPERTY(currentHealth)
RTB_END_REGISTER(HealthComponent)

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
    healthChangedEvent.Clear();
    damageTakenEvent.Clear();
    deathEvent.Clear();
    hasLastNotifiedEvent = false;
    hasLastDamageTakenEvent = false;
    hasLastDeathEvent = false;
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
    TakeDamage(amount, {});
}

void HealthComponent::TakeDamage(float amount, const DamageContext& context)
{
    if (amount <= 0.0f || IsDead()) {
        return;
    }

    const float previousHealth = currentHealth;
    currentHealth -= amount;
    ClampHealth();

    DamageTakenEvent damageEventData;
    damageEventData.previousHealth = previousHealth;
    damageEventData.currentHealth = currentHealth;
    damageEventData.maxHealth = maxHealth;
    damageEventData.normalizedHealth = GetHealthNormalized();
    damageEventData.damage = context;
    damageEventData.damage.amount = amount;

    lastDamageTakenEvent = damageEventData;
    hasLastDamageTakenEvent = true;
    damageTakenEvent.Invoke(damageEventData);

    if (currentHealth <= 0.0f) {
        DeathEvent deathEventData;
        deathEventData.previousHealth = previousHealth;
        deathEventData.currentHealth = currentHealth;
        deathEventData.maxHealth = maxHealth;
        deathEventData.damage = damageEventData.damage;

        lastDeathEvent = deathEventData;
        hasLastDeathEvent = true;
        deathEvent.Invoke(deathEventData);
    }

    NotifyHealthChanged(false);
}

RTBEngine::Core::EventSubscription HealthComponent::SubscribeToHealthChanged(HealthChangedCallback callback)
{
    return healthChangedEvent.Subscribe(std::move(callback));
}

RTBEngine::Core::EventSubscription HealthComponent::SubscribeToDamageTaken(DamageTakenCallback callback)
{
    return damageTakenEvent.Subscribe(std::move(callback));
}

RTBEngine::Core::EventSubscription HealthComponent::SubscribeToDeath(DeathCallback callback)
{
    return deathEvent.Subscribe(std::move(callback));
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
    if (!forceNotify && !healthChangedEvent.HasListeners()) {
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
    healthChangedEvent.Invoke(eventData);
}
