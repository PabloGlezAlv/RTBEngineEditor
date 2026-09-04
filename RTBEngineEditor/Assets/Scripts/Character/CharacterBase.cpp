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
    if (PeekHealthSlot() || !owner) {
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

bool CharacterBase::HasSimulationAuthority() const
{
    if (!owner) {
        return true;
    }

    const RTBEngine::Scene::NetworkIdentity* identity = owner->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    // Online: only the lobby host runs movement/physics. Offline: always true.
    return identity->IsSimulatedByHost();
}

bool CharacterBase::IsLocallyControlled() const
{
    if (!owner) {
        return true;
    }

    const RTBEngine::Scene::NetworkIdentity* identity = owner->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    // True for the human player on this machine (camera + client input send).
    return identity->IsLocallyControlled();
}

bool CharacterBase::HasLocalGameplayAuthority() const
{
    // Legacy alias kept for existing call sites; means simulation authority.
    return HasSimulationAuthority();
}

bool CharacterBase::IsStunned() const
{
    if (!owner) {
        return false;
    }

    const StunReceiver* stun = owner->GetComponent<StunReceiver>();
    return stun && stun->IsStunned();
}
