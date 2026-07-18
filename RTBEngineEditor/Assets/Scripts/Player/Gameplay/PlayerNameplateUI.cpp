#include "PlayerNameplateUI.h"

#include "HealthBarUI.h"
#include "OnlineDisplayNameHelper.h"

#include "HealthComponent.h"

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/UI/Canvas.h>

using ThisClass = PlayerNameplateUI;

RTB_REGISTER_COMPONENT(PlayerNameplateUI)
    RTB_PROPERTY_COMPONENT(displayNameText, UIText)
    RTB_PROPERTY_COMPONENT(healthBarUI, HealthBarUI)
RTB_END_REGISTER(PlayerNameplateUI)

namespace {

    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kFixedNameplatePitchDegrees = 50.0f;
    constexpr float kFixedNameplateYawDegrees = 0.0f;

    RTBEngine::Scene::GameObject* FindPlayerRoot(RTBEngine::Scene::GameObject* from)
    {
        for (RTBEngine::Scene::GameObject* current = from; current; current = current->GetParent()) {
            if (current->GetComponent<RTBEngine::Scene::NetworkIdentity>()) {
                return current;
            }
        }

        return nullptr;
    }

    RTBEngine::Scene::GameObject* FindNameplateRoot(RTBEngine::Scene::GameObject* from)
    {
        RTBEngine::Scene::GameObject* playerRoot = FindPlayerRoot(from);
        if (!playerRoot || !from) {
            return nullptr;
        }

        for (RTBEngine::Scene::GameObject* current = from; current; current = current->GetParent()) {
            if (current->GetParent() == playerRoot) {
                return current;
            }
        }

        return nullptr;
    }

    RTBEngine::Math::Quaternion GetFixedNameplateWorldRotation()
    {
        const RTBEngine::Math::Quaternion cameraFacingRotation =
            RTBEngine::Math::Quaternion::FromEulerAngles(
                kFixedNameplatePitchDegrees * kDegToRad,
                kFixedNameplateYawDegrees * kDegToRad,
                0.0f);
        const RTBEngine::Math::Quaternion readableFlip =
            RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, kPi, 0.0f);
        return cameraFacingRotation * readableFlip;
    }

    void DisableCanvasBillboard(RTBEngine::UI::Canvas* canvas)
    {
        if (!canvas) {
            return;
        }

        if (canvas->GetFaceCamera()) {
            canvas->SetFaceCamera(false);
            canvas->SetFaceCameraLockY(false);
            canvas->MarkHierarchyDirty();
        }
    }

}

void PlayerNameplateUI::OnStart()
{
    DisableCanvasBillboard(owner ? owner->GetComponent<RTBEngine::UI::Canvas>() : nullptr);
    ApplyFixedWorldOrientation();

    RefreshDisplayName();
    BindHealthBar();

    profileChangedSubscription =
        RTBEngine::Online::OnlineSystem::GetInstance().SubscribeToPlayerSessionProfileChanged(
            [this](const RTBEngine::Online::PlayerSessionProfileChangedEvent& eventData) {
                const int ownerSlot = ResolveOwnerPlayerSlot();
                if (ownerSlot < 0 || eventData.playerSlot != ownerSlot) {
                    return;
                }

                RefreshDisplayName();
            });
}

void PlayerNameplateUI::OnLateUpdate(float /*deltaTime*/)
{
    ApplyFixedWorldOrientation();
}

void PlayerNameplateUI::OnDestroy()
{
    profileChangedSubscription.Reset();
}

void PlayerNameplateUI::OnValidate()
{
    DisableCanvasBillboard(owner ? owner->GetComponent<RTBEngine::UI::Canvas>() : nullptr);
    ApplyFixedWorldOrientation();
    RefreshDisplayName();
    BindHealthBar();
}

void PlayerNameplateUI::ForceRefreshDisplayName()
{
    RefreshDisplayName();
    BindHealthBar();
}

void PlayerNameplateUI::ApplyFixedWorldOrientation() const
{
    RTBEngine::Scene::GameObject* playerRoot = FindPlayerRoot(owner);
    RTBEngine::Scene::GameObject* nameplateRoot = FindNameplateRoot(owner);
    if (!playerRoot || !nameplateRoot) {
        return;
    }

    const RTBEngine::Math::Quaternion fixedWorld = GetFixedNameplateWorldRotation();
    nameplateRoot->GetTransform().SetRotation(
        playerRoot->GetWorldRotation().Inverse() * fixedWorld);
}

int PlayerNameplateUI::ResolveOwnerPlayerSlot() const
{
    RTBEngine::Scene::GameObject* playerRoot = FindPlayerRoot(owner);
    if (!playerRoot) {
        return -1;
    }

    if (const RTBEngine::Scene::NetworkIdentity* identity =
            playerRoot->GetComponent<RTBEngine::Scene::NetworkIdentity>()) {
        return identity->networkPlayerSlot;
    }

    return -1;
}

void PlayerNameplateUI::RefreshDisplayName() const
{
    if (!displayNameText) {
        return;
    }

    RTBEngine::Scene::GameObject* playerRoot = FindPlayerRoot(owner);
    if (!playerRoot) {
        displayNameText->SetText(GameNet::ResolveLocalDisplayName());
        return;
    }

    const RTBEngine::Scene::NetworkIdentity* identity =
        playerRoot->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    const std::string resolvedName = GameNet::ResolvePlayerDisplayName(identity);
    displayNameText->SetText(resolvedName);
}

void PlayerNameplateUI::BindHealthBar()
{
    if (!healthBarUI) {
        return;
    }

    RTBEngine::Scene::GameObject* playerRoot = FindPlayerRoot(owner);
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
