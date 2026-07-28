#include "PlayerAmmoSystem.h"

#include "CharacterDefinition.h"
#include "CombatAuthority.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/UI/Elements/UISlider.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerAmmoSystem;

namespace {
    constexpr float kAmmoEpsilon = 0.001f;

    bool IsDescendantOf(RTBEngine::Scene::GameObject* node, RTBEngine::Scene::GameObject* root)
    {
        for (RTBEngine::Scene::GameObject* current = node; current; current = current->GetParent()) {
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
    RTB_PROPERTY_SERIALIZED_RANGE(maxShots, 1, 6)
    RTB_PROPERTY_SERIALIZED_RANGE(fullReloadDuration, 0.0f, 120.0f)
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
        RTBEngine::Scene::GameObject* sliderOwner = ammoSlider->GetOwner();
        if (!sliderOwner || !IsDescendantOf(sliderOwner, owner)) {
            ammoSlider = nullptr;
        }
    }

    if (ammoFillPanel) {
        RTBEngine::Scene::GameObject* fillOwner = ammoFillPanel->GetOwner();
        if (!fillOwner || !IsDescendantOf(fillOwner, owner)) {
            ammoFillPanel = nullptr;
        }
    }

    if (!ammoFillPanel && ammoSlider && ammoSlider->fillPanel) {
        RTBEngine::Scene::GameObject* fillOwner = ammoSlider->fillPanel->GetOwner();
        if (fillOwner && IsDescendantOf(fillOwner, owner)) {
            ammoFillPanel = ammoSlider->fillPanel;
        }
    }

    if (!ammoSlider) {
        RTB_WARN("[PlayerAmmoSystem] ammoSlider is not assigned on '" + owner->GetName() + "'.");
    }
}

void PlayerAmmoSystem::OnStart()
{
    maxShots = std::max(1, maxShots);
    fullReloadDuration = std::max(0.0f, fullReloadDuration);
    EnsureReferences();

    if (IsLocalPlayer()) {
        normalizedAmmo = 1.0f;
        SetUpdateTickEnabled(true);
    } else {
        SetUpdateTickEnabled(false);
    }

    RefreshNetworkState();
}

void PlayerAmmoSystem::OnLateUpdate(float deltaTime)
{
    if (!IsLocalPlayer()) {
        SetUpdateTickEnabled(false);
        return;
    }

    SetBarVisible(true);
    RechargeAmmo(deltaTime);
}

void PlayerAmmoSystem::RefreshNetworkState()
{
    EnsureReferences();
    const bool localPlayer = IsLocalPlayer();
    SetBarVisible(localPlayer);
    SetUpdateTickEnabled(localPlayer);

    if (localPlayer) {
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
    return CombatAuthority::IsLocallyControlled(owner);
}

void PlayerAmmoSystem::SetBarVisible(bool visible)
{
    if (!ammoSlider) {
        return;
    }

    RTBEngine::Scene::GameObject* trackObject = ammoSlider->GetOwner();
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

bool PlayerAmmoSystem::HasAmmoAvailable(RTBEngine::Scene::GameObject* instigator)
{
    if (!instigator || !CombatAuthority::CanConsumeAmmo(instigator)) {
        return true;
    }

    if (const auto* ammoSystem = instigator->GetComponent<PlayerAmmoSystem>()) {
        return ammoSystem->CanFire();
    }

    return true;
}

bool PlayerAmmoSystem::TryConsumeAttackAmmo(RTBEngine::Scene::GameObject* instigator)
{
    if (!instigator || !CombatAuthority::CanConsumeAmmo(instigator)) {
        return false;
    }

    auto* ammoSystem = instigator->GetComponent<PlayerAmmoSystem>();
    if (!ammoSystem) {
        return false;
    }

    if (!ammoSystem->CanFire()) {
        return false;
    }

    ammoSystem->ConsumeShot();
    return true;
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
