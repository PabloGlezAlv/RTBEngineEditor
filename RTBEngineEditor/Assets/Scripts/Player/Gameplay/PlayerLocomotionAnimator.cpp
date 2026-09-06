#include "PlayerLocomotionAnimator.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace {
    constexpr const char* kAnimIdle = "Idle";
    constexpr const char* kAnimWalk = "Walk";
    constexpr const char* kAnimRun = "Run";
    constexpr const char* kAnimAimDraw = "AimDraw";
    constexpr const char* kAnimAimLoop = "AimLoop";
    constexpr const char* kAnimAttack = "Attack";
    constexpr const char* kAnimDeath = "Death";

    bool IsCombatLocomotionBlockerClip(const std::string& clipName)
    {
        const std::string normalized =
            RTBEngine::Animation::Animator::NormalizeClipName(clipName);
        return normalized == kAnimAttack ||
            normalized == kAnimAimDraw ||
            normalized == kAnimAimLoop;
    }

    bool IsNonLocomotionFallbackClip(const std::string& clipName)
    {
        const std::string normalized =
            RTBEngine::Animation::Animator::NormalizeClipName(clipName);
        if (normalized == kAnimDeath ||
            normalized == kAnimAttack ||
            normalized == kAnimAimDraw ||
            normalized == kAnimAimLoop) {
            return true;
        }

        return normalized.rfind("Death", 0) == 0;
    }
}

void PlayerLocomotionAnimator::Bind(RTBEngine::Animation::Animator* targetAnimator)
{
    animator = targetAnimator;
}

void PlayerLocomotionAnimator::ResetReplicatedMotionSample()
{
    hasReplicatedMotionSample = false;
    replicatedPlanarSpeed = 0.0f;
}

void PlayerLocomotionAnimator::UpdateLocomotion(
    bool inLocomotionState,
    bool hasMovementInput,
    bool isRunning)
{
    if (!animator || !inLocomotionState) {
        return;
    }

    const char* targetKey = nullptr;
    if (hasMovementInput) {
        if (isRunning && animator->HasKey(kAnimRun)) {
            targetKey = kAnimRun;
        } else if (animator->HasKey(kAnimWalk)) {
            targetKey = kAnimWalk;
        }
    } else if (animator->HasKey(kAnimIdle)) {
        targetKey = kAnimIdle;
    }

    if (!targetKey) {
        return;
    }

    const std::string& currentClip = animator->GetCurrentClipName();
    if (!IsCombatLocomotionBlockerClip(currentClip) &&
        animator->IsPlayingKey(targetKey)) {
        return;
    }

    animator->PlayKey(targetKey);
}

void PlayerLocomotionAnimator::SampleReplicatedMotion(
    RTBEngine::Scene::GameObject* owner,
    float deltaTime,
    bool inLocomotionState,
    float baseMoveSpeed,
    float sprintMultiplier)
{
    if (!owner || !inLocomotionState) {
        return;
    }

    const RTBEngine::Math::Vector3 currentPosition = owner->GetWorldPosition();
    if (!hasReplicatedMotionSample) {
        lastReplicatedWorldPosition = currentPosition;
        hasReplicatedMotionSample = true;
        replicatedPlanarSpeed = 0.0f;
        UpdateLocomotion(inLocomotionState, false, false);
        return;
    }

    RTBEngine::Math::Vector3 delta = currentPosition - lastReplicatedWorldPosition;
    delta.y = 0.0f;
    lastReplicatedWorldPosition = currentPosition;

    const float timestep = std::max(deltaTime, 0.0001f);
    const float instantPlanarSpeed = delta.Length() / timestep;
    constexpr float kSpeedSmoothing = 10.0f;
    const float blend = 1.0f - std::exp(-kSpeedSmoothing * timestep);
    replicatedPlanarSpeed += (instantPlanarSpeed - replicatedPlanarSpeed) * blend;

    const float walkThreshold = std::max(0.02f, baseMoveSpeed * 0.08f);
    const float runThreshold = std::max(walkThreshold + 0.05f, baseMoveSpeed * sprintMultiplier * 0.45f);

    const bool hasMovementInput = replicatedPlanarSpeed >= walkThreshold;
    const bool isRunning = replicatedPlanarSpeed >= runThreshold;
    UpdateLocomotion(inLocomotionState, hasMovementInput, isRunning);
}

void PlayerLocomotionAnimator::ForceStartLocomotionAnimation(const RTBEngine::Scene::GameObject* owner)
{
    if (!animator) {
        return;
    }

    if (animator->HasKey(kAnimIdle)) {
        if (!animator->IsPlayingKey(kAnimIdle)) {
            animator->PlayKey(kAnimIdle);
        }
        return;
    }

    const std::vector<std::string> clipNames = animator->GetClipNames();
    for (const std::string& clipName : clipNames) {
        if (clipName == "T-Pose" || clipName == "TPose" || clipName == "BindPose" ||
            clipName == "bind_pose" || IsNonLocomotionFallbackClip(clipName)) {
            continue;
        }

        if (owner) {
            RTB_WARN("[ThirdPersonCharacterController] Falling back to clip '" + clipName +
                     "' on '" + owner->GetName() + "'.");
        }
        animator->Play(clipName, true);
        return;
    }

    if (animator->HasBones()) {
        if (owner) {
            RTB_WARN("[ThirdPersonCharacterController] No locomotion clip available on '" +
                     owner->GetName() + "'; holding bind pose.");
        }
        animator->HoldCurrentPose();
    }
}
