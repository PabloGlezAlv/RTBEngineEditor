#include "CharacterBase.h"
#include "StunReceiver.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/NetworkIdentity.h>

#include <RTBEngine/Scene/GameObject.h>

bool CharacterBase::IsCharacterDead() const
{
    HealthComponent* health = PeekHealthSlot();
    return health && health->IsDead();
}

void CharacterBase::ValidateCharacterHealth()
{
    if (PeekHealthSlot()) {
        return;
    }

    RTB_WARN("[CharacterBase] health is not assigned on '" + owner->GetName() + "'.");
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

void CharacterBase::CacheCharacterBaseReferences()
{
    cachedNetworkIdentity = owner->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    cachedStunReceiver = owner->GetComponent<StunReceiver>();
}

void CharacterBase::SetCachedStunReceiver(StunReceiver* receiver)
{
    cachedStunReceiver = receiver;
}

bool CharacterBase::HasSimulationAuthority() const
{
    if (!cachedNetworkIdentity) {
        return true;
    }

    // Online: only the lobby host runs movement/physics. Offline: always true.
    return cachedNetworkIdentity->IsSimulatedByHost();
}

bool CharacterBase::IsLocallyControlled() const
{
    if (!cachedNetworkIdentity) {
        return true;
    }

    // True for the human player on this machine (camera + client input send).
    return cachedNetworkIdentity->IsLocallyControlled();
}

bool CharacterBase::HasLocalGameplayAuthority() const
{
    // Legacy alias kept for existing call sites; means simulation authority.
    return HasSimulationAuthority();
}

bool CharacterBase::IsStunned() const
{
    return cachedStunReceiver && cachedStunReceiver->IsStunned();
}
