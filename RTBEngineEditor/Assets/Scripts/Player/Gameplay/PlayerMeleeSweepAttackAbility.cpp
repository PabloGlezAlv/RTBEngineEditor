#include "PlayerMeleeSweepAttackAbility.h"

#include "CharacterCombatUtils.h"
#include "CharacterDefinition.h"
#include "CombatAuthority.h"
#include "HealthComponent.h"
#include "PlayerAmmoSystem.h"

#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerMeleeSweepAttackAbility;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
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
    RTB_PROPERTY_RANGE(tickInterval, 0.0f, 2.0f)
    RTB_PROPERTY_RANGE(tickCount, 1, 8)
    RTB_PROPERTY_RANGE(knockbackStrength, 0.0f, 20.0f)
    RTB_PROPERTY(ignoreSameTeam)
    RTB_PROPERTY_COMPONENT(hitAudio, AudioSourceComponent)
RTB_END_REGISTER(PlayerMeleeSweepAttackAbility)

void PlayerMeleeSweepAttackAbility::OnValidate()
{
    ClampSettings();
}

void PlayerMeleeSweepAttackAbility::ApplyCharacterStats(const CharacterDefinition& definition)
{
    SetMeleeCombatOverrides(
        definition.projectileDamage,
        definition.meleeRange,
        definition.meleeRadius,
        definition.meleeTickInterval,
        definition.meleeTickCount,
        definition.meleeKnockback);
}

void PlayerMeleeSweepAttackAbility::SetMeleeCombatOverrides(
    float damageAmount,
    float range,
    float radius,
    float tickSeconds,
    int definitionTickCount,
    float knockback)
{
    totalAttackDamage = std::max(0.0f, damageAmount);
    attackTickCount = std::max(1, definitionTickCount);
    if (attackTickCount <= 1 && tickSeconds > 0.0f && tickCount > 1) {
        attackTickCount = tickCount;
    }
    tickCount = attackTickCount;

    sphereDistance = std::max(0.05f, range);
    sphereRadius = std::max(0.05f, radius);
    knockbackStrength = std::max(0.0f, knockback);

    if (attackTickCount <= 1 || tickSeconds <= 0.0f) {
        damagePerHit = totalAttackDamage;
        damage = totalAttackDamage;
        tickInterval = 0.0f;
    } else {
        damagePerHit = totalAttackDamage / static_cast<float>(attackTickCount);
        damage = damagePerHit;
        tickInterval = std::max(0.1f, tickSeconds);
        recoveryDuration = static_cast<float>(attackTickCount - 1) * tickInterval;
    }

    ClampSettings();
}

int PlayerMeleeSweepAttackAbility::GetTickCount() const
{
    if (tickInterval <= 0.0f) {
        return 0;
    }

    const int ticks = attackTickCount > 1 ? attackTickCount : tickCount;
    return ticks > 1 ? ticks : 0;
}

float PlayerMeleeSweepAttackAbility::GetDamagePerHit() const
{
    if (damagePerHit > 0.0f) {
        return damagePerHit;
    }

    if (totalAttackDamage > 0.0f && GetTickCount() > 1) {
        return totalAttackDamage / static_cast<float>(GetTickCount());
    }

    return totalAttackDamage > 0.0f ? totalAttackDamage : damage;
}

bool PlayerMeleeSweepAttackAbility::CanActivateAbility(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& direction) const
{
    if (!instigator || !HasValidAttack() || !HasPlanarDirection(direction)) {
        return false;
    }

    if (!CharacterCombatUtils::ResolvePhysicsWorld(instigator)) {
        return false;
    }

    if (CombatAuthority::CanConsumeAmmo(instigator)) {
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
    if (!instigator || !CombatAuthority::CanConsumeAmmo(instigator)) {
        return;
    }

    if (auto* ammoSystem = instigator->GetComponent<PlayerAmmoSystem>()) {
        ammoSystem->ConsumeShot();
    }
}

void PlayerMeleeSweepAttackAbility::ExecuteAbilityHit()
{
    RTBEngine::ECS::GameObject* instigator = GetActiveInstigator();
    if (!instigator || !CombatAuthority::CanApplyDamage(instigator)) {
        return;
    }

    ApplySweepHits(instigator, GetActiveDirection(), GetDamagePerHit());
}

bool PlayerMeleeSweepAttackAbility::ApplySweepHits(
    RTBEngine::ECS::GameObject* instigator,
    const RTBEngine::Math::Vector3& attackDirection,
    float hitDamage)
{
    ClampSettings();

    if (!instigator || hitDamage <= 0.0f) {
        return false;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld =
        CharacterCombatUtils::ResolvePhysicsWorld(instigator);
    if (!physicsWorld) {
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

    CharacterCombatUtils::HostileOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = physicsWorld;
    overlapQuery.instigator = instigator;
    overlapQuery.origin = castStart;
    overlapQuery.direction = castDirection;
    overlapQuery.distance = sphereDistance;
    overlapQuery.radius = sphereRadius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargets(overlapQuery);

    bool anyHit = false;
    for (const CharacterCombatUtils::HostileOverlapHit& hit : hits) {
        if (!hit.health || hit.health->IsDead()) {
            continue;
        }

        HealthComponent::DamageContext damageContext;
        damageContext.amount = hitDamage;
        damageContext.instigator = instigator;
        damageContext.hitPoint = hit.hitPoint;
        damageContext.hitDirection = castDirection;
        damageContext.knockbackStrength = knockbackStrength;
        hit.health->TakeDamage(hitDamage, damageContext);

        anyHit = true;
    }

    if (anyHit && hitAudio && CombatAuthority::IsLocallyControlled(instigator)) {
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
    if (tickInterval > 0.0f) {
        tickInterval = std::max(0.1f, tickInterval);
    }
    tickCount = std::max(1, tickCount);
    attackTickCount = std::max(1, attackTickCount);
    knockbackStrength = std::max(0.0f, knockbackStrength);
}
