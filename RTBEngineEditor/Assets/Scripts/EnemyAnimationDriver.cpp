#include "EnemyAnimationDriver.h"

#include "EnemyMeleeAIShared.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>

using ThisClass = EnemyAnimationDriver;

RTB_REGISTER_COMPONENT(EnemyAnimationDriver)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_FBX(walkAnimationFbx)
    RTB_PROPERTY_FBX(attackAnimationFbx)
    RTB_PROPERTY_FBX(deathAnimationFbx)
RTB_END_REGISTER(EnemyAnimationDriver)

void EnemyAnimationDriver::OnStart()
{
    ResolveAnimator();
    RegisterAnimationSlots();
}

void EnemyAnimationDriver::OnValidate()
{
    ResolveAnimator();
    if (animator) {
        RegisterAnimationSlots();
    }
}

void EnemyAnimationDriver::ResolveAnimator()
{
    if (!owner) {
        animator = nullptr;
        return;
    }

    animator = owner->GetComponentInChildren<RTBEngine::Animation::Animator>();
}

void EnemyAnimationDriver::RegisterAnimationSlots()
{
    const bool animatorChanged = (registeredAnimator != animator);
    if (animatorChanged) {
        registeredAnimator = animator;
        walkSlotState = {};
        attackSlotState = {};
        deathSlotState = {};
    }

    if (!animator) {
        if (!missingAnimatorWarningShown &&
            (!walkAnimationFbx.empty() || !attackAnimationFbx.empty() || !deathAnimationFbx.empty())) {
            RTB_WARN("[EnemyAnimationDriver] Assign an Animator component to use FBX animation slots.");
            missingAnimatorWarningShown = true;
        }
        return;
    }

    missingAnimatorWarningShown = false;
    RegisterAnimationSlot("Walk", walkAnimationFbx, EnemyMeleeAIDetail::kWalkAlias, walkSlotState);
    RegisterAnimationSlot("Attack", attackAnimationFbx, EnemyMeleeAIDetail::kAttackAlias, attackSlotState);
    RegisterAnimationSlot("Death", deathAnimationFbx, EnemyMeleeAIDetail::kDeathAlias, deathSlotState);
}

void EnemyAnimationDriver::RegisterAnimationSlot(const char* slotLabel,
                                                 const std::string& sourceFbx,
                                                 const char* alias,
                                                 AnimationSlotState& slotState)
{
    if (slotState.sourceFbx == sourceFbx) {
        return;
    }

    slotState.sourceFbx = sourceFbx;
    slotState.ready = false;

    if (!animator || sourceFbx.empty()) {
        return;
    }

    if (!animator->LoadClipFromFbx(alias, sourceFbx)) {
        RTB_WARN(std::string("[EnemyAnimationDriver] ") + slotLabel +
                 " slot FBX has no usable animation clip: " + sourceFbx);
        return;
    }

    slotState.ready = true;
}

void EnemyAnimationDriver::PlayWalkLoop()
{
    if (!animator || !walkSlotState.ready || !animator->GetClip(EnemyMeleeAIDetail::kWalkAlias)) {
        return;
    }

    if (animator->GetCurrentClipName() == EnemyMeleeAIDetail::kWalkAlias && animator->IsPlaying()) {
        return;
    }

    animator->Play(EnemyMeleeAIDetail::kWalkAlias, true);
}

bool EnemyAnimationDriver::PlayAttack()
{
    if (!HasAttackAnimation()) {
        return false;
    }

    animator->Play(EnemyMeleeAIDetail::kAttackAlias, false);
    return true;
}

bool EnemyAnimationDriver::PlayDeath()
{
    if (!HasDeathAnimation()) {
        return false;
    }

    animator->Play(EnemyMeleeAIDetail::kDeathAlias, false);
    return true;
}

bool EnemyAnimationDriver::HasAttackAnimation() const
{
    return animator && attackSlotState.ready && animator->GetClip(EnemyMeleeAIDetail::kAttackAlias);
}

bool EnemyAnimationDriver::HasDeathAnimation() const
{
    return animator && deathSlotState.ready && animator->GetClip(EnemyMeleeAIDetail::kDeathAlias);
}

bool EnemyAnimationDriver::IsAttackPlaying() const
{
    return HasAttackAnimation() &&
        animator->GetCurrentClipName() == EnemyMeleeAIDetail::kAttackAlias &&
        animator->IsPlaying();
}

bool EnemyAnimationDriver::IsDeathPlaying() const
{
    return HasDeathAnimation() &&
        animator->GetCurrentClipName() == EnemyMeleeAIDetail::kDeathAlias &&
        animator->IsPlaying();
}
