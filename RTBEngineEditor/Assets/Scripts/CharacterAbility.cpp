#include "CharacterAbility.h"

#include <RTBEngine/ECS/GameObject.h>

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
    if (!hitExecuted && activeElapsed + kCompletionEpsilon >= GetHitDelayDuration()) {
        hitExecuted = true;
        ExecuteAbilityHit();
    }

    if (activeElapsed + kCompletionEpsilon >= GetHitDelayDuration() + GetRecoveryDuration()) {
        FinishAbility();
    }
}

void CharacterAbility::OnDestroy()
{
    CancelAbility();
}

bool CharacterAbility::TryActivate(RTBEngine::ECS::GameObject* instigator,
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
    hitExecuted = false;
    abilityActive = false;
}

bool CharacterAbility::CanActivateAbility(RTBEngine::ECS::GameObject* /*instigator*/,
                                          const RTBEngine::Math::Vector3& /*direction*/) const
{
    return true;
}

void CharacterAbility::FinishAbility()
{
    OnAbilityFinished();
    CancelAbility();
}
