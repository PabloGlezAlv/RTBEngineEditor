#include "PlayerSpecialBouncingBallAttack.h"

#include "BouncingBallProjectile.h"
#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include "ThirdPersonCharacterController.h"

#include <algorithm>
#include <cmath>
#include <vector>

using ThisClass = PlayerSpecialBouncingBallAttack;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr int kCircleSegments = 40;
}

RTB_REGISTER_COMPONENT(PlayerSpecialBouncingBallAttack)
    RTB_PROPERTY_COMPONENT(pathPreviewTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(bouncePreviewTrail, TrailRenderer)
    RTB_PROPERTY_SERIALIZED(ballPrefabRef)
    RTB_PROPERTY_SERIALIZED_RANGE(maxRange, 1.0f, 25.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(minAimStrength, 0.05f, 0.95f)
    RTB_PROPERTY_SERIALIZED_RANGE(arcHeight, 0.2f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(ballSpeed, 1.0f, 40.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damage, 0.0f, 200.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(knockbackStrength, 0.0f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(contactRadius, 0.05f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(bounceRadius0, 0.25f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(bounceRadius1, 0.25f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(bounceRadius2, 0.25f, 8.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(launchHeightOffset, 0.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(launchForwardOffset, 0.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(groundSnapLift, -1.0f, 1.0f)
    RTB_PROPERTY_SERIALIZED(ignoreSameTeam)
    RTB_PROPERTY_SERIALIZED(bounceImpactPrefabRef)
RTB_END_REGISTER(PlayerSpecialBouncingBallAttack)

void PlayerSpecialBouncingBallAttack::ClampSettings()
{
    maxRange = std::max(1.0f, maxRange);
    minAimStrength = std::clamp(minAimStrength, 0.05f, 0.95f);
    arcHeight = std::max(0.2f, arcHeight);
    ballSpeed = std::max(1.0f, ballSpeed);
    damage = std::max(0.0f, damage);
    knockbackStrength = std::max(0.0f, knockbackStrength);
    contactRadius = std::max(0.05f, contactRadius);
    bounceRadius0 = std::max(0.25f, bounceRadius0);
    bounceRadius1 = std::max(0.25f, bounceRadius1);
    bounceRadius2 = std::max(0.25f, bounceRadius2);
    launchHeightOffset = std::max(0.0f, launchHeightOffset);
    launchForwardOffset = std::max(0.0f, launchForwardOffset);

    // Keep bounce radii strictly decreasing.
    bounceRadius1 = std::min(bounceRadius1, bounceRadius0 * 0.95f);
    bounceRadius2 = std::min(bounceRadius2, bounceRadius1 * 0.95f);
}

void PlayerSpecialBouncingBallAttack::ResolveBallPrefab()
{
    ballPrefab = nullptr;
    if (ballPrefabRef.empty()) {
        return;
    }

    const std::string poolKey = RTBEngine::Scene::ObjectPool::ResolvePoolKey(ballPrefabRef);
    ballPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(poolKey);
    if (!ballPrefab) {
        ballPrefab = RTBEngine::Scene::PrefabRegistry::GetInstance().Get(poolKey);
    }
}

void PlayerSpecialBouncingBallAttack::CacheGameplayReferences()
{
    controller = owner->GetComponent<ThirdPersonCharacterController>();
    ResolveBallPrefab();
}

void PlayerSpecialBouncingBallAttack::ValidateRequiredReferences() const
{
    if (!pathPreviewTrail) {
        RTB_WARN("[PlayerSpecialBouncingBallAttack] pathPreviewTrail is not assigned on '" +
                 owner->GetName() + "'.");
    }
    if (!bouncePreviewTrail) {
        RTB_WARN("[PlayerSpecialBouncingBallAttack] bouncePreviewTrail is not assigned on '" +
                 owner->GetName() + "'.");
    }
    if (ballPrefabRef.empty() || !ballPrefab) {
        RTB_WARN("[PlayerSpecialBouncingBallAttack] Failed to resolve ball prefab '" +
                 ballPrefabRef + "'.");
    }
}

void PlayerSpecialBouncingBallAttack::OnStart()
{
    ClampSettings();
    CacheGameplayReferences();
    ValidateRequiredReferences();
    HideAimPreview();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialBouncingBallAttack::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
}

void PlayerSpecialBouncingBallAttack::OnDestroy()
{
    ClearActiveProjectile();
    HideAimPreview();
    controller = nullptr;
    ballPrefab = nullptr;
}

void PlayerSpecialBouncingBallAttack::ClearActiveProjectile()
{
    if (activeProjectileObject) {
        RTBEngine::Scene::ObjectPool::GetInstance().Release(activeProjectileObject);
    }
    activeProjectile = nullptr;
    activeProjectileObject = nullptr;
    active = false;
    SetUpdateTickEnabled(false);
}

void PlayerSpecialBouncingBallAttack::OnUpdate(float /*deltaTime*/)
{
    if (!active) {
        return;
    }

    if (!activeProjectile || activeProjectile->HasFinished()) {
        ClearActiveProjectile();
    }
}

void PlayerSpecialBouncingBallAttack::ConfigurePreviewTrails() const
{
    if (pathPreviewTrail) {
        pathPreviewTrail->width = 0.26f;
        pathPreviewTrail->startWidth = 0.26f;
        pathPreviewTrail->endWidth = 0.16f;
        pathPreviewTrail->color = RTBEngine::Math::Vector4(0.72f, 0.45f, 1.00f, 0.78f);
        pathPreviewTrail->fadeAlphaAlongLength = false;
        pathPreviewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
        pathPreviewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
        pathPreviewTrail->softEdge = 0.18f;
        pathPreviewTrail->uvScrollSpeed = 0.0f;
        pathPreviewTrail->texture = nullptr;
        pathPreviewTrail->SetGlobalAlphaScale(1.0f);
        pathPreviewTrail->SetEnabled(true);
    }

    if (bouncePreviewTrail) {
        bouncePreviewTrail->width = 0.18f;
        bouncePreviewTrail->startWidth = 0.18f;
        bouncePreviewTrail->endWidth = 0.18f;
        bouncePreviewTrail->color = RTBEngine::Math::Vector4(0.95f, 0.70f, 1.00f, 0.80f);
        bouncePreviewTrail->fadeAlphaAlongLength = false;
        bouncePreviewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
        bouncePreviewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
        bouncePreviewTrail->softEdge = 0.20f;
        bouncePreviewTrail->uvScrollSpeed = 0.0f;
        bouncePreviewTrail->texture = nullptr;
        bouncePreviewTrail->SetGlobalAlphaScale(1.0f);
        bouncePreviewTrail->SetEnabled(true);
    }
}

void PlayerSpecialBouncingBallAttack::HidePreviewTrails() const
{
    if (pathPreviewTrail) {
        pathPreviewTrail->SetVisible(false);
        pathPreviewTrail->ClearPoints();
    }
    if (bouncePreviewTrail) {
        bouncePreviewTrail->SetVisible(false);
        bouncePreviewTrail->ClearPoints();
    }
}

void PlayerSpecialBouncingBallAttack::ShowPathPreview(
    const BouncingBallTrajectory::Path& path) const
{
    if (!pathPreviewTrail || path.samples.empty()) {
        return;
    }

    pathPreviewTrail->SetPoints(path.samples);
    pathPreviewTrail->SetVisible(true);
    if (RTBEngine::Scene::GameObject* trailOwner = pathPreviewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

void PlayerSpecialBouncingBallAttack::ShowBounceCirclesPreview(
    const BouncingBallTrajectory::Path& path) const
{
    if (!bouncePreviewTrail || path.bounceCount <= 0) {
        return;
    }

    std::vector<RTBEngine::Math::Vector3> points;
    points.reserve(static_cast<std::size_t>(path.bounceCount * (kCircleSegments + 2)));

    for (int bounceIndex = 0; bounceIndex < path.bounceCount; ++bounceIndex) {
        const RTBEngine::Math::Vector3& center =
            path.bouncePoints[static_cast<std::size_t>(bounceIndex)];
        const float radius = path.bounceRadii[static_cast<std::size_t>(bounceIndex)];

        for (int i = 0; i <= kCircleSegments; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(kCircleSegments);
            const float angle = t * (kPi * 2.0f);
            points.emplace_back(
                center.x + std::cos(angle) * radius,
                center.y + 0.04f,
                center.z + std::sin(angle) * radius);
        }
    }

    bouncePreviewTrail->SetPoints(points);
    bouncePreviewTrail->SetVisible(true);
    if (RTBEngine::Scene::GameObject* trailOwner = bouncePreviewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

RTBEngine::Math::Vector3 PlayerSpecialBouncingBallAttack::GetLaunchOrigin(
    const RTBEngine::Math::Vector3& direction) const
{
    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetFeetWorld(owner);
    origin.y += launchHeightOffset;
    origin = CharacterCombatOrigins::ApplyPlanarDirectionOffset(
        origin,
        direction,
        launchForwardOffset);
    return origin;
}

bool PlayerSpecialBouncingBallAttack::BuildAimPath(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength,
    BouncingBallTrajectory::Path& outPath) const
{
    if (!CharacterCombatUtils::HasPlanarDirection(direction)) {
        return false;
    }

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    const RTBEngine::Math::Vector3 planar =
        CharacterCombatUtils::NormalizePlanarDirection(direction);

    BouncingBallTrajectory::BuildParams params;
    params.origin = GetLaunchOrigin(planar);
    params.planarDirection = planar;
    params.totalRange = maxRange * strength;
    params.arcHeight = arcHeight;
    params.bounceRadius0 = bounceRadius0;
    params.bounceRadius1 = bounceRadius1;
    params.bounceRadius2 = bounceRadius2;
    params.groundSnapLift = groundSnapLift;
    params.physicsWorld = CharacterCombatUtils::ResolvePhysicsWorld(owner);
    params.ignoreObject = owner;

    return BouncingBallTrajectory::Build(params, outPath);
}

void PlayerSpecialBouncingBallAttack::UpdateAimPreview(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active) {
        HideAimPreview();
        return;
    }

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength || !CharacterCombatUtils::HasPlanarDirection(direction)) {
        HideAimPreview();
        return;
    }

    BouncingBallTrajectory::Path path;
    if (!BuildAimPath(direction, strength, path)) {
        HideAimPreview();
        return;
    }

    ConfigurePreviewTrails();
    ShowPathPreview(path);
    ShowBounceCirclesPreview(path);
    previewActive = true;
}

void PlayerSpecialBouncingBallAttack::HideAimPreview()
{
    previewActive = false;
    HidePreviewTrails();
}

void PlayerSpecialBouncingBallAttack::ApplyMovementLock(float deltaTime)
{
    if (!active || !CharacterCombatUtils::HasPlanarDirection(aimDirection)) {
        return;
    }

    if (controller) {
        controller->FaceTowardPlanarDirection(aimDirection, deltaTime);
    }
}

bool PlayerSpecialBouncingBallAttack::TryActivate(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active) {
        return false;
    }

    ClampSettings();
    CacheGameplayReferences();

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength || !CharacterCombatUtils::HasPlanarDirection(direction)) {
        return false;
    }

    if (!ballPrefab) {
        RTB_WARN("[PlayerSpecialBouncingBallAttack] Cannot activate: ball prefab missing.");
        return false;
    }

    BouncingBallTrajectory::Path path;
    if (!BuildAimPath(direction, strength, path)) {
        return false;
    }

    HideAimPreview();

    const RTBEngine::Math::Vector3 spawnPosition = path.samples.front();
    RTBEngine::Scene::GameObject* spawned =
        RTBEngine::Scene::SceneManager::GetInstance().Instantiate(
            *ballPrefab,
            spawnPosition,
            RTBEngine::Math::Quaternion::Identity());
    if (!spawned) {
        RTB_WARN("[PlayerSpecialBouncingBallAttack] Failed to spawn bouncing ball.");
        return false;
    }

    BouncingBallProjectile* projectile = spawned->GetComponent<BouncingBallProjectile>();
    if (!projectile) {
        RTB_WARN("[PlayerSpecialBouncingBallAttack] Spawned prefab missing BouncingBallProjectile.");
        RTBEngine::Scene::ObjectPool::GetInstance().Release(spawned);
        return false;
    }

    BouncingBallProjectile::LaunchConfig launchConfig;
    launchConfig.instigator = owner;
    launchConfig.physicsWorld = CharacterCombatUtils::ResolvePhysicsWorld(owner);
    launchConfig.path = path;
    launchConfig.speed = ballSpeed;
    launchConfig.damage = damage;
    launchConfig.knockbackStrength = knockbackStrength;
    launchConfig.contactRadius = contactRadius;
    launchConfig.ignoreSameTeam = ignoreSameTeam;
    launchConfig.bounceImpactPrefabRef = bounceImpactPrefabRef;
    projectile->Launch(launchConfig);

    aimDirection = CharacterCombatUtils::NormalizePlanarDirection(direction);
    activeProjectile = projectile;
    activeProjectileObject = spawned;
    active = true;
    SetUpdateTickEnabled(true);
    return true;
}
