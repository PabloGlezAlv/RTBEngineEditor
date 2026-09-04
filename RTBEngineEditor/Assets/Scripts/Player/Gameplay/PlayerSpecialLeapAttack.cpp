#include "PlayerSpecialLeapAttack.h"

#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "HealthComponent.h"
#include "StunReceiver.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Physics/RigidBody.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <cmath>
#include <vector>

using ThisClass = PlayerSpecialLeapAttack;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr int kCircleSegments = 48;
    constexpr int kArcSegments = 10;

    float SmoothStep01(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
}

RTB_REGISTER_COMPONENT(PlayerSpecialLeapAttack)
    RTB_PROPERTY_COMPONENT(jumpPathPreviewTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(impactPreviewTrail, TrailRenderer)
    RTB_PROPERTY_SERIALIZED_RANGE(maxRange, 1.0f, 20.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(minAimStrength, 0.05f, 0.95f)
    RTB_PROPERTY_SERIALIZED_RANGE(impactRadius, 0.5f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(leapDuration, 0.15f, 2.5f)
    RTB_PROPERTY_SERIALIZED_RANGE(leapHeight, 0.0f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damage, 0.0f, 200.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(stunDuration, 0.0f, 5.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(knockbackStrength, 0.0f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(castRadius, 0.05f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(trailForwardOffset, 0.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(trailHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(impactHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED(ignoreSameTeam)
    RTB_PROPERTY_SERIALIZED(landingAuraPrefabRef)
RTB_END_REGISTER(PlayerSpecialLeapAttack)

void PlayerSpecialLeapAttack::ClampSettings()
{
    maxRange = std::max(1.0f, maxRange);
    minAimStrength = std::clamp(minAimStrength, 0.05f, 0.95f);
    impactRadius = std::max(0.5f, impactRadius);
    leapDuration = std::max(0.15f, leapDuration);
    leapHeight = std::max(0.0f, leapHeight);
    damage = std::max(0.0f, damage);
    stunDuration = std::max(0.0f, stunDuration);
    knockbackStrength = std::max(0.0f, knockbackStrength);
    castRadius = std::max(0.05f, castRadius);
}

void PlayerSpecialLeapAttack::CacheGameplayReferences()
{
    controller = owner->GetComponent<ThirdPersonCharacterController>();
    physicsPose = CharacterCombatUtils::ResolveActorPhysicsPose(owner);
    ResolveLandingAuraPrefab();
}

void PlayerSpecialLeapAttack::ResolveLandingAuraPrefab()
{
    landingAuraPrefab = nullptr;
    if (landingAuraPrefabRef.empty()) {
        return;
    }

    const std::string poolKey =
        RTBEngine::Scene::ObjectPool::ResolvePoolKey(landingAuraPrefabRef);
    landingAuraPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(poolKey);
    if (!landingAuraPrefab) {
        landingAuraPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().Get(poolKey);
    }
}

void PlayerSpecialLeapAttack::ValidateRequiredReferences() const
{
    if (!jumpPathPreviewTrail) {
        RTB_WARN("[PlayerSpecialLeapAttack] jumpPathPreviewTrail is not assigned on '" +
                 owner->GetName() + "'.");
    }
    if (!impactPreviewTrail) {
        RTB_WARN("[PlayerSpecialLeapAttack] impactPreviewTrail is not assigned on '" +
                 owner->GetName() + "'.");
    }
    if (!landingAuraPrefabRef.empty() && !landingAuraPrefab) {
        RTB_WARN("[PlayerSpecialLeapAttack] Failed to resolve landing aura prefab '" +
                 landingAuraPrefabRef + "'.");
    }
}

void PlayerSpecialLeapAttack::OnStart()
{
    ClampSettings();
    CacheGameplayReferences();
    ValidateRequiredReferences();
    HideAimPreview();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialLeapAttack::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
}

void PlayerSpecialLeapAttack::OnDestroy()
{
    FinishLeap();
    HideAimPreview();
    controller = nullptr;
    physicsPose = {};
    landingAuraPrefab = nullptr;
}

void PlayerSpecialLeapAttack::ConfigurePreviewTrails() const
{
    if (jumpPathPreviewTrail) {
        jumpPathPreviewTrail->width = 0.28f;
        jumpPathPreviewTrail->startWidth = 0.28f;
        jumpPathPreviewTrail->endWidth = 0.18f;
        jumpPathPreviewTrail->color = RTBEngine::Math::Vector4(1.00f, 1.00f, 1.00f, 0.72f);
        jumpPathPreviewTrail->fadeAlphaAlongLength = false;
        jumpPathPreviewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
        jumpPathPreviewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
        jumpPathPreviewTrail->softEdge = 0.15f;
        jumpPathPreviewTrail->uvScrollSpeed = 0.0f;
        jumpPathPreviewTrail->texture = nullptr;
        jumpPathPreviewTrail->SetGlobalAlphaScale(1.0f);
        jumpPathPreviewTrail->SetEnabled(true);
    }

    if (impactPreviewTrail) {
        impactPreviewTrail->width = 0.22f;
        impactPreviewTrail->startWidth = 0.22f;
        impactPreviewTrail->endWidth = 0.22f;
        impactPreviewTrail->color = RTBEngine::Math::Vector4(1.00f, 0.92f, 0.35f, 0.85f);
        impactPreviewTrail->fadeAlphaAlongLength = false;
        impactPreviewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
        impactPreviewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
        impactPreviewTrail->softEdge = 0.2f;
        impactPreviewTrail->uvScrollSpeed = 0.0f;
        impactPreviewTrail->texture = nullptr;
        impactPreviewTrail->SetGlobalAlphaScale(1.0f);
        impactPreviewTrail->SetEnabled(true);
    }
}

void PlayerSpecialLeapAttack::ShowJumpPathPreview(
    const RTBEngine::Math::Vector3& start,
    const RTBEngine::Math::Vector3& end) const
{
    if (!jumpPathPreviewTrail) {
        return;
    }

    std::vector<RTBEngine::Math::Vector3> points;
    points.reserve(static_cast<std::size_t>(kArcSegments) + 1);
    for (int i = 0; i <= kArcSegments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(kArcSegments);
        RTBEngine::Math::Vector3 point = start + (end - start) * t;
        point.y = start.y + leapHeight * 4.0f * t * (1.0f - t);
        points.push_back(point);
    }

    jumpPathPreviewTrail->SetPoints(points);
    jumpPathPreviewTrail->SetVisible(true);
    if (RTBEngine::Scene::GameObject* trailOwner = jumpPathPreviewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

void PlayerSpecialLeapAttack::ShowImpactCirclePreview(const RTBEngine::Math::Vector3& center) const
{
    if (!impactPreviewTrail) {
        return;
    }

    std::vector<RTBEngine::Math::Vector3> points;
    points.reserve(static_cast<std::size_t>(kCircleSegments) + 1);
    for (int i = 0; i <= kCircleSegments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(kCircleSegments);
        const float angle = t * (kPi * 2.0f);
        points.emplace_back(
            center.x + std::cos(angle) * impactRadius,
            center.y,
            center.z + std::sin(angle) * impactRadius);
    }

    impactPreviewTrail->SetPoints(points);
    impactPreviewTrail->SetVisible(true);
    if (RTBEngine::Scene::GameObject* trailOwner = impactPreviewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

void PlayerSpecialLeapAttack::HidePreviewTrails() const
{
    if (jumpPathPreviewTrail) {
        jumpPathPreviewTrail->SetVisible(false);
        jumpPathPreviewTrail->ClearPoints();
    }
    if (impactPreviewTrail) {
        impactPreviewTrail->SetVisible(false);
        impactPreviewTrail->ClearPoints();
    }
}

RTBEngine::Math::Vector3 PlayerSpecialLeapAttack::GetFeetOrigin(
    const RTBEngine::Math::Vector3& /*direction*/) const
{
    // Fixed origin at feet: do not push forward or the aim length looks inconsistent.
    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetFeetWorld(owner, physicsPose.collider);
    origin.y += trailHeightOffset;
    return origin;
}

void PlayerSpecialLeapAttack::SnapLandingToGround(RTBEngine::Math::Vector3& feetPosition) const
{
    RTBEngine::Physics::PhysicsWorld* physicsWorld = physicsPose.physicsWorld
        ? physicsPose.physicsWorld
        : CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return;
    }

    constexpr float kProbeUp = 4.0f;
    constexpr float kProbeDown = 30.0f;

    RTBEngine::Physics::PhysicsQueryOptions options;
    options.ignoredObject = owner;
    options.ignoreIgnoredObjectHierarchy = true;
    options.ignoreTriggers = true;
    options.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");

    const RTBEngine::Math::Vector3 castStart(
        feetPosition.x,
        feetPosition.y + kProbeUp,
        feetPosition.z);
    const RTBEngine::Math::Vector3 castEnd(
        feetPosition.x,
        feetPosition.y - kProbeDown,
        feetPosition.z);

    RTBEngine::Physics::PhysicsQueryHit hit;
    if (!physicsWorld->RaycastClosest(castStart, castEnd, hit, options)) {
        return;
    }

    // Only snap to walkable ground; walls/ceilings are ignored for landing height.
    if (hit.normal.y <= 0.65f) {
        return;
    }

    feetPosition.y = hit.point.y + trailHeightOffset;
}

void PlayerSpecialLeapAttack::ResolveLeapEndpoints(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength,
    RTBEngine::Math::Vector3& outFeetStart,
    RTBEngine::Math::Vector3& outFeetEnd,
    RTBEngine::Math::Vector3& outRootStart,
    RTBEngine::Math::Vector3& outRootEnd) const
{
    const RTBEngine::Math::Vector3 planar =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    outFeetStart = GetFeetOrigin(planar);

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    const float distance = maxRange * strength;

    // Distance scales with aim pull. Walls ignored; only ground height is sampled.
    outFeetEnd = outFeetStart + planar * distance;
    outFeetEnd.y = outFeetStart.y;
    SnapLandingToGround(outFeetEnd);

    outRootStart = owner->GetWorldPosition();
    const RTBEngine::Math::Vector3 rootToFeet = outFeetStart - outRootStart;
    outRootEnd = outFeetEnd - rootToFeet;
}

void PlayerSpecialLeapAttack::UpdateAimPreview(
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

    const RTBEngine::Math::Vector3 planar =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    if (!CharacterCombatUtils::HasPlanarDirection(planar)) {
        HideAimPreview();
        return;
    }

    ClampSettings();
    ConfigurePreviewTrails();

    RTBEngine::Math::Vector3 feetStart;
    RTBEngine::Math::Vector3 feetEnd;
    RTBEngine::Math::Vector3 rootStart;
    RTBEngine::Math::Vector3 rootEnd;
    ResolveLeapEndpoints(planar, strength, feetStart, feetEnd, rootStart, rootEnd);

    ShowJumpPathPreview(feetStart, feetEnd);
    ShowImpactCirclePreview(feetEnd);
    previewActive = true;
}

void PlayerSpecialLeapAttack::HideAimPreview()
{
    previewActive = false;
    HidePreviewTrails();
}

void PlayerSpecialLeapAttack::ApplyMovementLock(float deltaTime)
{
    if (!active || !CharacterCombatUtils::HasPlanarDirection(leapDirection)) {
        return;
    }

    if (controller) {
        controller->FaceTowardPlanarDirection(leapDirection, deltaTime);
    }
}

void PlayerSpecialLeapAttack::SetActorWorldPosition(const RTBEngine::Math::Vector3& position)
{
    CharacterCombatUtils::SetActorWorldPosition(
        owner,
        position,
        owner->GetTransform().GetRotation(),
        &physicsPose);
}

void PlayerSpecialLeapAttack::ApplyLeapPose(float normalizedT)
{
    const float t = SmoothStep01(normalizedT);
    RTBEngine::Math::Vector3 position = leapStart + (leapEnd - leapStart) * t;
    position.y = leapStart.y + leapHeight * 4.0f * normalizedT * (1.0f - normalizedT);
    SetActorWorldPosition(position);
}

bool PlayerSpecialLeapAttack::TryActivate(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active) {
        return false;
    }

    ClampSettings();
    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength) {
        return false;
    }

    const RTBEngine::Math::Vector3 planar =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    if (!CharacterCombatUtils::HasPlanarDirection(planar)) {
        return false;
    }

    HideAimPreview();
    CacheGameplayReferences();

    RTBEngine::Math::Vector3 feetStart;
    RTBEngine::Math::Vector3 feetEnd;
    ResolveLeapEndpoints(planar, strength, feetStart, feetEnd, leapStart, leapEnd);
    if ((leapEnd - leapStart).Length() < 0.35f) {
        return false;
    }

    leapDirection = planar;
    impactApplied = false;
    leapImpactCenter = feetEnd;
    elapsed = 0.0f;
    active = true;
    SetEnabled(true);
    SetUpdateTickEnabled(true);
    ApplyLeapPose(0.0f);
    return true;
}

void PlayerSpecialLeapAttack::SpawnLandingAura() const
{
    if (!landingAuraPrefab) {
        return;
    }

    RTBEngine::Math::Vector3 spawnPosition = leapImpactCenter;
    spawnPosition.y += 0.08f;

    RTBEngine::Scene::GameObject* spawned =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(
            *landingAuraPrefab,
            spawnPosition,
            RTBEngine::Math::Quaternion::Identity());

    if (!spawned) {
        RTB_WARN("[PlayerSpecialLeapAttack] Failed to spawn landing aura prefab.");
    }
}

void PlayerSpecialLeapAttack::ApplyLandingImpact()
{
    if (impactApplied) {
        return;
    }
    impactApplied = true;

    SpawnLandingAura();

    if (damage <= 0.0f) {
        return;
    }

    if (!CombatAuthority::CanApplyDamage(owner)) {
        return;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld = physicsPose.physicsWorld
        ? physicsPose.physicsWorld
        : CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return;
    }

    RTBEngine::Math::Vector3 center = leapImpactCenter;
    center.y += impactHeightOffset;

    CharacterCombatUtils::HostileSphereOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = physicsWorld;
    overlapQuery.instigator = owner;
    overlapQuery.center = center;
    overlapQuery.radius = impactRadius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;
    overlapQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Characters");

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargetsInSphere(overlapQuery);

    for (const CharacterCombatUtils::HostileOverlapHit& hit : hits) {
        if (!hit.health || hit.health->IsDead() || !hit.targetRoot) {
            continue;
        }

        HealthComponent::DamageContext damageContext;
        damageContext.amount = damage;
        damageContext.instigator = owner;
        damageContext.hitPoint = hit.hitPoint;
        damageContext.hitDirection = leapDirection;
        damageContext.knockbackStrength = knockbackStrength;
        hit.health->TakeDamage(damage, damageContext);

        ApplyStunTo(hit.targetRoot, stunDuration);

        if (ThirdPersonCharacterController* targetController =
                hit.targetRoot->GetComponent<ThirdPersonCharacterController>()) {
            targetController->AddPlanarKnockback(leapDirection, knockbackStrength);
        }
    }
}

void PlayerSpecialLeapAttack::FinishLeap()
{
    if (!active) {
        SetUpdateTickEnabled(false);
        return;
    }

    active = false;
    elapsed = 0.0f;
    leapDirection = RTBEngine::Math::Vector3::Zero();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialLeapAttack::OnUpdate(float deltaTime)
{
    if (!active) {
        FinishLeap();
        return;
    }

    elapsed += std::max(0.0f, deltaTime);
    const float t = std::clamp(elapsed / leapDuration, 0.0f, 1.0f);
    ApplyLeapPose(t);

    if (t >= 1.0f) {
        SetActorWorldPosition(leapEnd);
        ApplyLandingImpact();
        FinishLeap();
    }
}

void PlayerSpecialLeapAttack::OnLateUpdate(float /*deltaTime*/)
{
    if (!active) {
        return;
    }

    const float t = std::clamp(elapsed / leapDuration, 0.0f, 1.0f);
    ApplyLeapPose(t);
}
