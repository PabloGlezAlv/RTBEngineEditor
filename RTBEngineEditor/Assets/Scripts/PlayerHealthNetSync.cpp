#include "PlayerHealthNetSync.h"

#include "HealthComponent.h"
#include "OnlineGameNetMessages.h"

#include <RTBEngine/Scene/GameObject.h>

using ThisClass = PlayerHealthNetSync;

RTB_REGISTER_COMPONENT(PlayerHealthNetSync)
RTB_END_REGISTER(PlayerHealthNetSync)

void PlayerHealthNetSync::OnStart()
{
    BindHealth();
}

void PlayerHealthNetSync::OnDestroy()
{
    UnbindHealth();
}

void PlayerHealthNetSync::BindHealth()
{
    UnbindHealth();

    if (!owner) {
        return;
    }

    health = owner->GetComponent<HealthComponent>();
    if (!health) {
        health = owner->GetComponentInChildren<HealthComponent>();
    }

    if (!health) {
        return;
    }

    healthChangedSubscription = health->SubscribeToHealthChanged(
        [this](const HealthComponent::HealthChangedEvent& eventData) {
            SyncHealth(eventData.normalizedHealth);
        });

    SyncHealth(health->GetHealthNormalized());
}

void PlayerHealthNetSync::UnbindHealth()
{
    healthChangedSubscription.Reset();
    health = nullptr;
}

void PlayerHealthNetSync::SyncHealth(float normalizedHealth) const
{
    if (!health) {
        return;
    }

    GameNet::OnlineGameNetSubsystem::TrySyncPlayerHealthFromComponent(health, normalizedHealth);
}
