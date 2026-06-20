#include "PlayerNameplateUI.h"

#include "HealthBarUI.h"
#include "OnlineDisplayNameHelper.h"

#include "HealthComponent.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Online/OnlineSystem.h>

using ThisClass = PlayerNameplateUI;

RTB_REGISTER_COMPONENT(PlayerNameplateUI)
    RTB_PROPERTY_COMPONENT(displayNameText, UIText)
    RTB_PROPERTY_COMPONENT(healthBarUI, HealthBarUI)
RTB_END_REGISTER(PlayerNameplateUI)

namespace {

    RTBEngine::ECS::GameObject* FindPlayerRoot(RTBEngine::ECS::GameObject* from)
    {
        for (RTBEngine::ECS::GameObject* current = from; current; current = current->GetParent()) {
            if (current->GetComponent<RTBEngine::ECS::NetworkIdentity>()) {
                return current;
            }
        }

        return nullptr;
    }

}

void PlayerNameplateUI::OnStart()
{
    refreshTimer = 0.0f;
    SetUpdateTickEnabled(true);
    RefreshDisplayName();
    BindHealthBar();

    if (ShouldStopRefreshing()) {
        SetUpdateTickEnabled(false);
    }
}

void PlayerNameplateUI::OnUpdate(float deltaTime)
{
    refreshTimer += deltaTime;
    if (refreshTimer < 0.35f) {
        return;
    }

    refreshTimer = 0.0f;
    RefreshDisplayName();
    BindHealthBar();

    if (ShouldStopRefreshing()) {
        SetUpdateTickEnabled(false);
    }
}

void PlayerNameplateUI::OnValidate()
{
    RefreshDisplayName();
    BindHealthBar();
}

void PlayerNameplateUI::RefreshDisplayName() const
{
    if (!displayNameText) {
        return;
    }

    RTBEngine::ECS::GameObject* playerRoot = FindPlayerRoot(owner);
    if (!playerRoot) {
        displayNameText->SetText(GameNet::ResolveLocalDisplayName());
        return;
    }

    displayNameText->SetText(
        GameNet::ResolvePlayerDisplayName(playerRoot->GetComponent<RTBEngine::ECS::NetworkIdentity>()));
}

void PlayerNameplateUI::BindHealthBar()
{
    if (!healthBarUI) {
        return;
    }

    RTBEngine::ECS::GameObject* playerRoot = FindPlayerRoot(owner);
    if (!playerRoot) {
        return;
    }

    HealthComponent* health = playerRoot->GetComponent<HealthComponent>();
    if (healthBarUI->health == health) {
        return;
    }

    healthBarUI->health = health;
    healthBarUI->RefreshBinding();
}

bool PlayerNameplateUI::ShouldStopRefreshing() const
{
    RTBEngine::ECS::GameObject* playerRoot = FindPlayerRoot(owner);
    if (!playerRoot) {
        return false;
    }

    const RTBEngine::ECS::NetworkIdentity* identity =
        playerRoot->GetComponent<RTBEngine::ECS::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    if (!online.IsInLobby()) {
        return true;
    }

    if (identity->networkPlayerSlot < 0) {
        return false;
    }

    if (identity->IsLocallyControlled()) {
        return true;
    }

    return online.HasPlayerSessionProfile(identity->networkPlayerSlot);
}
