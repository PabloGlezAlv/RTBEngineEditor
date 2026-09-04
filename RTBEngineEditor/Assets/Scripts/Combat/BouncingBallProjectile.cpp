#include "BouncingBallProjectile.h"

#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/SceneManager.h>

#include <algorithm>
#include <cmath>

using ThisClass = BouncingBallProjectile;

namespace {
    constexpr float kWalkableNormalY = 0.65f;
    constexpr float kImpactFxBaseRadius = 1.0f;
}

RTB_REGISTER_COMPONENT(BouncingBallProjectile)
    RTB_PROPERTY_SERIALIZED_RANGE(speed, 1.0f, 40.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damage, 0.0f, 200.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(knockbackStrength, 0.0f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(contactRadius, 0.05f, 2.0f)
    RTB_PROPERTY_SERIALIZED(ignoreSameTeam)
    RTB_PROPERTY_SERIALIZED(bounceImpactPrefabRef)
RTB_END_REGISTER(BouncingBallProjectile)

void BouncingBallProjectile::ClampSettings()
{
    speed = std::max(1.0f, speed);
    damage = std::max(0.0f, damage);
    knockbackStrength = std::max(0.0f, knockbackStrength);
    contactRadius = std::max(0.05f, contactRadius);
}

void BouncingBallProjectile::ResolveBounceImpactPrefab()
{
    bounceImpactPrefab = nullptr;
    if (bounceImpactPrefabRef.empty()) {
        return;
    }

    const std::string poolKey =
        RTBEngine::Scene::ObjectPool::ResolvePoolKey(bounceImpactPrefabRef);
    bounceImpactPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(poolKey);
    if (!bounceImpactPrefab) {
        bounceImpactPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().Get(poolKey);
    }
}

void BouncingBallProjectile::OnStart()
{
    ClampSettings();
    ResolveBounceImpactPrefab();
    SetUpdateTickEnabled(false);
}

void BouncingBallProjectile::OnDestroy()
{
    instigator = nullptr;
    physicsWorld = nullptr;
    bounceImpactPrefab = nullptr;
    hitTargets.clear();
}

void BouncingBallProjectile::Launch(const LaunchConfig& config)
{
    ClampSettings();

    if (config.path.samples.size() < 2) {
        FinishAndDestroy();
        return;
    }

    path = config.path;
    instigator = config.instigator;
    physicsWorld = config.physicsWorld;
    speed = std::max(1.0f, config.speed);
    damage = std::max(0.0f, config.damage);
    knockbackStrength = std::max(0.0f, config.knockbackStrength);
    contactRadius = std::max(0.05f, config.contactRadius);
    ignoreSameTeam = config.ignoreSameTeam;
    if (!config.bounceImpactPrefabRef.empty()) {
        bounceImpactPrefabRef = config.bounceImpactPrefabRef;
    }
    ResolveBounceImpactPrefab();

    distanceTravelled = 0.0f;
    nextBounceIndex = 0;
    finished = false;
    pendingDestroy = false;
    launched = true;
    hitTargets.clear();

    previousPosition = path.samples.front();
    owner->GetTransform().SetPosition(previousPosition);
    SetUpdateTickEnabled(true);
}

bool BouncingBallProjectile::HasAlreadyHit(HealthComponent* target) const
{
    return std::find(hitTargets.begin(), hitTargets.end(), target) != hitTargets.end();
}

float BouncingBallProjectile::CurrentAoeRadius() const
{
    if (path.bounceCount <= 0) {
        return 1.0f;
    }
    const int index = std::clamp(nextBounceIndex, 0, path.bounceCount - 1);
    return path.bounceRadii[static_cast<std::size_t>(index)];
}

void BouncingBallProjectile::SpawnImpactFx(
    const RTBEngine::Math::Vector3& center,
    float radius) const
{
    if (!bounceImpactPrefab) {
        return;
    }

    RTBEngine::Math::Vector3 spawnPosition = center;
    spawnPosition.y += 0.08f;

    RTBEngine::Scene::GameObject* spawned =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(
            *bounceImpactPrefab,
            spawnPosition,
            RTBEngine::Math::Quaternion::Identity());
    if (!spawned) {
        RTB_WARN("[BouncingBallProjectile] Failed to spawn bounce impact FX.");
        return;
    }

    const float scale = std::max(0.25f, radius / kImpactFxBaseRadius);
    spawned->GetTransform().SetScale(RTBEngine::Math::Vector3(scale, 1.0f, scale));
}

void BouncingBallProjectile::ApplyAreaDamage(
    const RTBEngine::Math::Vector3& center,
    float radius,
    const RTBEngine::Math::Vector3& hitDirection)
{
    if (!instigator || damage <= 0.0f || radius <= 0.0f) {
        return;
    }

    if (!CombatAuthority::CanApplyDamage(instigator)) {
        return;
    }

    RTBEngine::Physics::PhysicsWorld* world =
        physicsWorld ? physicsWorld : CharacterCombatUtils::ResolvePhysicsWorld(instigator);
    if (!world) {
        return;
    }

    CharacterCombatUtils::HostileSphereOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = world;
    overlapQuery.instigator = instigator;
    overlapQuery.center = center;
    overlapQuery.radius = radius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;
    overlapQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Characters");

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargetsInSphere(overlapQuery);

    RTBEngine::Math::Vector3 planarDir = hitDirection;
    planarDir.y = 0.0f;
    if (CharacterCombatUtils::HasPlanarDirection(planarDir)) {
        planarDir = CharacterCombatUtils::NormalizePlanarDirection(planarDir);
    } else {
        planarDir = RTBEngine::Math::Vector3::Forward();
    }

    for (const CharacterCombatUtils::HostileOverlapHit& hit : hits) {
        if (!hit.health || hit.health->IsDead() || !hit.targetRoot) {
            continue;
        }
        if (HasAlreadyHit(hit.health)) {
            continue;
        }

        hitTargets.push_back(hit.health);

        HealthComponent::DamageContext damageContext;
        damageContext.amount = damage;
        damageContext.instigator = instigator;
        damageContext.hitPoint = hit.hitPoint;
        damageContext.hitDirection = planarDir;
        damageContext.knockbackStrength = knockbackStrength;
        hit.health->TakeDamage(damage, damageContext);
    }
}

bool BouncingBallProjectile::TryHitWallAlongSegment(
    const RTBEngine::Math::Vector3& from,
    const RTBEngine::Math::Vector3& to,
    RTBEngine::Math::Vector3& outHitPoint) const
{
    RTBEngine::Physics::PhysicsWorld* world =
        physicsWorld ? physicsWorld : CharacterCombatUtils::ResolvePhysicsWorld(instigator);
    if (!world) {
        return false;
    }

    const RTBEngine::Math::Vector3 segment = to - from;
    if (segment.LengthSquared() <= 0.000001f) {
        return false;
    }

    RTBEngine::Physics::PhysicsQueryOptions options;
    options.ignoredObject = instigator;
    options.ignoreIgnoredObjectHierarchy = true;
    options.ignoreTriggers = true;
    options.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");

    RTBEngine::Physics::PhysicsQueryHit hit;
    if (!world->SphereCastClosest(from, to, contactRadius, hit, options)) {
        return false;
    }

    // Walkable ground is for bounces; only solid walls/ceilings stop the ball.
    if (hit.normal.y > kWalkableNormalY) {
        return false;
    }

    outHitPoint = hit.point;
    return true;
}

bool BouncingBallProjectile::TryHitEnemiesAlongSegment(
    const RTBEngine::Math::Vector3& from,
    const RTBEngine::Math::Vector3& to)
{
    if (!instigator || damage <= 0.0f) {
        return false;
    }

    if (!CombatAuthority::CanApplyDamage(instigator)) {
        return false;
    }

    RTBEngine::Physics::PhysicsWorld* world =
        physicsWorld ? physicsWorld : CharacterCombatUtils::ResolvePhysicsWorld(instigator);
    if (!world) {
        return false;
    }

    RTBEngine::Math::Vector3 segment = to - from;
    const float segmentLength = segment.Length();
    if (segmentLength <= 0.0001f) {
        return false;
    }

    CharacterCombatUtils::HostileOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = world;
    overlapQuery.instigator = instigator;
    overlapQuery.origin = from;
    overlapQuery.direction = segment * (1.0f / segmentLength);
    overlapQuery.distance = segmentLength;
    overlapQuery.radius = contactRadius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;
    overlapQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Characters");

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargets(overlapQuery);
    if (hits.empty()) {
        return false;
    }

    const CharacterCombatUtils::HostileOverlapHit& hit = hits.front();
    if (!hit.health || hit.health->IsDead()) {
        return false;
    }

    RTBEngine::Math::Vector3 planarDir = segment;
    planarDir.y = 0.0f;
    if (CharacterCombatUtils::HasPlanarDirection(planarDir)) {
        planarDir = CharacterCombatUtils::NormalizePlanarDirection(planarDir);
    } else {
        planarDir = RTBEngine::Math::Vector3::Forward();
    }

    const float aoeRadius = CurrentAoeRadius();
    ApplyAreaDamage(hit.hitPoint, aoeRadius, planarDir);
    SpawnImpactFx(hit.hitPoint, aoeRadius);

    if (owner) {
        owner->GetTransform().SetPosition(hit.hitPoint);
    }
    return true;
}

void BouncingBallProjectile::TriggerBounce(int bounceIndex)
{
    if (bounceIndex < 0 || bounceIndex >= path.bounceCount) {
        return;
    }

    const RTBEngine::Math::Vector3& bouncePoint =
        path.bouncePoints[static_cast<std::size_t>(bounceIndex)];
    const float radius = path.bounceRadii[static_cast<std::size_t>(bounceIndex)];

    RTBEngine::Math::Vector3 hitDirection = RTBEngine::Math::Vector3::Forward();
    if (bounceIndex > 0) {
        hitDirection =
            bouncePoint - path.bouncePoints[static_cast<std::size_t>(bounceIndex - 1)];
    } else if (!path.samples.empty()) {
        hitDirection = bouncePoint - path.samples.front();
    }

    SpawnImpactFx(bouncePoint, radius);
    ApplyAreaDamage(bouncePoint, radius, hitDirection);
}

void BouncingBallProjectile::FinishAndDestroy()
{
    if (finished) {
        return;
    }

    finished = true;
    launched = false;
    pendingDestroy = true;
    SetUpdateTickEnabled(false);
}

void BouncingBallProjectile::OnUpdate(float deltaTime)
{
    if (!launched || finished || pendingDestroy || deltaTime <= 0.0f) {
        return;
    }

    distanceTravelled = std::min(path.totalLength, distanceTravelled + speed * deltaTime);

    int sampleIndex = 0;
    const RTBEngine::Math::Vector3 nextPosition =
        BouncingBallTrajectory::EvaluateAtDistance(path, distanceTravelled, &sampleIndex);

    RTBEngine::Math::Vector3 wallHitPoint;
    if (TryHitWallAlongSegment(previousPosition, nextPosition, wallHitPoint)) {
        const float aoeRadius = CurrentAoeRadius();
        RTBEngine::Math::Vector3 hitDirection = nextPosition - previousPosition;
        ApplyAreaDamage(wallHitPoint, aoeRadius, hitDirection);
        SpawnImpactFx(wallHitPoint, aoeRadius);
        owner->GetTransform().SetPosition(wallHitPoint);
        FinishAndDestroy();
        return;
    }

    if (TryHitEnemiesAlongSegment(previousPosition, nextPosition)) {
        FinishAndDestroy();
        return;
    }

    owner->GetTransform().SetPosition(nextPosition);
    previousPosition = nextPosition;

    while (nextBounceIndex < path.bounceCount) {
        const int bounceSample =
            path.bounceSampleIndices[static_cast<std::size_t>(nextBounceIndex)];
        if (bounceSample < 0 ||
            bounceSample >= static_cast<int>(path.cumulativeLengths.size())) {
            break;
        }

        const float bounceDistance =
            path.cumulativeLengths[static_cast<std::size_t>(bounceSample)];
        if (distanceTravelled + 0.0001f < bounceDistance) {
            break;
        }

        TriggerBounce(nextBounceIndex);
        ++nextBounceIndex;
    }

    if (distanceTravelled >= path.totalLength - 0.0001f &&
        nextBounceIndex >= path.bounceCount) {
        FinishAndDestroy();
    }
}
