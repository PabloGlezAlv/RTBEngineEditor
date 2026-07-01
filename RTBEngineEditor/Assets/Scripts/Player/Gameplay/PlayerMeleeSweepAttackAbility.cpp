#include "PlayerMeleeSweepAttackAbility.h"

#include "CharacterBase.h"
#include "CharacterCombatUtils.h"
#include "PlayerAmmoSystem.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Physics/PhysicsWorld.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerMeleeSweepAttackAbility;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    int ResolveCharacterTeam(RTBEngine::ECS::GameObject* gameObject)
    {
        return CharacterCombatUtils::ResolveCharacterTeam(gameObject);
    }

    RTBEngine::ECS::GameObject* ResolveHealthRoot(RTBEngine::ECS::GameObject* gameObject)
    {
        if (!gameObject) {
            return nullptr;
        }

        for (RTBEngine::ECS::GameObject* current = gameObject; current; current = current->GetParent()) {
            if (current->GetComponent<HealthComponent>()) {
                return current;
            }
        }

        return nullptr;
    }

    bool IsLocallyControlledInstigator(RTBEngine::ECS::GameObject* instigator)
    {
        if (!instigator) {
            return false;
        }

        const RTBEngine::ECS::NetworkIdentity* identity =
            instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return true;
        }

        return identity->IsLocallyControlled();
    }

    bool HasSimulationAuthority(RTBEngine::ECS::GameObject* instigator)
    {
        if (!instigator) {
            return false;
        }

        if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
            const RTBEngine::ECS::NetworkIdentity* identity =
                instigator->GetComponent<RTBEngine::ECS::NetworkIdentity>();
            if (identity && !identity->IsSimulatedByHost()) {
                return false;
            }
        }

        return true;
    }
}

