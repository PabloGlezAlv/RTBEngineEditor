#include "PlayerSpecialAttackCharge.h"

#include "PlayerSpecialBeamAttack.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIImage.h>
#include <RTBEngine/UI/Elements/UIJoystick.h>

#include <algorithm>
#include <cmath>
#include <string>

using ThisClass = PlayerSpecialAttackCharge;

namespace {
    constexpr float kMinChargeAlpha = 0.08f;
    constexpr float kDirectionEpsilon = 0.0001f;

    RTBEngine::Scene::GameObject* FindChildByName(RTBEngine::Scene::GameObject* root, const std::string& name)
    {
        if (!root) {
            return nullptr;
        }

        for (RTBEngine::Scene::GameObject* child : root->GetChildren()) {
            if (!child) {
                continue;
            }

            if (child->GetName() == name) {
                return child;
            }

            if (RTBEngine::Scene::GameObject* nested = FindChildByName(child, name)) {
                return nested;
            }
        }

        return nullptr;
    }

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }
}

RTB_REGISTER_COMPONENT(PlayerSpecialAttackCharge)
    RTB_PROPERTY_COMPONENT(specialAttackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(readyIcon, UIImage)
    RTB_PROPERTY_RANGE(hitsToFullyCharge, 1, 50)
RTB_END_REGISTER(PlayerSpecialAttackCharge)

PlayerSpecialAttackCharge::~PlayerSpecialAttackCharge() = default;

void PlayerSpecialAttackCharge::ClampSettings()
{
    hitsToFullyCharge = std::max(1, hitsToFullyCharge);
    currentHits = std::clamp(currentHits, 0, hitsToFullyCharge);
}

bool PlayerSpecialAttackCharge::IsLocalPlayer() const
{
    if (!owner) {
        return false;
    }

    const RTBEngine::Scene::NetworkIdentity* identity =
        owner->GetComponent<RTBEngine::Scene::NetworkIdentity>();
    if (!identity) {
        return true;
    }

    return identity->IsLocallyControlled();
}

bool PlayerSpecialAttackCharge::IsReady() const
{
    return currentHits >= std::max(1, hitsToFullyCharge);
}

float PlayerSpecialAttackCharge::GetChargeNormalized() const
{
    const int required = std::max(1, hitsToFullyCharge);
    return std::clamp(static_cast<float>(currentHits) / static_cast<float>(required), 0.0f, 1.0f);
}

RTBEngine::UI::UIImage* PlayerSpecialAttackCharge::GetBackgroundImage() const
{
    if (!specialAttackJoystick) {
        return nullptr;
    }

    RTBEngine::Scene::GameObject* joystickOwner = specialAttackJoystick->GetOwner();
    if (!joystickOwner) {
        return nullptr;
    }

    return joystickOwner->GetComponent<RTBEngine::UI::UIImage>();
}

RTBEngine::UI::UIImage* PlayerSpecialAttackCharge::GetHandleImage() const
{
    if (!specialAttackJoystick) {
        return nullptr;
    }

    return specialAttackJoystick->handleImage;
}

void PlayerSpecialAttackCharge::ResolveImagesFromJoystick()
{
    if (!specialAttackJoystick) {
        return;
    }

    RTBEngine::Scene::GameObject* joystickOwner = specialAttackJoystick->GetOwner();
    if (!joystickOwner) {
        return;
    }

    if (!readyIcon) {
        if (RTBEngine::Scene::GameObject* iconObject =
                FindChildByName(joystickOwner, "SpecialAttackIcon")) {
            readyIcon = iconObject->GetComponent<RTBEngine::UI::UIImage>();
        }
    }
}

void PlayerSpecialAttackCharge::EnsureReferences()
{
    if (!IsLocalPlayer()) {
        specialAttackJoystick = nullptr;
        readyIcon = nullptr;
        return;
    }

    ResolveImagesFromJoystick();
}

void PlayerSpecialAttackCharge::ApplyVisuals(bool forceReset)
{
    if (!forceReset && !IsLocalPlayer()) {
        return;
    }

    EnsureReferences();
    if (!specialAttackJoystick && !forceReset) {
        return;
    }

    const bool ready = !forceReset && IsReady();
    const float normalized = forceReset ? 0.0f : GetChargeNormalized();
    const float alpha = forceReset ? kMinChargeAlpha : std::max(kMinChargeAlpha, normalized);

    PlayerSpecialBeamAttack* beamAttack =
        owner ? owner->GetComponent<PlayerSpecialBeamAttack>() : nullptr;
    const bool beamActive = beamAttack && beamAttack->IsActive();
    const bool canInteract = ready && !beamActive;

    if (RTBEngine::UI::UIImage* background = GetBackgroundImage()) {
        const RTBEngine::Math::Vector4 tint = background->GetTint();
        background->SetTint(RTBEngine::Math::Vector4(tint.x, tint.y, tint.z, alpha));
        background->SetRaycastTarget(canInteract);
    }

    if (RTBEngine::UI::UIImage* handle = GetHandleImage()) {
        const RTBEngine::Math::Vector4 tint = handle->GetTint();
        handle->SetTint(RTBEngine::Math::Vector4(tint.x, tint.y, tint.z, alpha));
    }

    if (specialAttackJoystick) {
        specialAttackJoystick->interactable = canInteract;
    }

    if (readyIcon) {
        readyIcon->SetVisible(ready && !beamActive);
        if (ready) {
            const RTBEngine::Math::Vector4 tint = readyIcon->GetTint();
            readyIcon->SetTint(RTBEngine::Math::Vector4(tint.x, tint.y, tint.z, 1.0f));
        }
    }
}

void PlayerSpecialAttackCharge::ResetSceneJoystickVisuals()
{
    if (!specialAttackJoystick) {
        return;
    }

    RTBEngine::Scene::GameObject* joystickOwner = specialAttackJoystick->GetOwner();
    if (!joystickOwner || joystickOwner->IsBeingDestroyed()) {
        return;
    }

    ApplyVisuals(true);
}

void PlayerSpecialAttackCharge::RegisterSuccessfulHit()
{
    if (!IsLocalPlayer()) {
        return;
    }

    ClampSettings();
    if (currentHits >= hitsToFullyCharge) {
        return;
    }

    ++currentHits;
    currentHits = std::min(currentHits, hitsToFullyCharge);
    ApplyVisuals(false);
}

bool PlayerSpecialAttackCharge::ConsumeCharge()
{
    if (!IsReady()) {
        return false;
    }

    currentHits = 0;
    ApplyVisuals(false);
    return true;
}

void PlayerSpecialAttackCharge::UnsubscribeFromSpecialJoystick()
{
    specialJoystickReleaseSubscription.Reset();
    subscribedSpecialJoystick = nullptr;
}

void PlayerSpecialAttackCharge::RebindSpecialJoystickSubscription()
{
    if (subscribedSpecialJoystick == specialAttackJoystick &&
        specialJoystickReleaseSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromSpecialJoystick();

    if (!specialAttackJoystick || !IsLocalPlayer()) {
        return;
    }

    subscribedSpecialJoystick = specialAttackJoystick;
    specialJoystickReleaseSubscription = specialAttackJoystick->SubscribeToReleased(
        [this](const RTBEngine::Math::Vector2& joystickValue) {
            HandleSpecialJoystickReleased(joystickValue);
        });
}

void PlayerSpecialAttackCharge::HandleSpecialJoystickReleased(
    const RTBEngine::Math::Vector2& joystickValue)
{
    if (!IsLocalPlayer() || !owner) {
        return;
    }

    auto* beamAttack = owner->GetComponent<PlayerSpecialBeamAttack>();
    if (beamAttack) {
        beamAttack->HideAimPreview();
    }

    if (!IsReady()) {
        return;
    }

    if (!beamAttack || beamAttack->IsActive()) {
        return;
    }

    auto* controller = owner->GetComponent<ThirdPersonCharacterController>();
    if (!controller) {
        return;
    }

    const RTBEngine::Math::Vector3 attackDirection =
        controller->GetPlanarAttackDirectionFromJoystick(joystickValue);
    if (!HasPlanarDirection(attackDirection)) {
        return;
    }

    if (!beamAttack->TryActivate(attackDirection)) {
        return;
    }

    ConsumeCharge();
}

bool PlayerSpecialAttackCharge::TryGetSpecialAimDirection(
    RTBEngine::Math::Vector3& outAimDirection) const
{
    if (!IsLocalPlayer() || !owner || !specialAttackJoystick || !IsReady()) {
        return false;
    }

    if (!specialAttackJoystick->IsDragging()) {
        return false;
    }

    const auto* beamAttack = owner->GetComponent<PlayerSpecialBeamAttack>();
    if (!beamAttack || beamAttack->IsActive()) {
        return false;
    }

    const auto* controller = owner->GetComponent<ThirdPersonCharacterController>();
    if (!controller) {
        return false;
    }

    outAimDirection = controller->GetPlanarAttackDirectionFromJoystick(
        specialAttackJoystick->GetValue());
    return HasPlanarDirection(outAimDirection);
}

void PlayerSpecialAttackCharge::OnLateUpdate(float /*deltaTime*/)
{
    if (!IsLocalPlayer() || !owner) {
        return;
    }

    auto* beamAttack = owner->GetComponent<PlayerSpecialBeamAttack>();
    if (!beamAttack) {
        return;
    }

    RTBEngine::Math::Vector3 aimDirection = RTBEngine::Math::Vector3::Zero();
    if (!TryGetSpecialAimDirection(aimDirection)) {
        beamAttack->HideAimPreview();
        return;
    }

    beamAttack->UpdateAimPreview(aimDirection);
}

void PlayerSpecialAttackCharge::RefreshAfterSpawn()
{
    ClampSettings();
    EnsureReferences();
    if (!IsLocalPlayer()) {
        UnsubscribeFromSpecialJoystick();
        return;
    }

    RebindSpecialJoystickSubscription();
    SetUpdateTickEnabled(true);
    ApplyVisuals(false);
}

void PlayerSpecialAttackCharge::OnStart()
{
    ClampSettings();
    if (!IsLocalPlayer()) {
        specialAttackJoystick = nullptr;
        readyIcon = nullptr;
        SetUpdateTickEnabled(false);
        return;
    }

    EnsureReferences();
    RebindSpecialJoystickSubscription();
    SetUpdateTickEnabled(true);
    ApplyVisuals(false);
}

void PlayerSpecialAttackCharge::OnValidate()
{
    ClampSettings();
    if (IsLocalPlayer()) {
        EnsureReferences();
        ApplyVisuals(false);
    }
}

void PlayerSpecialAttackCharge::OnDestroy()
{
    UnsubscribeFromSpecialJoystick();

    if (!RTBEngine::Scene::SceneManager::GetInstance().IsSceneUnloading()) {
        ResetSceneJoystickVisuals();
    }

    specialAttackJoystick = nullptr;
    readyIcon = nullptr;
}
