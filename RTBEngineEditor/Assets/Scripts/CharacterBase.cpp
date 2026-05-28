#include "CharacterBase.h"

#include "NetworkIdentity.h"

#include <RTBEngine/ECS/GameObject.h>

bool CharacterBase::IsCharacterDead() const
{
    HealthComponent* health = PeekHealthSlot();
    return health && health->IsDead();
}

void CharacterBase::ResolveCharacterHealth()
{
    HealthComponent*& health = AccessHealthSlot();
    if (health || !owner) {
        return;
    }

    health = owner->GetComponent<HealthComponent>();
}

void CharacterBase::RebindCharacterDeathSubscription()
{
    HealthComponent* health = PeekHealthSlot();
    if (subscribedCharacterHealth == health && characterDeathSubscription.IsValid()) {
        return;
    }

    UnsubscribeCharacterDeath();
    if (!health) {
        return;
    }

    subscribedCharacterHealth = health;
    characterDeathSubscription = health->SubscribeToDeath(
        [this](const HealthComponent::DeathEvent& eventData) {
            HandleCharacterDeath(eventData);
        });
}

void CharacterBase::UnsubscribeCharacterDeath()
{
    characterDeathSubscription.Reset();
    subscribedCharacterHealth = nullptr;
}

bool CharacterBase::HasSimulationAuthority() const
{
    if (!owner) {
        return true;
    }

    const NetworkIdentity* identity = owner->GetComponent<NetworkIdentity>();
    if (!identity) {
        return true;
    }

    return identity->IsSimulatedByHost();
}

bool CharacterBase::IsLocallyControlled() const
{
    if (!owner) {
        return true;
    }

    const NetworkIdentity* identity = owner->GetComponent<NetworkIdentity>();
    if (!identity) {
        return true;
    }

    return identity->IsLocallyControlled();
}

bool CharacterBase::HasLocalGameplayAuthority() const
{
    return HasSimulationAuthority();
}