RTB_REGISTER_COMPONENT(PlayerMeleeSweepAttackAbility)
    RTB_PROPERTY(attackOriginOffset)
    RTB_PROPERTY_RANGE(cooldown, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(damage, 0.0f, 1000.0f)
    RTB_PROPERTY_RANGE(hitDelay, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(recoveryDuration, 0.0f, 10.0f)
    RTB_PROPERTY_RANGE(sphereRadius, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(sphereDistance, 0.05f, 10.0f)
    RTB_PROPERTY_RANGE(tickInterval, 0.1f, 2.0f)
    RTB_PROPERTY_RANGE(knockbackStrength, 0.0f, 20.0f)
    RTB_PROPERTY(ignoreSameTeam)
    RTB_PROPERTY_COMPONENT(hitAudio, AudioSourceComponent)
RTB_END_REGISTER(PlayerMeleeSweepAttackAbility)

void PlayerMeleeSweepAttackAbility::OnValidate()
{
    ClampSettings();
}

void PlayerMeleeSweepAttackAbility::SetMeleeCombatOverrides(
    float damageAmount,
    float range,
    float radius,
    float tickSeconds,
    float knockback)
{
    damage = std::max(0.0f, damageAmount);
    sphereDistance = std::max(0.05f, range);
    sphereRadius = std::max(0.05f, radius);
    tickInterval = std::max(0.1f, tickSeconds);
    knockbackStrength = std::max(0.0f, knockback);
    ClampSettings();
}

bool PlayerMeleeSweepAttackAbility::CanActivateAbility(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& direction) const
{
    if (!instigator || !HasValidAttack() || !HasPlanarDirection(direction)) {
        return false;
    }

    if (IsLocallyControlledInstigator(instigator)) {
        if (const auto* ammoSystem = instigator->GetComponent<PlayerAmmoSystem>()) {
            if (!ammoSystem->CanFire()) {
                return false;
            }
        }
    }

    return true;
}

void PlayerMeleeSweepAttackAbility::OnAbilityStarted()
{
    RTBEngine::ECS::GameObject* instigator = GetActiveInstigator();
    if (!instigator || !IsLocallyControlledInstigator(instigator)) {
        return;
    }

    if (auto* ammoSystem = instigator->GetComponent<PlayerAmmoSystem>()) {
        ammoSystem->ConsumeShot();
    }
}

void PlayerMeleeSweepAttackAbility::ExecuteAbilityHit()
{
    RTBEngine::ECS::GameObject* instigator = GetActiveInstigator();
    if (!instigator || !HasSimulationAuthority(instigator)) {
        return;
    }

    ApplySweepHits(instigator, GetActiveDirection());
}

bool PlayerMeleeSweepAttackAbility::ApplySweepHits(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& attackDirection)
{
    ClampSettings();

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene || damage <= 0.0f) {
        return false;
    }

    RTBEngine::Math::Vector3 castDirection = attackDirection;
    castDirection.y = 0.0f;
    if (!HasPlanarDirection(castDirection)) {
        castDirection = instigator->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
        castDirection.y = 0.0f;
    }
    if (!HasPlanarDirection(castDirection)) {
        return false;
    }
    castDirection.Normalize();

    const RTBEngine::Math::Vector3 castStart =
        instigator->GetWorldPosition() + (instigator->GetWorldRotation() * attackOriginOffset);
    const int instigatorTeam = ResolveCharacterTeam(instigator);
    bool anyHit = false;

    for (const auto& gameObject : scene->GetGameObjects()) {
        if (!gameObject || !gameObject->IsActiveInHierarchy() || gameObject.get() == instigator) {
            continue;
        }

        HealthComponent* targetHealth = gameObject->GetComponent<HealthComponent>();
        if (!targetHealth) {
            targetHealth = gameObject->GetComponentInChildren<HealthComponent>();
        }
        if (!targetHealth || targetHealth->IsDead()) {
            continue;
        }

        RTBEngine::ECS::GameObject* targetRoot = ResolveHealthRoot(targetHealth->GetOwner());
        if (!targetRoot || targetRoot == instigator) {
            continue;
        }

        const int targetTeam = ResolveCharacterTeam(targetRoot);
        if (ignoreSameTeam &&
            instigatorTeam != static_cast<int>(CharacterTeam::Neutral) &&
            instigatorTeam == targetTeam) {
            continue;
        }

        RTBEngine::Math::Vector3 targetPosition = targetRoot->GetWorldPosition();
        targetPosition.y = castStart.y;

        RTBEngine::Math::Vector3 toTarget = targetPosition - castStart;
        const float along = toTarget.x * castDirection.x + toTarget.z * castDirection.z;
        if (along < 0.0f || along > sphereDistance) {
            continue;
        }

        RTBEngine::Math::Vector3 closestPoint = castStart + castDirection * along;
        RTBEngine::Math::Vector3 lateral = targetPosition - closestPoint;
        lateral.y = 0.0f;
        const float lateralDistanceSquared = lateral.LengthSquared();
        const float hitRadius = sphereRadius + 0.35f;
        if (lateralDistanceSquared > hitRadius * hitRadius) {
            continue;
        }

        HealthComponent::DamageContext damageContext;
        damageContext.amount = damage;
        damageContext.instigator = instigator;
        damageContext.hitPoint = targetPosition;
        damageContext.hitDirection = castDirection;
        damageContext.knockbackStrength = knockbackStrength;
        targetHealth->TakeDamage(damage, damageContext);

        anyHit = true;
    }

    if (anyHit && hitAudio && IsLocallyControlledInstigator(instigator)) {
        hitAudio->PlayOneShot();
    }

    return anyHit;
}

void PlayerMeleeSweepAttackAbility::ClampSettings()
{
    cooldown = std::max(0.0f, cooldown);
    damage = std::max(0.0f, damage);
    hitDelay = std::max(0.0f, hitDelay);
    recoveryDuration = std::max(0.05f, recoveryDuration);
    sphereRadius = std::max(0.05f, sphereRadius);
    sphereDistance = std::max(0.05f, sphereDistance);
    tickInterval = std::max(0.1f, tickInterval);
    knockbackStrength = std::max(0.0f, knockbackStrength);
}
