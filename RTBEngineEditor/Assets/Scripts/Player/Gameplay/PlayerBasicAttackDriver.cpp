#include "PlayerBasicAttackDriver.h"

#include "ThirdPersonCharacterController.h"
#include "CharacterAbility.h"
#include "PauseMenuController.h"
#include "PlayerCameraBasis.h"
#include "PlayerCombatNet.h"
#include "PlayerAmmoSystem.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Animation/AnimationClip.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/TrailRenderer.h>
#include <RTBEngine/UI/Elements/UIJoystick.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerBasicAttackDriver;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;
    constexpr const char* kAnimAimDraw = "AimDraw";
    constexpr const char* kAnimAimLoop = "AimLoop";
    constexpr const char* kAnimAttack = "Attack";

    bool HasMovementInput(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }
}

RTB_REGISTER_COMPONENT(PlayerBasicAttackDriver)
    RTB_PROPERTY_COMPONENT(attackAbility, CharacterAbility)
    RTB_PROPERTY_COMPONENT(attackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(attackAimTrail, TrailRenderer)
    RTB_PROPERTY_SERIALIZED_RANGE(aimTrailForwardOffset, 0.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(aimTrailHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(aimTrailWallClipRadius, 0.05f, 3.0f)
    RTB_PROPERTY_GAMEOBJECT(aimArrowVisual)
RTB_END_REGISTER(PlayerBasicAttackDriver)

void PlayerBasicAttackDriver::BindController(ThirdPersonCharacterController* inController)
{
    controller = inController;
    ammoSystem = owner ? owner->GetComponent<PlayerAmmoSystem>() : nullptr;
    aimTrailPresenter.Bind(
        attackAimTrail,
        aimTrailForwardOffset,
        aimTrailHeightOffset,
        aimTrailWallClipRadius);
    if (owner) {
        aimTrailPresenter.CacheOwner(owner);
    }
}

void PlayerBasicAttackDriver::RefreshBindings()
{
    RebindAttackJoystickSubscription();
    RebindAnimatorKeySubscriptions();
}

void PlayerBasicAttackDriver::OnStart()
{
    RefreshBindings();
    SetAimArrowVisible(false);
}

void PlayerBasicAttackDriver::OnValidate()
{
    RefreshBindings();
    HideAimVisuals();
}

void PlayerBasicAttackDriver::OnDestroy()
{
    HideAimVisuals();
    aimArrowVisual = nullptr;
    UnsubscribeFromAttackJoystick();
    UnsubscribeFromAnimatorKeys();
}

void PlayerBasicAttackDriver::SetAttackJoystick(RTBEngine::UI::UIJoystick* joystick)
{
    attackJoystick = joystick;
    RebindAttackJoystickSubscription();
}

void PlayerBasicAttackDriver::UpdateLocalPredictedVisual(float deltaTime)
{
    if (combatPhase != CombatPhase::Attacking) {
        return;
    }

    PollAttackCompletion(deltaTime);
}

void PlayerBasicAttackDriver::UpdateLocalAimInput(float deltaTime)
{
    if (!controller) {
        return;
    }

    const bool dragging = attackJoystick && attackJoystick->IsDragging();
    if (dragging) {
        cachedAttackJoystickValue = attackJoystick->GetValue();
    }

    if (dragging && !wasDraggingJoystick && CanStartAiming()) {
        TryBeginAiming();
    }

    if (combatPhase == CombatPhase::Aiming) {
        if (dragging) {
            UpdateAimingState(deltaTime);
        } else if (wasDraggingJoystick) {
            const RTBEngine::Math::Vector3 releaseDirection =
                GetAttackDirectionFromJoystick(cachedAttackJoystickValue);
            if (!HasMovementInput(releaseDirection)) {
                FinishAiming();
            }
        } else {
            FinishAiming();
        }

        wasDraggingJoystick = dragging;
        if (combatPhase != CombatPhase::Attacking) {
            return;
        }
    }

    wasDraggingJoystick = dragging;

    if (combatPhase == CombatPhase::Idle) {
        if (controller->UsesReplicatedAnimator()) {
            controller->UpdateAnimatorFromReplicatedMotion(deltaTime);
        }
        return;
    }

    if (combatPhase == CombatPhase::Attacking) {
        PollAttackCompletion(0.0f);
    }
}

void PlayerBasicAttackDriver::UpdateFixedAiming(float deltaTime)
{
    if (controller) {
        controller->ApplyAimMovement(ResolveAimDirection(), deltaTime);
    }
}

void PlayerBasicAttackDriver::UpdateFixedAttacking(float fixedDeltaTime)
{
    HideAimVisuals();

    RTBEngine::Animation::Animator* pawnAnimator = controller ? controller->GetAnimator() : nullptr;
    const bool attackClipPlaying =
        pawnAnimator && pawnAnimator->HasKey(kAnimAttack) && !attackClipFinished;
    if (attackClipPlaying) {
        controller->ApplyAttackFacingLock(GetActiveAttackDirection(), fixedDeltaTime);
    }

    PollAttackCompletion(fixedDeltaTime);
}

void PlayerBasicAttackDriver::CancelForMenu()
{
    if (combatPhase == CombatPhase::Aiming) {
        FinishAiming();
    }
}

void PlayerBasicAttackDriver::CancelForDeath()
{
    combatPhase = CombatPhase::Idle;
    aimPhase = AimPhase::Draw;
    wasDraggingJoystick = false;
    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    attackStateElapsed = 0.0f;
    attackClipFinished = false;
    ClearAttackSafetyTimeout();
    if (attackAbility && attackAbility->IsAbilityActive()) {
        attackAbility->CancelAbility();
    }
    HideAimVisuals();
}

void PlayerBasicAttackDriver::HideAimVisuals()
{
    HideAttackAimTrail();
    SetAimArrowVisible(false);
}

void PlayerBasicAttackDriver::ResetAfterRevive()
{
    combatPhase = CombatPhase::Idle;
    aimPhase = AimPhase::Draw;
    wasDraggingJoystick = false;
    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    HideAimVisuals();
}

void PlayerBasicAttackDriver::TryProcessRemoteAttack()
{
    if (!controller || controller->IsLocallyControlled() || !attackAbility) {
        return;
    }

    RTBEngine::Math::Vector3 attackDirection = RTBEngine::Math::Vector3::Zero();
    if (!PlayerCombatNet::TryConsumeRemoteAttackDirection(
            owner,
            controller->GetNetworkIdentity(),
            lastProcessedRemoteAttackSequence,
            attackDirection)) {
        return;
    }

    StartAttack(attackDirection);
}

void PlayerBasicAttackDriver::PlayReplicatedVisual(const RTBEngine::Math::Vector3& attackDirection)
{
    if (!controller ||
        controller->IsLocallyControlled() ||
        controller->HasSimulationAuthority()) {
        return;
    }

    PlayPredictedAttackVisual(attackDirection);
}

RTBEngine::Math::Vector3 PlayerBasicAttackDriver::GetPlanarAttackDirectionFromJoystick(
    const RTBEngine::Math::Vector2& joystickValue) const
{
    RTBEngine::Math::Vector3 attackDirection = GetAttackDirectionFromJoystick(joystickValue);
    if (!HasMovementInput(attackDirection) && owner) {
        attackDirection = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
        attackDirection.y = 0.0f;
        if (attackDirection.LengthSquared() > kDirectionEpsilon) {
            attackDirection.Normalize();
        }
    }

    return attackDirection;
}

void PlayerBasicAttackDriver::ApplyCombatAnimationOverrides(
    const std::string& aimDrawFbx,
    const std::string& aimLoopFbx,
    const std::string& attackFbx)
{
    RTBEngine::Animation::Animator* targetAnimator = controller ? controller->EnsureAnimator() : nullptr;
    if (!targetAnimator) {
        return;
    }

    if (!aimDrawFbx.empty()) {
        targetAnimator->SetKeyClip(kAnimAimDraw, aimDrawFbx, false);
    }
    if (!aimLoopFbx.empty()) {
        targetAnimator->SetKeyClip(kAnimAimLoop, aimLoopFbx, true);
    }
    if (!attackFbx.empty()) {
        targetAnimator->SetKeyClip(kAnimAttack, attackFbx, false);
    }

    RebindAnimatorKeySubscriptions();
}

void PlayerBasicAttackDriver::RebindAttackJoystickSubscription()
{
    if (subscribedAttackJoystick == attackJoystick && attackJoystickReleaseSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromAttackJoystick();

    if (!attackJoystick) {
        return;
    }

    subscribedAttackJoystick = attackJoystick;
    attackJoystickReleaseSubscription = attackJoystick->SubscribeToReleased(
        [this](const RTBEngine::Math::Vector2& joystickValue) {
            cachedAttackJoystickValue = joystickValue;
            HandleJoystickAttackReleased(joystickValue);
        });
}

void PlayerBasicAttackDriver::UnsubscribeFromAttackJoystick()
{
    attackJoystickReleaseSubscription.Reset();
    subscribedAttackJoystick = nullptr;
}

void PlayerBasicAttackDriver::RebindAnimatorKeySubscriptions()
{
    UnsubscribeFromAnimatorKeys();

    RTBEngine::Animation::Animator* pawnAnimator = controller ? controller->GetAnimator() : nullptr;
    if (!pawnAnimator) {
        return;
    }

    aimDrawFinishedSubscription = pawnAnimator->SubscribeKeyFinished(
        kAnimAimDraw,
        [this](const RTBEngine::Animation::AnimationKeyFinishedEvent& /*event*/) {
            RTBEngine::Animation::Animator* currentAnimator =
                controller ? controller->GetAnimator() : nullptr;
            if (!currentAnimator ||
                combatPhase != CombatPhase::Aiming ||
                aimPhase != AimPhase::Draw) {
                return;
            }

            aimPhase = AimPhase::Hold;
            if (currentAnimator->HasKey(kAnimAimLoop)) {
                currentAnimator->PlayKey(kAnimAimLoop);
            }
        });

    attackFinishedSubscription = pawnAnimator->SubscribeKeyFinished(
        kAnimAttack,
        [this](const RTBEngine::Animation::AnimationKeyFinishedEvent& /*event*/) {
            if (combatPhase != CombatPhase::Attacking) {
                return;
            }

            attackClipFinished = true;
            PollAttackCompletion(0.0f);
        });
}

void PlayerBasicAttackDriver::UnsubscribeFromAnimatorKeys()
{
    aimDrawFinishedSubscription.Reset();
    attackFinishedSubscription.Reset();
}

void PlayerBasicAttackDriver::HandleJoystickAttackReleased(
    const RTBEngine::Math::Vector2& joystickValue)
{
    HandleAttackReleasedWithDirection(GetAttackDirectionFromJoystick(joystickValue));
}

void PlayerBasicAttackDriver::HandleAttackReleasedWithDirection(
    const RTBEngine::Math::Vector3& attackDirection)
{
    const bool wasAiming = combatPhase == CombatPhase::Aiming;
    HideAttackAimTrail();
    SetAimArrowVisible(false);

    if (!controller || !controller->IsLocallyControlled()) {
        return;
    }

    if (!attackAbility ||
        !attackAbility->HasValidAttack() ||
        PauseMenuController::IsAnyMenuOpen()) {
        if (wasAiming) {
            FinishAiming();
        }
        return;
    }

    if (!HasMovementInput(attackDirection)) {
        if (wasAiming) {
            FinishAiming();
        }
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        if (combatPhase == CombatPhase::Attacking) {
            PollAttackCompletion(0.0f);
            if (combatPhase == CombatPhase::Attacking) {
                if (wasAiming) {
                    FinishAiming();
                }
                return;
            }
        }

        if (attackAbility->ConsumesAmmo()) {
            if (ammoSystem && !ammoSystem->CanFire()) {
                if (wasAiming) {
                    FinishAiming();
                }
                return;
            }
            if (ammoSystem) {
                ammoSystem->ConsumeShot();
            }
        }

        controller->QueueNetworkAttack(attackDirection);
        PlayPredictedAttackVisual(attackDirection);
        return;
    }

    StartAttack(attackDirection);
}

bool PlayerBasicAttackDriver::CanStartAiming() const
{
    return controller &&
        attackJoystick &&
        attackAbility &&
        attackAbility->HasValidAttack() &&
        !PauseMenuController::IsAnyMenuOpen() &&
        combatPhase == CombatPhase::Idle &&
        !controller->IsDead();
}

void PlayerBasicAttackDriver::TryBeginAiming()
{
    if (!CanStartAiming()) {
        return;
    }

    combatPhase = CombatPhase::Aiming;
    aimPhase = AimPhase::Draw;
    SetAimArrowVisible(
        attackAbility->GetAimVisualKind() == CharacterAbility::AimVisualKind::RangedProjectile);

    RTBEngine::Animation::Animator* pawnAnimator = controller->GetAnimator();
    if (!pawnAnimator) {
        return;
    }

    if (pawnAnimator->HasKey(kAnimAimDraw)) {
        pawnAnimator->PlayKey(kAnimAimDraw, false);
        return;
    }

    if (pawnAnimator->HasKey(kAnimAimLoop)) {
        aimPhase = AimPhase::Hold;
        pawnAnimator->PlayKey(kAnimAimLoop);
    }
}

void PlayerBasicAttackDriver::UpdateAimingState(float /*deltaTime*/)
{
    UpdateAttackAimTrail();
    if (attackAbility &&
        attackAbility->GetAimVisualKind() == CharacterAbility::AimVisualKind::RangedProjectile) {
        SetAimArrowVisible(true);
    }
}

void PlayerBasicAttackDriver::FinishAiming()
{
    if (!controller || combatPhase != CombatPhase::Aiming) {
        return;
    }

    combatPhase = CombatPhase::Idle;
    aimPhase = AimPhase::Draw;
    SetAimArrowVisible(false);
    HideAttackAimTrail();

    controller->RefreshLocomotionAnimation();
}

RTBEngine::Math::Vector3 PlayerBasicAttackDriver::ResolveAimDirection() const
{
    RTBEngine::Math::Vector2 joystickSample = cachedAttackJoystickValue;
    if (attackJoystick && attackJoystick->IsDragging()) {
        joystickSample = attackJoystick->GetValue();
    }

    return GetAttackDirectionFromJoystick(joystickSample);
}

void PlayerBasicAttackDriver::SetAimArrowVisible(bool visible)
{
    if (!aimArrowVisual || !owner) {
        return;
    }

    if (owner->IsBeingDestroyed() || aimArrowVisual->IsBeingDestroyed()) {
        return;
    }

    for (RTBEngine::Scene::GameObject* current = aimArrowVisual; current; current = current->GetParent()) {
        if (current == owner) {
            aimArrowVisual->SetActive(visible);
            return;
        }
    }
}

void PlayerBasicAttackDriver::UpdateAttackAimTrail()
{
    if (!aimTrailPresenter.trail || !controller) {
        return;
    }

    if (!attackAbility ||
        !attackAbility->HasValidAttack() ||
        attackAbility->GetAimRangeForVisual() <= 0.0f ||
        combatPhase != CombatPhase::Aiming ||
        PauseMenuController::IsAnyMenuOpen()) {
        HideAttackAimTrail();
        return;
    }

    const RTBEngine::Math::Vector3 attackDirection =
        GetAttackDirectionFromJoystick(cachedAttackJoystickValue);
    if (!HasMovementInput(attackDirection)) {
        HideAttackAimTrail();
        return;
    }

    const RTBEngine::Math::Vector3 start =
        aimTrailPresenter.ResolveVisualOrigin(owner, attackDirection);
    const float maxRange = GetAimRangeForVisual();
    const float effectiveLength =
        aimTrailPresenter.ResolveClippedLength(owner, attackDirection, maxRange);
    const RTBEngine::Math::Vector3 end = start + attackDirection * effectiveLength;
    aimTrailPresenter.ShowSegment(start, end);
}

void PlayerBasicAttackDriver::HideAttackAimTrail()
{
    aimTrailPresenter.Hide();
}

void PlayerBasicAttackDriver::StartAttack(const RTBEngine::Math::Vector3& attackDirection)
{
    HideAttackAimTrail();
    SetAimArrowVisible(false);

    if (!controller ||
        controller->IsDead() ||
        combatPhase == CombatPhase::Attacking ||
        !attackAbility) {
        return;
    }

    RTBEngine::Math::Vector3 normalizedAttackDirection = attackDirection;
    normalizedAttackDirection.y = 0.0f;
    if (normalizedAttackDirection.LengthSquared() <= kDirectionEpsilon) {
        return;
    }

    normalizedAttackDirection.Normalize();
    activeAttackDirection = normalizedAttackDirection;
    controller->FaceAttackDirection(activeAttackDirection);

    if (!attackAbility->TryActivate(owner, activeAttackDirection)) {
        activeAttackDirection = RTBEngine::Math::Vector3::Zero();
        FinishAiming();
        return;
    }

    combatPhase = CombatPhase::Attacking;
    attackStateElapsed = 0.0f;
    attackClipFinished = false;
    ScheduleAttackSafetyTimeout();

    RTBEngine::Animation::Animator* pawnAnimator = controller->GetAnimator();
    if (pawnAnimator && pawnAnimator->HasKey(kAnimAttack)) {
        pawnAnimator->PlayKey(kAnimAttack, false);
    }
}

void PlayerBasicAttackDriver::FinishAttack()
{
    if (!controller || combatPhase != CombatPhase::Attacking) {
        return;
    }

    if (attackAbility && attackAbility->IsAbilityActive()) {
        attackAbility->CancelAbility();
    }

    ClearAttackSafetyTimeout();
    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    combatPhase = CombatPhase::Idle;
    aimPhase = AimPhase::Draw;
    attackStateElapsed = 0.0f;
    attackClipFinished = false;
    wasDraggingJoystick = false;
    HideAttackAimTrail();
    SetAimArrowVisible(false);
    controller->StopPlanarMotion();

    controller->RefreshLocomotionAnimation();
    controller->ForceStartLocomotionAnimation();
}

void PlayerBasicAttackDriver::PlayPredictedAttackVisual(
    const RTBEngine::Math::Vector3& attackDirection)
{
    HideAttackAimTrail();
    SetAimArrowVisible(false);

    if (!controller ||
        controller->IsDead() ||
        combatPhase == CombatPhase::Attacking ||
        !attackAbility) {
        return;
    }

    RTBEngine::Math::Vector3 normalizedAttackDirection = attackDirection;
    normalizedAttackDirection.y = 0.0f;
    if (!HasMovementInput(normalizedAttackDirection)) {
        return;
    }

    normalizedAttackDirection.Normalize();
    activeAttackDirection = normalizedAttackDirection;
    controller->FaceAttackDirection(activeAttackDirection);
    combatPhase = CombatPhase::Attacking;
    attackStateElapsed = 0.0f;
    attackClipFinished = false;
    ScheduleAttackSafetyTimeout();

    RTBEngine::Animation::Animator* pawnAnimator = controller->GetAnimator();
    if (pawnAnimator && pawnAnimator->HasKey(kAnimAttack)) {
        pawnAnimator->PlayKey(kAnimAttack, false);
    }
}

void PlayerBasicAttackDriver::PollAttackCompletion(float deltaTime)
{
    if (!controller || combatPhase != CombatPhase::Attacking) {
        return;
    }

    if (deltaTime > 0.0f) {
        attackStateElapsed += deltaTime;
    }

    if (attackAbility && attackAbility->IsAbilityActive() && !attackAbilitySafetyExpired) {
        return;
    }

    RTBEngine::Animation::Animator* pawnAnimator = controller->GetAnimator();
    const bool hasAttackAnimation = pawnAnimator && pawnAnimator->HasKey(kAnimAttack);

    if (hasAttackAnimation) {
        if (!attackClipFinished) {
            float maxAttackSeconds = 1.5f;
            if (RTBEngine::Animation::AnimationClip* clip = pawnAnimator->GetClip(kAnimAttack)) {
                maxAttackSeconds = std::max(0.05f, clip->GetDurationInSeconds()) + 0.08f;
            }

            if (attackStateElapsed < maxAttackSeconds) {
                return;
            }
        }
    } else if (attackAbility && attackAbility->IsAbilityActive() && attackStateElapsed < 0.05f) {
        return;
    }

    if (attackStateElapsed > 3.0f) {
        RTB_WARN("[PlayerBasicAttackDriver] Attack state timed out; forcing locomotion.");
    }

    FinishAttack();
}

void PlayerBasicAttackDriver::ClearAttackSafetyTimeout()
{
    if (attackSafetyHandle.IsValid()) {
        CancelInvoke(attackSafetyHandle);
        attackSafetyHandle = {};
    }

    attackAbilitySafetyExpired = false;
}

void PlayerBasicAttackDriver::ScheduleAttackSafetyTimeout()
{
    ClearAttackSafetyTimeout();

    constexpr float kAbilitySafetyTimeoutSeconds = 5.0f;
    attackSafetyHandle = Invoke(kAbilitySafetyTimeoutSeconds, [this]() {
        if (combatPhase != CombatPhase::Attacking) {
            return;
        }

        attackAbilitySafetyExpired = true;

        if (attackAbility && attackAbility->IsAbilityActive()) {
            RTB_WARN("[PlayerBasicAttackDriver] Attack ability timed out; forcing locomotion.");
        }
    });
}

float PlayerBasicAttackDriver::GetAimRangeForVisual() const
{
    if (!attackAbility) {
        return 0.0f;
    }

    return attackAbility->GetAimRangeForVisual();
}

RTBEngine::Math::Vector3 PlayerBasicAttackDriver::GetAttackDirectionFromJoystick(
    const RTBEngine::Math::Vector2& joystickValue) const
{
    RTBEngine::Math::Vector3 forward = RTBEngine::Math::Vector3::Forward();
    RTBEngine::Math::Vector3 right = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
    PlayerCameraBasis::GetPlanarBasis(forward, right);

    RTBEngine::Math::Vector3 attackDirection = right * joystickValue.x + forward * joystickValue.y;
    attackDirection.y = 0.0f;
    if (attackDirection.LengthSquared() <= kDirectionEpsilon) {
        return RTBEngine::Math::Vector3::Zero();
    }

    attackDirection.Normalize();
    return attackDirection;
}

RTBEngine::Math::Vector3 PlayerBasicAttackDriver::GetActiveAttackDirection() const
{
    RTBEngine::Math::Vector3 attackDirection = activeAttackDirection;
    attackDirection.y = 0.0f;
    if (attackDirection.LengthSquared() <= kDirectionEpsilon && owner) {
        attackDirection = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
        attackDirection.y = 0.0f;
    }

    if (attackDirection.LengthSquared() <= kDirectionEpsilon) {
        return RTBEngine::Math::Vector3::Forward();
    }

    attackDirection.Normalize();
    return attackDirection;
}
