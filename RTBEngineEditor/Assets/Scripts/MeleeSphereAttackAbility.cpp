#include "MeleeSphereAttackAbility.h"

#include "CharacterBase.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>
#include <cmath>

using ThisClass = MeleeSphereAttackAbility;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    bool IsWithinHierarchy(RTBEngine::ECS::GameObject* root, RTBEngine::ECS::GameObject* candidate)
    {
        if (!root || !candidate) {
            return false;
        }

        for (RTBEngine::ECS::GameObject* current = candidate; current; current = current->GetParent()) {
            if (current == root) {
                return true;
            }
        }

        return false;
    }

    int ResolveCharacterTeam(RTBEngine::ECS::GameObject* gameObject)
    {
        if (!gameObject) {
            return static_cast<int>(CharacterTeam::Neutral);
        }

        for (RTBEngine::ECS::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (auto* character = current->GetComponent<CharacterBase>()) {
                return character->GetTeam();
            }
        }

        return static_cast<int>(CharacterTeam::Neutral);
    }
}

RTB_REGISTER_COMPONENT(MeleeSphereAttackAbility)
    RTB_PROPERTY(attackOriginOffset)
    RTB_PROPERTY_RANGE(cooldown, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(damage, 0.0f, 1000.0f)
    RTB_PROPERTY_RANGE(hitDelay, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(recoveryDuration, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(sphereRadius, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(sphereDistance, 0.05f, 10.0f)
    RTB_PROPERTY(ignoreSameTeam)
RTB_END_REGISTER(MeleeSphereAttackAbility)

void MeleeSphereAttackAbility::OnValidate()
{
    ClampSettings();
}

void MeleeSphereAttackAbility::SetTargetContext(
    RTBEngine::ECS::GameObject* targetRoot,
    HealthComponent* targetHealth,
    RTBEngine::Physics::PhysicsWorld* physicsWorld)
{
    preparedTargetRoot = targetRoot;
    preparedTargetHealth = targetHealth;
    preparedPhysicsWorld = physicsWorld;
}

void MeleeSphereAttackAbility::ClearTargetContext()
{
    preparedTargetRoot = nullptr;
    preparedTargetHealth = nullptr;
    preparedPhysicsWorld = nullptr;
}

bool MeleeSphereAttackAbility::ApplySphereHit(
    RTBEngine::ECS::GameObject* instigator,
    RTBEngine::ECS::GameObject* targetRoot,
    HealthComponent* targetHealth,
    RTBEngine::Physics::PhysicsWorld* physicsWorld,
    const RTBEngine::Math::Vector3& fallbackDirection)
{
    ClampSettings();

    if (!instigator || !targetRoot || !targetHealth || !physicsWorld || damage <= 0.0f) {
        return false;
    }

    const int instigatorTeam = ResolveCharacterTeam(instigator);
    const int targetTeam = ResolveCharacterTeam(targetRoot);
    if (ignoreSameTeam &&
        instigatorTeam != static_cast<int>(CharacterTeam::Neutral) &&
        instigatorTeam == targetTeam) {
        return false;
    }

    RTBEngine::Math::Vector3 castStart =
        instigator->GetWorldPosition() + (instigator->GetWorldRotation() * attackOriginOffset);
    RTBEngine::Math::Vector3 castDirection =
        instigator->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
    castDirection.y = 0.0f;
    if (!HasPlanarDirection(castDirection)) {
        castDirection = fallbackDirection;
        castDirection.y = 0.0f;
    }
    if (!HasPlanarDirection(castDirection)) {
        castDirection = RTBEngine::Math::Vector3::Forward();
    } else {
        castDirection.Normalize();
    }

    RTBEngine::Physics::PhysicsQueryHit hit;
    RTBEngine::Physics::PhysicsQueryOptions queryOptions;
    queryOptions.ignoredObject = instigator;
    queryOptions.ignoreIgnoredObjectHierarchy = true;
    queryOptions.ignoreTriggers = true;

    if (!physicsWorld->SphereCastClosest(
            castStart,
            castStart + castDirection * sphereDistance,
            sphereRadius,
            hit,
            queryOptions)) {
        return false;
    }

    if (!IsWithinHierarchy(targetRoot, hit.gameObject)) {
        return false;
    }

    HealthComponent::DamageContext damageContext;
    damageContext.amount = damage;
    damageContext.instigator = instigator;
    damageContext.hitPoint = hit.point;
    damageContext.hitDirection = castDirection;
    targetHealth->TakeDamage(damage, damageContext);

    return true;
}

bool MeleeSphereAttackAbility::CanActivateAbility(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& direction) const
{
    return instigator &&
        preparedTargetRoot &&
        preparedTargetHealth &&
        preparedPhysicsWorld &&
        damage > 0.0f &&
        HasPlanarDirection(direction);
}

void MeleeSphereAttackAbility::ExecuteAbilityHit()
{
    ApplySphereHit(
        GetActiveInstigator(),
        preparedTargetRoot,
        preparedTargetHealth,
        preparedPhysicsWorld,
        GetActiveDirection());
}

void MeleeSphereAttackAbility::OnAbilityFinished()
{
    ClearTargetContext();
}

void MeleeSphereAttackAbility::ClampSettings()
{
    cooldown = std::max(0.0f, cooldown);
    damage = std::max(0.0f, damage);
    hitDelay = std::max(0.0f, hitDelay);
    recoveryDuration = std::max(0.0f, recoveryDuration);
    sphereRadius = std::max(0.05f, sphereRadius);
    sphereDistance = std::max(0.05f, sphereDistance);
}
