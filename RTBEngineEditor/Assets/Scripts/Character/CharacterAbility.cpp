#include "CharacterAbility.h"

#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>
#include <cmath>

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;
    constexpr float kCompletionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }
}

void CharacterAbility::OnUpdate(float deltaTime)
{
    const float safeDeltaTime = std::max(0.0f, deltaTime);
    cooldownRemaining = std::max(0.0f, cooldownRemaining - safeDeltaTime);

    if (!abilityActive) {
        return;
    }

    activeElapsed += safeDeltaTime;
    ProcessActiveAbility(safeDeltaTime);
}

void CharacterAbility::ProcessActiveAbility(float safeDeltaTime)
{
    if (GetTickInterval() > kCompletionEpsilon) {
        ProcessTickedHitAbility(safeDeltaTime);
        return;
    }

    ProcessSingleHitAbility(safeDeltaTime);
}

void CharacterAbility::ProcessSingleHitAbility(float /*safeDeltaTime*/)
{
    if (!hitExecuted && activeElapsed + kCompletionEpsilon >= GetHitDelayDuration()) {
        hitExecuted = true;
        ExecuteAbilityHit();
    }

    if (activeElapsed + kCompletionEpsilon >= GetHitDelayDuration() + GetRecoveryDuration()) {
        FinishAbility();
    }
}

void CharacterAbility::ProcessTickedHitAbility(float safeDeltaTime)
{
    const float tickInterval = std::max(kCompletionEpsilon, GetTickInterval());
    const float activeWindowStart = GetHitDelayDuration();
    const float activeWindowEnd = GetHitDelayDuration() + GetRecoveryDuration();
    const int maxTicks = GetTickCount();

    if (activeElapsed + kCompletionEpsilon >= activeWindowStart) {
        if (maxTicks > 1) {
            if (ticksExecuted == 0) {
                ++ticksExecuted;
                ExecuteAbilityHit();
            } else {
                tickAccumulator += safeDeltaTime;
                while (tickAccumulator + kCompletionEpsilon >= tickInterval &&
                       ticksExecuted < maxTicks) {
                    tickAccumulator -= tickInterval;
                    ++ticksExecuted;
                    ExecuteAbilityHit();
                }
            }
        } else {
            tickAccumulator += safeDeltaTime;
            while (tickAccumulator + kCompletionEpsilon >= tickInterval &&
                   activeElapsed <= activeWindowEnd + kCompletionEpsilon) {
                tickAccumulator -= tickInterval;
                ExecuteAbilityHit();
            }
        }
    }

    if (maxTicks > 1 && ticksExecuted >= maxTicks) {
        FinishAbility();
        return;
    }

    if (activeElapsed + kCompletionEpsilon >= activeWindowEnd) {
        if (maxTicks > 1 && ticksExecuted < maxTicks) {
            while (ticksExecuted < maxTicks) {
                ++ticksExecuted;
                ExecuteAbilityHit();
            }
        }

        FinishAbility();
    }
}

void CharacterAbility::OnDestroy()
{
    CancelAbility();
}

bool CharacterAbility::TryActivate(RTBEngine::Scene::GameObject* instigator,
                                   const RTBEngine::Math::Vector3& direction)
{
    RTBEngine::Math::Vector3 planarDirection = direction;
    planarDirection.y = 0.0f;
    if (!instigator || abilityActive || cooldownRemaining > 0.0f ||
        !HasPlanarDirection(planarDirection) ||
        !CanActivateAbility(instigator, planarDirection)) {
        return false;
    }

    planarDirection.Normalize();
    activeInstigator = instigator;
    activeDirection = planarDirection;
    activeElapsed = 0.0f;
    tickAccumulator = 0.0f;
    ticksExecuted = 0;
    hitExecuted = false;
    abilityActive = true;
    cooldownRemaining = std::max(0.0f, GetCooldownDuration());
    OnAbilityStarted();
    return true;
}

void CharacterAbility::CancelAbility()
{
    activeInstigator = nullptr;
    activeDirection = RTBEngine::Math::Vector3::Forward();
    activeElapsed = 0.0f;
    tickAccumulator = 0.0f;
    ticksExecuted = 0;
    hitExecuted = false;
    abilityActive = false;
}

bool CharacterAbility::CanActivateAbility(RTBEngine::Scene::GameObject* /*instigator*/,
                                          const RTBEngine::Math::Vector3& /*direction*/) const
{
    return true;
}

void CharacterAbility::FinishAbility()
{
    OnAbilityFinished();
    CancelAbility();
}
