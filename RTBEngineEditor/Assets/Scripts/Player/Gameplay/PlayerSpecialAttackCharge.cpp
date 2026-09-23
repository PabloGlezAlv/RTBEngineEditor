#include "PlayerSpecialAttackCharge.h"

#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "OnlineGameNetMessages.h"
#include "PlayerRegistry.h"
#include "PlayerSpecialAttackUtil.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/NetworkIdentity.h>

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIImage.h>
#include <RTBEngine/UI/Elements/UIJoystick.h>

#include <algorithm>
#include <cmath>
#include <string>

using ThisClass = PlayerSpecialAttackCharge;

namespace {
    constexpr float kMinChargeAlpha = 0.08f;
}

RTB_REGISTER_COMPONENT(PlayerSpecialAttackCharge)
    RTB_PROPERTY_COMPONENT(specialAttackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(readyIcon, UIImage)
    RTB_PROPERTY_SERIALIZED_RANGE(hitsToFullyCharge, 1, 50)
RTB_END_REGISTER(PlayerSpecialAttackCharge)

PlayerSpecialAttackCharge::~PlayerSpecialAttackCharge() = default;

void PlayerSpecialAttackCharge::SetSpecialAttackJoystick(RTBEngine::UI::UIJoystick* joystick)
{
    specialAttackJoystick = joystick;
}

void PlayerSpecialAttackCharge::SetReadyIcon(RTBEngine::UI::UIImage* icon)
{
    readyIcon = icon;
}

void PlayerSpecialAttackCharge::ClampSettings()
{
    hitsToFullyCharge = std::max(1, hitsToFullyCharge);
    currentHits = std::clamp(currentHits, 0, hitsToFullyCharge);
}

bool PlayerSpecialAttackCharge::IsLocalPlayer() const
{
    return CombatAuthority::IsLocallyControlled(owner);
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

void PlayerSpecialAttackCharge::CacheGameplayReferences()
{
    specialAttack = ResolvePlayerSpecialAttack(owner);
    controller = owner->GetComponent<ThirdPersonCharacterController>();
}

void PlayerSpecialAttackCharge::ApplyVisuals(bool forceReset)
{
    if (!forceReset && !IsLocalPlayer()) {
        return;
    }

    if (!specialAttackJoystick && !forceReset) {
        return;
    }

    const bool ready = !forceReset && IsReady();
    const float normalized = forceReset ? 0.0f : GetChargeNormalized();
    const float alpha = forceReset ? kMinChargeAlpha : std::max(kMinChargeAlpha, normalized);

    const bool specialActive = specialAttack && specialAttack->IsActive();
    const bool canInteract = ready && !specialActive;

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
        readyIcon->SetVisible(ready && !specialActive);
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

    RegisterAuthoritativeHit();
}

bool PlayerSpecialAttackCharge::RegisterAuthoritativeHit()
{
    ClampSettings();
    if (currentHits >= hitsToFullyCharge) {
        return false;
    }

    ++currentHits;
    currentHits = std::min(currentHits, hitsToFullyCharge);
    if (IsLocalPlayer()) {
        ApplyVisuals(false);
    }
    return true;
}

void PlayerSpecialAttackCharge::ApplyReplicatedCharge(int hits)
{
    ClampSettings();
    currentHits = std::clamp(hits, 0, hitsToFullyCharge);
    if (IsLocalPlayer()) {
        ApplyVisuals(false);
    }
}

bool PlayerSpecialAttackCharge::ActivateOnPawn(
    RTBEngine::Scene::GameObject* pawn,
    const RTBEngine::Math::Vector3& direction,
    float aimStrength,
    bool requireCharge)
{
    if (!pawn) {
        return false;
    }

    PlayerSpecialAttackCharge* charge = pawn->GetComponent<PlayerSpecialAttackCharge>();
    if (charge) {
        charge->CacheGameplayReferences();
    }

    IPlayerSpecialAttack* attack = charge
        ? charge->specialAttack
        : ResolvePlayerSpecialAttack(pawn);
    if (!attack || attack->IsActive()) {
        return false;
    }

    if (requireCharge && (!charge || !charge->IsReady())) {
        return false;
    }

    if (!attack->TryActivate(direction, aimStrength)) {
        return false;
    }

    if (!charge) {
        return true;
    }

    if (requireCharge || charge->IsLocalPlayer()) {
        charge->currentHits = 0;
        if (charge->IsLocalPlayer()) {
            charge->ApplyVisuals(false);
        }
    }

    return true;
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
    if (!IsLocalPlayer()) {
        return;
    }

    if (specialAttack) {
        specialAttack->HideAimPreview();
    }

    if (!IsReady()) {
        return;
    }

    if (!specialAttack || specialAttack->IsActive()) {
        return;
    }

    if (!controller) {
        return;
    }

    const float aimStrength = std::clamp(joystickValue.Length(), 0.0f, 1.0f);
    const RTBEngine::Math::Vector3 attackDirection =
        controller->GetPlanarAttackDirectionFromJoystick(joystickValue);
    if (!CharacterCombatUtils::HasPlanarDirection(attackDirection)) {
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        GameNet::OnlineGameNetSubsystem::SendSpecialAttack(attackDirection, aimStrength);
        return;
    }

    if (!specialAttack->TryActivate(attackDirection, aimStrength)) {
        return;
    }

    ConsumeCharge();

    if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        const RTBEngine::Scene::NetworkIdentity* identity =
            owner->GetComponent<RTBEngine::Scene::NetworkIdentity>();
        if (identity && identity->networkPlayerSlot >= 0) {
            GameNet::OnlineGameNetSubsystem::BroadcastSpecialAttack(
                identity->networkPlayerSlot,
                attackDirection,
                aimStrength);
        }
    }
}

bool PlayerSpecialAttackCharge::TryGetSpecialAimDirection(
    RTBEngine::Math::Vector3& outAimDirection) const
{
    if (!IsLocalPlayer() || !specialAttackJoystick || !IsReady()) {
        return false;
    }

    if (!specialAttackJoystick->IsDragging()) {
        return false;
    }

    if (!specialAttack || specialAttack->IsActive()) {
        return false;
    }

    if (!controller) {
        return false;
    }

    const RTBEngine::Math::Vector2 joystickValue = specialAttackJoystick->GetValue();
    if (joystickValue.LengthSquared() <= 0.0001f) {
        return false;
    }

    outAimDirection = controller->GetPlanarAttackDirectionFromJoystick(joystickValue);
    return CharacterCombatUtils::HasPlanarDirection(outAimDirection);
}

void PlayerSpecialAttackCharge::OnLateUpdate(float /*deltaTime*/)
{
    if (!IsLocalPlayer() || !specialAttack) {
        return;
    }

    RTBEngine::Math::Vector3 aimDirection = RTBEngine::Math::Vector3::Zero();
    if (!TryGetSpecialAimDirection(aimDirection)) {
        specialAttack->HideAimPreview();
        return;
    }

    const float aimStrength = specialAttackJoystick
        ? std::clamp(specialAttackJoystick->GetValue().Length(), 0.0f, 1.0f)
        : 1.0f;
    specialAttack->UpdateAimPreview(aimDirection, aimStrength);
}

void PlayerSpecialAttackCharge::RefreshAfterSpawn()
{
    ClampSettings();
    CacheGameplayReferences();
    if (!IsLocalPlayer()) {
        specialAttackJoystick = nullptr;
        readyIcon = nullptr;
        UnsubscribeFromSpecialJoystick();
        SetUpdateTickEnabled(false);
        return;
    }

    RebindSpecialJoystickSubscription();
    SetUpdateTickEnabled(true);
    currentHits = 0;
    ApplyVisuals(false);
}

void PlayerSpecialAttackCharge::OnStart()
{
    ClampSettings();
    CacheGameplayReferences();
    if (!IsLocalPlayer()) {
        specialAttackJoystick = nullptr;
        readyIcon = nullptr;
        SetUpdateTickEnabled(false);
        return;
    }

    RebindSpecialJoystickSubscription();
    SetUpdateTickEnabled(true);
    currentHits = 0;
    ApplyVisuals(false);
}

void PlayerSpecialAttackCharge::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
    if (IsLocalPlayer()) {
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
    specialAttack = nullptr;
    controller = nullptr;
}
