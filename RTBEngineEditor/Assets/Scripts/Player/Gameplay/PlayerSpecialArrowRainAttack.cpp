#include "PlayerSpecialArrowRainAttack.h"

#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "HealthComponent.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

using ThisClass = PlayerSpecialArrowRainAttack;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr int kCircleSegments = 48;
    constexpr float kDegToRad = kPi / 180.0f;

    float Random01()
    {
        return static_cast<float>(std::rand() % 10001) / 10000.0f;
    }
}

RTB_REGISTER_COMPONENT(PlayerSpecialArrowRainAttack)
    RTB_PROPERTY_COMPONENT(pathPreviewTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(areaPreviewTrail, TrailRenderer)
    RTB_PROPERTY_SERIALIZED(arrowVisualPrefabRef)
    RTB_PROPERTY_SERIALIZED(impactAuraPrefabRef)
    RTB_PROPERTY_SERIALIZED_RANGE(maxRange, 1.0f, 25.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(minAimStrength, 0.05f, 0.95f)
    RTB_PROPERTY_SERIALIZED_RANGE(rainRadius, 0.5f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(rainDuration, 0.2f, 5.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(impactDelay, 0.0f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(tickInterval, 0.05f, 1.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damagePerTick, 0.0f, 100.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(knockbackStrength, 0.0f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(arrowCount, 1, 40)
    RTB_PROPERTY_SERIALIZED_RANGE(arrowFallHeight, 1.0f, 20.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(arrowFallDuration, 0.1f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(castRadius, 0.05f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(trailHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(impactHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED(ignoreSameTeam)
RTB_END_REGISTER(PlayerSpecialArrowRainAttack)

void PlayerSpecialArrowRainAttack::ClampSettings()
{
    maxRange = std::max(1.0f, maxRange);
    minAimStrength = std::clamp(minAimStrength, 0.05f, 0.95f);
    rainRadius = std::max(0.5f, rainRadius);
    rainDuration = std::max(0.2f, rainDuration);
    impactDelay = std::clamp(impactDelay, 0.0f, rainDuration);
    tickInterval = std::max(0.05f, tickInterval);
    damagePerTick = std::max(0.0f, damagePerTick);
    knockbackStrength = std::max(0.0f, knockbackStrength);
    arrowCount = std::clamp(arrowCount, 1, 40);
    arrowFallHeight = std::max(1.0f, arrowFallHeight);
    arrowFallDuration = std::max(0.1f, arrowFallDuration);
    castRadius = std::max(0.05f, castRadius);
}

void PlayerSpecialArrowRainAttack::ResolvePrefabs()
{
    arrowVisualPrefab = nullptr;
    impactAuraPrefab = nullptr;

    if (!arrowVisualPrefabRef.empty()) {
        const std::string key =
            RTBEngine::Scene::ObjectPool::ResolvePoolKey(arrowVisualPrefabRef);
        arrowVisualPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(key);
        if (!arrowVisualPrefab) {
            arrowVisualPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().Get(key);
        }
    }

    if (!impactAuraPrefabRef.empty()) {
        const std::string key =
            RTBEngine::Scene::ObjectPool::ResolvePoolKey(impactAuraPrefabRef);
        impactAuraPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(key);
        if (!impactAuraPrefab) {
            impactAuraPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().Get(key);
        }
    }
}

void PlayerSpecialArrowRainAttack::CacheGameplayReferences()
{
    controller = owner->GetComponent<ThirdPersonCharacterController>();
    colliderBody = CharacterCombatOrigins::ResolveColliderBody(owner);
    ResolvePrefabs();
}

void PlayerSpecialArrowRainAttack::OnStart()
{
    ClampSettings();
    CacheGameplayReferences();
    HideAimPreview();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialArrowRainAttack::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
}

void PlayerSpecialArrowRainAttack::OnDestroy()
{
    FinishRain();
    HideAimPreview();
    controller = nullptr;
    arrowVisualPrefab = nullptr;
    impactAuraPrefab = nullptr;
}

void PlayerSpecialArrowRainAttack::ConfigurePreviewTrails() const
{
    if (pathPreviewTrail) {
        pathPreviewTrail->SetVisible(false);
        pathPreviewTrail->ClearPoints();
    }

    if (areaPreviewTrail) {
        areaPreviewTrail->width = 0.20f;
        areaPreviewTrail->startWidth = 0.20f;
        areaPreviewTrail->endWidth = 0.20f;
        areaPreviewTrail->color = RTBEngine::Math::Vector4(0.35f, 0.80f, 1.00f, 0.88f);
        areaPreviewTrail->fadeAlphaAlongLength = false;
        areaPreviewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
        areaPreviewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
        areaPreviewTrail->softEdge = 0.20f;
        areaPreviewTrail->uvScrollSpeed = 0.0f;
        areaPreviewTrail->texture = nullptr;
        areaPreviewTrail->SetGlobalAlphaScale(1.0f);
        areaPreviewTrail->SetEnabled(true);
    }
}

void PlayerSpecialArrowRainAttack::HidePreviewTrails() const
{
    if (pathPreviewTrail) {
        pathPreviewTrail->SetVisible(false);
        pathPreviewTrail->ClearPoints();
    }
    if (areaPreviewTrail) {
        areaPreviewTrail->SetVisible(false);
        areaPreviewTrail->ClearPoints();
    }
}

void PlayerSpecialArrowRainAttack::ShowAreaCirclePreview(
    const RTBEngine::Math::Vector3& center) const
{
    if (!areaPreviewTrail) {
        return;
    }

    std::vector<RTBEngine::Math::Vector3> points;
    points.reserve(static_cast<std::size_t>(kCircleSegments) + 1);
    for (int i = 0; i <= kCircleSegments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(kCircleSegments);
        const float angle = t * (kPi * 2.0f);
        points.emplace_back(
            center.x + std::cos(angle) * rainRadius,
            center.y + 0.04f,
            center.z + std::sin(angle) * rainRadius);
    }

    areaPreviewTrail->SetPoints(points);
    areaPreviewTrail->SetVisible(true);
    if (RTBEngine::Scene::GameObject* trailOwner = areaPreviewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

void PlayerSpecialArrowRainAttack::SnapToGround(RTBEngine::Math::Vector3& position) const
{
    RTBEngine::Physics::PhysicsWorld* physicsWorld =
        CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return;
    }

    RTBEngine::Physics::PhysicsQueryOptions options;
    options.ignoredObject = owner;
    options.ignoreIgnoredObjectHierarchy = true;
    options.ignoreTriggers = true;
    options.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");

    const RTBEngine::Math::Vector3 castStart(position.x, position.y + 4.0f, position.z);
    const RTBEngine::Math::Vector3 castEnd(position.x, position.y - 30.0f, position.z);
    RTBEngine::Physics::PhysicsQueryHit hit;
    if (!physicsWorld->RaycastClosest(castStart, castEnd, hit, options)) {
        return;
    }
    if (hit.normal.y <= 0.65f) {
        return;
    }
    position.y = hit.point.y + trailHeightOffset;
}

bool PlayerSpecialArrowRainAttack::ResolveRainCenter(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength,
    RTBEngine::Math::Vector3& outCenter) const
{
    if (!CharacterCombatUtils::HasPlanarDirection(direction)) {
        return false;
    }

    const RTBEngine::Math::Vector3 planar =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);

    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetFeetWorld(owner, colliderBody);
    origin.y += trailHeightOffset;

    float distance = maxRange * strength;

    // Arrow rain can target through walls; only ground height is snapped.
    outCenter = origin + planar * distance;
    outCenter.y = origin.y;
    SnapToGround(outCenter);
    return distance >= 0.35f;
}

void PlayerSpecialArrowRainAttack::UpdateAimPreview(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active) {
        HideAimPreview();
        return;
    }

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength) {
        HideAimPreview();
        return;
    }

    RTBEngine::Math::Vector3 center;
    if (!ResolveRainCenter(direction, strength, center)) {
        HideAimPreview();
        return;
    }

    ConfigurePreviewTrails();
    ShowAreaCirclePreview(center);
    previewActive = true;
}

void PlayerSpecialArrowRainAttack::HideAimPreview()
{
    previewActive = false;
    HidePreviewTrails();
}

void PlayerSpecialArrowRainAttack::ApplyMovementLock(float deltaTime)
{
    if (!active || !CharacterCombatUtils::HasPlanarDirection(aimDirection)) {
        return;
    }
    if (controller) {
        controller->FaceTowardPlanarDirection(aimDirection, deltaTime);
    }
}

RTBEngine::Math::Vector3 PlayerSpecialArrowRainAttack::SamplePointInRainCircle() const
{
    const float angle = Random01() * kPi * 2.0f;
    const float radius = rainRadius * std::sqrt(Random01());
    return RTBEngine::Math::Vector3(
        rainCenter.x + std::cos(angle) * radius,
        rainCenter.y,
        rainCenter.z + std::sin(angle) * radius);
}

void PlayerSpecialArrowRainAttack::SpawnImpactAura() const
{
    if (!impactAuraPrefab) {
        return;
    }

    RTBEngine::Math::Vector3 spawnPosition = rainCenter;
    spawnPosition.y += 0.08f;
    RTBEngine::Scene::GameObject* spawned =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(
            *impactAuraPrefab,
            spawnPosition,
            RTBEngine::Math::Quaternion::Identity());
    if (!spawned) {
        return;
    }

    const float scale = std::max(0.35f, rainRadius / 1.0f);
    spawned->GetTransform().SetScale(RTBEngine::Math::Vector3(scale, 1.0f, scale));
}

void PlayerSpecialArrowRainAttack::SpawnAllFallingArrows()
{
    if (!arrowVisualPrefab || arrowsSpawned) {
        return;
    }

    fallingArrows.clear();
    fallingArrows.reserve(static_cast<std::size_t>(arrowCount));

    for (int i = 0; i < arrowCount; ++i) {
        const RTBEngine::Math::Vector3 end = SamplePointInRainCircle();
        RTBEngine::Math::Vector3 start = end;
        start.y += arrowFallHeight;

        const RTBEngine::Math::Quaternion rotation =
            RTBEngine::Math::Quaternion::FromEulerAngles(
                -90.0f * kDegToRad,
                Random01() * kPi * 2.0f,
                0.0f);

        RTBEngine::Scene::GameObject* spawned =
            RTBEngine::Scene::SceneManager::GetInstance().Instantiate(
                *arrowVisualPrefab,
                start,
                rotation);
        if (!spawned) {
            continue;
        }

        FallingArrow arrow;
        arrow.object = spawned;
        arrow.start = start;
        arrow.end = end;
        arrow.age = 0.0f;
        arrow.duration = arrowFallDuration;
        arrow.finished = false;
        fallingArrows.push_back(arrow);
    }

    arrowsSpawned = true;
}

void PlayerSpecialArrowRainAttack::UpdateFallingArrows(float deltaTime)
{
    for (FallingArrow& arrow : fallingArrows) {
        if (arrow.finished || !arrow.object) {
            continue;
        }

        arrow.age += deltaTime;
        const float t = std::clamp(arrow.age / std::max(0.05f, arrow.duration), 0.0f, 1.0f);
        const RTBEngine::Math::Vector3 pos = arrow.start + (arrow.end - arrow.start) * t;
        arrow.object->GetTransform().SetPosition(pos);

        if (t >= 1.0f) {
            arrow.finished = true;
            RTBEngine::Scene::ObjectPool::GetInstance().Release(arrow.object);
            arrow.object = nullptr;
        }
    }
}

void PlayerSpecialArrowRainAttack::ClearFallingArrows()
{
    for (FallingArrow& arrow : fallingArrows) {
        if (arrow.object) {
            RTBEngine::Scene::ObjectPool::GetInstance().Release(arrow.object);
            arrow.object = nullptr;
        }
    }
    fallingArrows.clear();
}

void PlayerSpecialArrowRainAttack::ApplyRainDamageTick()
{
    if (damagePerTick <= 0.0f) {
        return;
    }
    if (!CombatAuthority::CanApplyDamage(owner)) {
        return;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld =
        CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return;
    }

    RTBEngine::Math::Vector3 center = rainCenter;
    center.y += impactHeightOffset;

    CharacterCombatUtils::HostileSphereOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = physicsWorld;
    overlapQuery.instigator = owner;
    overlapQuery.center = center;
    overlapQuery.radius = rainRadius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;
    overlapQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Characters");

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargetsInSphere(overlapQuery);

    for (const CharacterCombatUtils::HostileOverlapHit& hit : hits) {
        if (!hit.health || hit.health->IsDead() || !hit.targetRoot) {
            continue;
        }

        HealthComponent::DamageContext damageContext;
        damageContext.amount = damagePerTick;
        damageContext.instigator = owner;
        damageContext.hitPoint = hit.hitPoint;
        damageContext.hitDirection = aimDirection;
        damageContext.knockbackStrength = knockbackStrength;
        hit.health->TakeDamage(damagePerTick, damageContext);
    }
}

void PlayerSpecialArrowRainAttack::FinishRain()
{
    ClearFallingArrows();
    active = false;
    impactAuraSpawned = false;
    arrowsSpawned = false;
    elapsed = 0.0f;
    tickTimer = 0.0f;
    SetUpdateTickEnabled(false);
}

bool PlayerSpecialArrowRainAttack::TryActivate(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active) {
        return false;
    }

    ClampSettings();
    CacheGameplayReferences();

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength) {
        return false;
    }

    RTBEngine::Math::Vector3 center;
    if (!ResolveRainCenter(direction, strength, center)) {
        return false;
    }

    HideAimPreview();
    aimDirection = CharacterCombatUtils::NormalizePlanarDirection(direction);
    rainCenter = center;
    elapsed = 0.0f;
    tickTimer = 0.0f;
    impactAuraSpawned = false;
    arrowsSpawned = false;
    ClearFallingArrows();
    SpawnAllFallingArrows();
    active = true;
    SetUpdateTickEnabled(true);
    return true;
}

void PlayerSpecialArrowRainAttack::OnUpdate(float deltaTime)
{
    if (!active || deltaTime <= 0.0f) {
        return;
    }

    elapsed += deltaTime;
    UpdateFallingArrows(deltaTime);

    if (elapsed >= impactDelay) {
        if (!impactAuraSpawned) {
            SpawnImpactAura();
            impactAuraSpawned = true;
            ApplyRainDamageTick();
            tickTimer = 0.0f;
        } else {
            tickTimer += deltaTime;
            while (tickTimer >= tickInterval && elapsed < rainDuration) {
                tickTimer -= tickInterval;
                ApplyRainDamageTick();
            }
        }
    }

    if (elapsed >= rainDuration) {
        FinishRain();
    }
}
