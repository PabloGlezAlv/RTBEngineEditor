#include "PlayerSpecialDashAttack.h"

#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Physics/RigidBody.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <cmath>
#include <vector>

using ThisClass = PlayerSpecialDashAttack;

namespace {
    float SmoothStep01(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
}

RTB_REGISTER_COMPONENT(PlayerSpecialDashAttack)
    RTB_PROPERTY_COMPONENT(pathPreviewTrail, TrailRenderer)
    RTB_PROPERTY_SERIALIZED_RANGE(maxRange, 1.0f, 10.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(minAimStrength, 0.05f, 0.95f)
    RTB_PROPERTY_SERIALIZED_RANGE(dashDuration, 0.05f, 0.60f)
    RTB_PROPERTY_SERIALIZED_RANGE(damage, 0.0f, 200.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(knockbackStrength, 0.0f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(hitRadius, 0.1f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(castRadius, 0.05f, 2.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(trailHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damageHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED(ignoreSameTeam)
RTB_END_REGISTER(PlayerSpecialDashAttack)

void PlayerSpecialDashAttack::ClampSettings()
{
    maxRange = std::max(1.0f, maxRange);
    minAimStrength = std::clamp(minAimStrength, 0.05f, 0.95f);
    dashDuration = std::max(0.05f, dashDuration);
    damage = std::max(0.0f, damage);
    knockbackStrength = std::max(0.0f, knockbackStrength);
    hitRadius = std::max(0.1f, hitRadius);
    castRadius = std::max(0.05f, castRadius);
}

void PlayerSpecialDashAttack::CacheGameplayReferences()
{
    controller = owner ? owner->GetComponent<ThirdPersonCharacterController>() : nullptr;
    rigidBodyComponent = owner ? owner->GetComponent<RTBEngine::Scene::RigidBodyComponent>() : nullptr;
}

void PlayerSpecialDashAttack::ValidateRequiredReferences() const
{
    if (!owner) {
        return;
    }
    if (!pathPreviewTrail) {
        RTB_WARN("[PlayerSpecialDashAttack] pathPreviewTrail missing on '" +
                 owner->GetName() + "'.");
    }
}

void PlayerSpecialDashAttack::OnStart()
{
    ClampSettings();
    CacheGameplayReferences();
    ValidateRequiredReferences();
    HideAimPreview();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialDashAttack::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
}

void PlayerSpecialDashAttack::OnDestroy()
{
    FinishDash();
    HideAimPreview();
    controller = nullptr;
    rigidBodyComponent = nullptr;
    hitTargets.clear();
}

void PlayerSpecialDashAttack::ConfigurePreviewTrail() const
{
    if (!pathPreviewTrail) {
        return;
    }

    pathPreviewTrail->width = 0.28f;
    pathPreviewTrail->startWidth = 0.28f;
    pathPreviewTrail->endWidth = 0.16f;
    pathPreviewTrail->color = RTBEngine::Math::Vector4(0.85f, 0.95f, 1.00f, 0.82f);
    pathPreviewTrail->fadeAlphaAlongLength = false;
    pathPreviewTrail->blendMode = RTBEngine::Scene::TrailBlendMode::Alpha;
    pathPreviewTrail->alignment = RTBEngine::Scene::TrailAlignment::FlatXZ;
    pathPreviewTrail->softEdge = 0.18f;
    pathPreviewTrail->uvScrollSpeed = 0.0f;
    pathPreviewTrail->texture = nullptr;
    pathPreviewTrail->SetGlobalAlphaScale(1.0f);
    pathPreviewTrail->SetEnabled(true);
}

void PlayerSpecialDashAttack::HidePreviewTrail() const
{
    if (pathPreviewTrail) {
        pathPreviewTrail->SetVisible(false);
        pathPreviewTrail->ClearPoints();
    }
}

void PlayerSpecialDashAttack::ShowPathPreview(
    const RTBEngine::Math::Vector3& start,
    const RTBEngine::Math::Vector3& end) const
{
    if (!pathPreviewTrail) {
        return;
    }

    std::vector<RTBEngine::Math::Vector3> points;
    points.push_back(start);
    points.push_back(end);
    pathPreviewTrail->SetPoints(points);
    pathPreviewTrail->SetVisible(true);
    if (RTBEngine::Scene::GameObject* trailOwner = pathPreviewTrail->GetOwner()) {
        trailOwner->SetActive(true);
    }
}

bool PlayerSpecialDashAttack::ResolveDashEndpoints(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength,
    RTBEngine::Math::Vector3& outRootStart,
    RTBEngine::Math::Vector3& outRootEnd) const
{
    if (!owner || !CharacterCombatUtils::HasPlanarDirection(direction)) {
        return false;
    }

    const RTBEngine::Math::Vector3 planar =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);

    outRootStart = owner->GetWorldPosition();
    SnapRootToGround(outRootStart);

    float distance = maxRange * strength;
    CharacterCombatUtils::PlanarEnvironmentClipQuery clipQuery;
    clipQuery.physicsWorld = CharacterCombatUtils::ResolvePhysicsWorld(owner);
    clipQuery.instigator = owner;
    clipQuery.origin = CharacterCombatOrigins::GetCapsuleCenterWorld(owner);
    clipQuery.direction = planar;
    clipQuery.maxLength = distance;
    clipQuery.castRadius = castRadius;
    clipQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");
    distance = CharacterCombatUtils::ResolvePlanarEnvironmentClipLength(clipQuery);

    outRootEnd = outRootStart + planar * distance;
    SnapRootToGround(outRootEnd);
    return distance >= 0.40f;
}

void PlayerSpecialDashAttack::SnapRootToGround(RTBEngine::Math::Vector3& rootPosition) const
{
    if (!owner) {
        return;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld =
        CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return;
    }

    const RTBEngine::Math::Vector3 currentRoot = owner->GetWorldPosition();
    const RTBEngine::Math::Vector3 currentFeet = CharacterCombatOrigins::GetFeetWorld(owner);
    const float rootAboveFeet = currentRoot.y - currentFeet.y;

    constexpr float kProbeUp = 4.0f;
    constexpr float kProbeDown = 30.0f;

    RTBEngine::Physics::PhysicsQueryOptions options;
    options.ignoredObject = owner;
    options.ignoreIgnoredObjectHierarchy = true;
    options.ignoreTriggers = true;
    options.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");

    const RTBEngine::Math::Vector3 castStart(
        rootPosition.x,
        rootPosition.y + kProbeUp,
        rootPosition.z);
    const RTBEngine::Math::Vector3 castEnd(
        rootPosition.x,
        rootPosition.y - kProbeDown,
        rootPosition.z);

    RTBEngine::Physics::PhysicsQueryHit hit;
    if (!physicsWorld->RaycastClosest(castStart, castEnd, hit, options)) {
        return;
    }

    if (hit.normal.y <= 0.65f) {
        return;
    }

    // Keep the same root-to-feet offset so the capsule stays planted on the floor.
    rootPosition.y = hit.point.y + std::max(0.05f, rootAboveFeet);
}

void PlayerSpecialDashAttack::ClearMotionVelocity() const
{
    if (!rigidBodyComponent || !rigidBodyComponent->HasRigidBody()) {
        return;
    }

    if (RTBEngine::Physics::RigidBody* rigidBody = rigidBodyComponent->GetRigidBody()) {
        // Zero XZ and Y: residual vertical velocity while moving caused post-dash falls.
        rigidBody->SetLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
    }
}

void PlayerSpecialDashAttack::UpdateAimPreview(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active || !owner) {
        HideAimPreview();
        return;
    }

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength) {
        HideAimPreview();
        return;
    }

    RTBEngine::Math::Vector3 rootStart;
    RTBEngine::Math::Vector3 rootEnd;
    if (!ResolveDashEndpoints(direction, strength, rootStart, rootEnd)) {
        HideAimPreview();
        return;
    }

    ConfigurePreviewTrail();
    RTBEngine::Math::Vector3 previewStart = rootStart;
    RTBEngine::Math::Vector3 previewEnd = rootEnd;
    previewStart.y += trailHeightOffset;
    previewEnd.y += trailHeightOffset;
    ShowPathPreview(previewStart, previewEnd);
    previewActive = true;
}

void PlayerSpecialDashAttack::HideAimPreview()
{
    previewActive = false;
    HidePreviewTrail();
}

void PlayerSpecialDashAttack::ApplyMovementLock(float deltaTime)
{
    if (!active || !owner || !CharacterCombatUtils::HasPlanarDirection(dashDirection)) {
        return;
    }
    if (controller) {
        controller->FaceTowardPlanarDirection(dashDirection, deltaTime);
    }
}

void PlayerSpecialDashAttack::SetActorWorldPosition(const RTBEngine::Math::Vector3& position)
{
    if (!owner) {
        return;
    }

    CharacterCombatUtils::SetActorWorldPosition(
        owner,
        position,
        owner->GetTransform().GetRotation());
}

void PlayerSpecialDashAttack::ApplyDashPose(float normalizedT)
{
    const float t = SmoothStep01(normalizedT);
    RTBEngine::Math::Vector3 position = dashStart + (dashEnd - dashStart) * t;
    SnapRootToGround(position);
    SetActorWorldPosition(position);
}

void PlayerSpecialDashAttack::ApplyDamageAlongSegment(
    const RTBEngine::Math::Vector3& from,
    const RTBEngine::Math::Vector3& to)
{
    if (!owner || damage <= 0.0f) {
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

    RTBEngine::Math::Vector3 segment = to - from;
    segment.y = 0.0f;
    const float segmentLength = segment.Length();
    if (segmentLength <= 0.0001f) {
        return;
    }

    RTBEngine::Math::Vector3 origin = from;
    origin.y += damageHeightOffset;

    CharacterCombatUtils::HostileOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = physicsWorld;
    overlapQuery.instigator = owner;
    overlapQuery.origin = origin;
    overlapQuery.direction = segment * (1.0f / segmentLength);
    overlapQuery.distance = segmentLength;
    overlapQuery.radius = hitRadius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;
    overlapQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Characters");

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargets(overlapQuery);

    for (const CharacterCombatUtils::HostileOverlapHit& hit : hits) {
        if (!hit.health || hit.health->IsDead() || !hit.targetRoot) {
            continue;
        }
        if (std::find(hitTargets.begin(), hitTargets.end(), hit.health) != hitTargets.end()) {
            continue;
        }

        hitTargets.push_back(hit.health);

        HealthComponent::DamageContext damageContext;
        damageContext.amount = damage;
        damageContext.instigator = owner;
        damageContext.hitPoint = hit.hitPoint;
        damageContext.hitDirection = dashDirection;
        damageContext.knockbackStrength = knockbackStrength;
        hit.health->TakeDamage(damage, damageContext);

        if (ThirdPersonCharacterController* targetController =
                hit.targetRoot->GetComponent<ThirdPersonCharacterController>()) {
            targetController->AddPlanarKnockback(dashDirection, knockbackStrength);
        }
    }
}

void PlayerSpecialDashAttack::FinishDash()
{
    if (active && owner) {
        RTBEngine::Math::Vector3 grounded = owner->GetWorldPosition();
        SnapRootToGround(grounded);
        SetActorWorldPosition(grounded);
        ClearMotionVelocity();
    }

    active = false;
    elapsed = 0.0f;
    hitTargets.clear();
    SetUpdateTickEnabled(false);
}

bool PlayerSpecialDashAttack::TryActivate(
    const RTBEngine::Math::Vector3& direction,
    float aimStrength)
{
    if (active || !owner) {
        return false;
    }

    ClampSettings();
    CacheGameplayReferences();

    const float strength = std::clamp(aimStrength, 0.0f, 1.0f);
    if (strength < minAimStrength) {
        return false;
    }

    RTBEngine::Math::Vector3 rootStart;
    RTBEngine::Math::Vector3 rootEnd;
    if (!ResolveDashEndpoints(direction, strength, rootStart, rootEnd)) {
        return false;
    }

    HideAimPreview();
    ClearMotionVelocity();

    dashDirection = CharacterCombatUtils::NormalizePlanarDirection(direction);
    dashStart = rootStart;
    dashEnd = rootEnd;
    previousRootPosition = dashStart;
    elapsed = 0.0f;
    hitTargets.clear();
    active = true;
    SetUpdateTickEnabled(true);
    ApplyDashPose(0.0f);
    return true;
}

void PlayerSpecialDashAttack::OnUpdate(float deltaTime)
{
    if (!active || !owner || deltaTime <= 0.0f) {
        return;
    }

    elapsed += deltaTime;
    const float normalizedT = std::clamp(elapsed / dashDuration, 0.0f, 1.0f);
    ApplyDashPose(normalizedT);

    const RTBEngine::Math::Vector3 currentPosition = owner->GetWorldPosition();
    ApplyDamageAlongSegment(previousRootPosition, currentPosition);
    previousRootPosition = currentPosition;

    if (normalizedT >= 1.0f) {
        FinishDash();
    }
}
