#include "PlayerAmmoSystem.h"

#include "CharacterDefinition.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/UI/Elements/UISlider.h>

#include <algorithm>
#include <cmath>
#include <functional>

using ThisClass = PlayerAmmoSystem;

namespace {
    constexpr float kAmmoEpsilon = 0.001f;

    bool IsDescendantOf(RTBEngine::ECS::GameObject* node, RTBEngine::ECS::GameObject* root)
    {
        for (RTBEngine::ECS::GameObject* current = node; current; current = current->GetParent()) {
            if (current == root) {
                return true;
            }
        }

        return false;
    }
}

RTB_REGISTER_COMPONENT(PlayerAmmoSystem)
    RTB_PROPERTY_COMPONENT(ammoSlider, UISlider)
    RTB_PROPERTY_COMPONENT(ammoFillPanel, UIPanel)
    RTB_PROPERTY_RANGE(maxShots, 1, 6)
    RTB_PROPERTY_RANGE(fullReloadDuration, 0.0f, 120.0f)
    RTB_PROPERTY_COLOR(fullAmmoColor)
    RTB_PROPERTY_COLOR(partialAmmoColor)
RTB_END_REGISTER(PlayerAmmoSystem)

void PlayerAmmoSystem::ApplyCharacterStats(const CharacterDefinition& definition)
{
    maxShots = definition.maxShots;
    fullReloadDuration = definition.fullReloadDuration;
}

float PlayerAmmoSystem::GetShotCost() const
{
    return 1.0f / static_cast<float>(std::max(1, maxShots));
}

float PlayerAmmoSystem::GetEffectiveReloadDuration() const
{
    if (fullReloadDuration > 0.0f) {
        return fullReloadDuration;
    }

    return 10.0f;
}

void PlayerAmmoSystem::EnsureReferences()
{
    if (!owner) {
        ammoSlider = nullptr;
        ammoFillPanel = nullptr;
        return;
    }

    if (ammoSlider) {
        RTBEngine::ECS::GameObject* sliderOwner = ammoSlider->GetOwner();
        if (!sliderOwner || !IsDescendantOf(sliderOwner, owner)) {
            ammoSlider = nullptr;
            ammoFillPanel = nullptr;
        }
    }

    if (ammoFillPanel) {
        RTBEngine::ECS::GameObject* fillOwner = ammoFillPanel->GetOwner();
        if (!fillOwner || !IsDescendantOf(fillOwner, owner)) {
            ammoFillPanel = nullptr;
        }
    }

    if (ammoSlider) {
        return;
    }

    std::function<void(RTBEngine::ECS::GameObject*)> visit =
        [&](RTBEngine::ECS::GameObject* gameObject) {
            if (!gameObject || ammoSlider) {
                return;
            }

            if (gameObject->GetName() == "NameplateAmmoTrack") {
                if (auto* slider = gameObject->GetComponent<RTBEngine::UI::UISlider>()) {
                    ammoSlider = slider;
                    return;
                }
            }

            for (RTBEngine::ECS::GameObject* child : gameObject->GetChildren()) {
                visit(child);
            }
        };

    visit(owner);

    if (!ammoFillPanel && ammoSlider && ammoSlider->fillPanel) {
        RTBEngine::ECS::GameObject* fillOwner = ammoSlider->fillPanel->GetOwner();
        if (fillOwner && IsDescendantOf(fillOwner, owner)) {
            ammoFillPanel = ammoSlider->fillPanel;
        }
    }
}

void PlayerAmmoSystem::OnStart()
{
    maxShots = std::max(1, maxShots);
    fullReloadDuration = std::max(0.0f, fullReloadDuration);
    SetUpdateTickEnabled(true);
    EnsureReferences();

    if (IsLocalPlayer()) {
        normalizedAmmo = 1.0f;
    }

    RefreshNetworkState();
}

void PlayerAmmoSystem::OnLateUpdate(float deltaTime)
{
    EnsureReferences();

    const bool localPlayer = IsLocalPlayer();
    SetBarVisible(localPlayer);

    if (!localPlayer) {
        return;
    }

    RechargeAmmo(deltaTime);
}

void PlayerAmmoSystem::RefreshNetworkState()
{
    EnsureReferences();
    SetBarVisible(IsLocalPlayer());

    if (IsLocalPlayer()) {
        UpdateVisuals();
    }
}

void PlayerAmmoSystem::RechargeAmmo(float deltaTime)
{
    if (normalizedAmmo >= 1.0f - kAmmoEpsilon) {
        if (normalizedAmmo < 1.0f) {
            normalizedAmmo = 1.0f;
            UpdateVisuals();
        }
        return;
    }

    const float reloadRate = 1.0f / GetEffectiveReloadDuration();
    normalizedAmmo = std::min(1.0f, normalizedAmmo + std::max(0.0f, deltaTime) * reloadRate);
    UpdateVisuals();
}

void PlayerAmmoSystem::OnValidate()
{
    maxShots = std::max(1, maxShots);
    fullReloadDuration = std::max(0.0f, fullReloadDuration);
    normalizedAmmo = std::clamp(normalizedAmmo, 0.0f, 1.0f);
    EnsureReferences();
    UpdateVisuals();
}

bool PlayerAmmoSystem::IsLocalPlayer() const
{
    if (!owner) {
        return false;
    }

    const RTBEngine::ECS::NetworkIdentity* identity = owner->GetComponent<RTBEngine::ECS::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    return identity->IsLocallyControlled();
}

void PlayerAmmoSystem::SetBarVisible(bool visible)
{
    EnsureReferences();

    if (!ammoSlider) {
        return;
    }

    RTBEngine::ECS::GameObject* trackObject = ammoSlider->GetOwner();
    if (!trackObject || !owner || !IsDescendantOf(trackObject, owner)) {
        return;
    }

    trackObject->SetActive(visible);
}

bool PlayerAmmoSystem::CanFire() const
{
    return normalizedAmmo + kAmmoEpsilon >= GetShotCost();
}

void PlayerAmmoSystem::ConsumeShot()
{
    if (!IsLocalPlayer()) {
        return;
    }

    normalizedAmmo = std::max(0.0f, normalizedAmmo - GetShotCost());
    UpdateVisuals();
}

void PlayerAmmoSystem::RefillAmmo()
{
    normalizedAmmo = 1.0f;
    UpdateVisuals();
}

RTBEngine::Math::Vector4 PlayerAmmoSystem::EvaluateFillColor() const
{
    if (normalizedAmmo + kAmmoEpsilon >= GetShotCost()) {
        return fullAmmoColor;
    }

    return partialAmmoColor;
}

void PlayerAmmoSystem::UpdateVisuals()
{
    if (!IsLocalPlayer()) {
        return;
    }

    EnsureReferences();

    if (!ammoSlider) {
        return;
    }

    ammoSlider->SetNormalizedValue(normalizedAmmo);

    RTBEngine::UI::UIPanel* fillPanel = ammoFillPanel;
    if (!fillPanel && ammoSlider->fillPanel) {
        fillPanel = ammoSlider->fillPanel;
    }

    if (fillPanel) {
        fillPanel->SetBackgroundColor(EvaluateFillColor());
    }
}
