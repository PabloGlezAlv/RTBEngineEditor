#include "EnemyAnimationDriver.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Scene/GameObject.h>

using ThisClass = EnemyAnimationDriver;

namespace {
    constexpr const char* kAnimWalk = "Walk";
    constexpr const char* kAnimAttack = "Attack";
    constexpr const char* kAnimDeath = "Death";
}

RTB_REGISTER_COMPONENT(EnemyAnimationDriver)
    RTB_PROPERTY_COMPONENT(animator, Animator)
RTB_END_REGISTER(EnemyAnimationDriver)

void EnemyAnimationDriver::OnStart()
{
    ResolveAnimator();
}

void EnemyAnimationDriver::OnValidate()
{
    ResolveAnimator();
}

void EnemyAnimationDriver::ResolveAnimator()
{
    if (!owner) {
        animator = nullptr;
        return;
    }

    if (!animator) {
        animator = owner->GetComponentInChildren<RTBEngine::Animation::Animator>();
    }
}

void EnemyAnimationDriver::PlayWalkLoop()
{
    if (!animator || !animator->HasKey(kAnimWalk)) {
        return;
    }

    if (animator->IsPlayingKey(kAnimWalk) && animator->IsPlaying()) {
        return;
    }

    animator->PlayKey(kAnimWalk);
}

bool EnemyAnimationDriver::PlayAttack()
{
    if (!HasAttackAnimation()) {
        return false;
    }

    animator->PlayKey(kAnimAttack, false);
    return true;
}

bool EnemyAnimationDriver::PlayDeath()
{
    if (!HasDeathAnimation()) {
        return false;
    }

    animator->PlayKey(kAnimDeath, false);
    return true;
}

bool EnemyAnimationDriver::HasAttackAnimation() const
{
    return animator && animator->HasKey(kAnimAttack);
}

bool EnemyAnimationDriver::HasDeathAnimation() const
{
    return animator && animator->HasKey(kAnimDeath);
}

bool EnemyAnimationDriver::IsAttackPlaying() const
{
    return HasAttackAnimation() && animator->IsPlayingKey(kAnimAttack);
}

bool EnemyAnimationDriver::IsDeathPlaying() const
{
    return HasDeathAnimation() && animator->IsPlayingKey(kAnimDeath);
}

void EnemyAnimationDriver::HoldDeathPose()
{
    if (!HasDeathAnimation()) {
        return;
    }

    animator->HoldCurrentPose();
}
