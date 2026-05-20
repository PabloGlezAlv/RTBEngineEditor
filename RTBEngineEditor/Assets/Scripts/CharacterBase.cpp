#include "CharacterBase.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Online/OnlineSystem.h>

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

bool CharacterBase::HasLocalGameplayAuthority() const
{
    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    RTBEngine::Online::IOnlineLobby* lobby = online.GetLobby();
    if (!online.IsInitialized() || !lobby || lobby->GetCurrentLobby().lobbyId.empty()) {
        return true;
    }

    return lobby->GetCurrentLobby().isOwner;
}
