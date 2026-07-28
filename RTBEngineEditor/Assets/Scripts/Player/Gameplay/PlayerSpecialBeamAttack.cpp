#include "PlayerSpecialBeamAttack.h"

#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "HealthComponent.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerSpecialBeamAttack;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;

    bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    RTBEngine::Math::Vector3 NormalizePlanarDirection(RTBEngine::Math::Vector3 direction)
    {
        direction.y = 0.0f;
        if (!HasPlanarDirection(direction)) {
            return {};
        }

        direction.Normalize();
        return direction;
    }

    RTBEngine::Scene::GameObject* FindChildByName(RTBEngine::Scene::GameObject* root, const std::string& name)
    {
        if (!root) {
            return nullptr;
        }

        for (RTBEngine::Scene::GameObject* child : root->GetChildren()) {
            if (!child) {
                continue;
            }

            if (child->GetName() == name) {
                return child;
            }

            if (RTBEngine::Scene::GameObject* nested = FindChildByName(child, name)) {
                return nested;
            }
        }

        return nullptr;
    }
}

RTB_REGISTER_COMPONENT(PlayerSpecialBeamAttack)
    RTB_PROPERTY_COMPONENT(beamTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(aimPreviewTrail, TrailRenderer)
    RTB_PROPERTY_RANGE(duration, 0.1f, 30.0f)
    RTB_PROPERTY_RANGE(tickInterval, 0.05f, 1.0f)
    RTB_PROPERTY_RANGE(damagePerTick, 0.0f, 100.0f)
    RTB_PROPERTY_RANGE(beamLength, 0.5f, 30.0f)
    RTB_PROPERTY_RANGE(beamRadius, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(damageRadius, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(beamWidth, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(previewWidth, 0.05f, 5.0f)
    RTB_PROPERTY_RANGE(trailForwardOffset, 0.0f, 3.0f)
    RTB_PROPERTY_RANGE(trailHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_RANGE(damageHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY(ignoreSameTeam)
RTB_END_REGISTER(PlayerSpecialBeamAttack)

void PlayerSpecialBeamAttack::ClampSettings()
{
    duration = std::max(0.1f, duration);
    tickInterval = std::max(0.05f, tickInterval);
    damagePerTick = std::max(0.0f, damagePerTick);
    beamLength = std::max(0.5f, beamLength);
    beamRadius = std::max(0.05f, beamRadius);
    damageRadius = std::max(beamRadius, damageRadius);
    beamWidth = std::max(0.05f, beamWidth);
    damageRadius = std::max(damageRadius, beamWidth * 0.5f);
}

void PlayerSpecialBeamAttack::EnsureReferences()
{
    if (!owner) {
        return;
    }

    if (!beamTrail) {
        if (RTBEngine::Scene::GameObject* trailObject = FindChildByName(owner, "Special Beam Trail")) {
            beamTrail = trailObject->GetComponent<RTBEngine::Scene::TrailRenderer>();
        }
    }

    if (!aimPreviewTrail) {
        if (RTBEngine::Scene::GameObject* previewObject =
                FindChildByName(owner, "Special Attack Aim Trail")) {
            aimPreviewTrail = previewObject->GetComponent<RTBEngine::Scene::TrailRenderer>();
        }
    }
}

void PlayerSpecialBeamAttack::OnStart()
{
    ClampSettings();
    EnsureReferences();
    HideAimPreview();
    HideBeamVisual();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialBeamAttack::OnValidate()
{
    ClampSettings();
    EnsureReferences();
}

void PlayerSpecialBeamAttack::OnDestroy()
{
    HideAimPreview();
    StopBeam();
}

void PlayerSpecialBeamAttack::UpdateAimPreview(const RTBEngine::Math::Vector3& direction)
{
    if (active || !owner) {
        HideAimPreview();
        return;
    }

    const RTBEngine::Math::Vector3 planarDirection = NormalizePlanarDirection(direction);
    if (!HasPlanarDirection(planarDirection)) {
        HideAimPreview();
        return;
    }

    ClampSettings();
    EnsureReferences();
    if (!aimPreviewTrail) {
        return;
    }

    previewActive = true;
    const RTBEngine::Math::Vector3 visualOrigin = GetBeamOrigin(planarDirection);
    const float effectiveLength = ResolveEffectiveLengthForDirection(
        GetCombatOrigin(planarDirection),
        planarDirection);
    const RTBEngine::Math::Vector3 end = visualOrigin + planarDirection * effectiveLength;
    const RTBEngine::Math::Vector3 points[] = { visualOrigin, end };

    aimPreviewTrail->width = previewWidth;
    aimPreviewTrail->fadeAlphaAlongLength = false;
    aimPreviewTrail->SetGlobalAlphaScale(1.0f);
    aimPreviewTrail->SetPoints(points, 2);
    aimPreviewTrail->SetVisible(true);
}

void PlayerSpecialBeamAttack::HideAimPreview()
{
    previewActive = false;
    if (!aimPreviewTrail) {
        return;
    }

    aimPreviewTrail->SetVisible(false);
    aimPreviewTrail->ClearPoints();
}

void PlayerSpecialBeamAttack::ApplyMovementLock(float deltaTime)
{
    if (!active || !owner || !HasPlanarDirection(beamDirection)) {
        return;
    }

    if (auto* controller = owner->GetComponent<ThirdPersonCharacterController>()) {
        controller->FaceTowardPlanarDirection(beamDirection, deltaTime);
    }
}

bool PlayerSpecialBeamAttack::TryActivate(const RTBEngine::Math::Vector3& direction)
{
    if (active || !owner) {
        return false;
    }

    if (RTBEngine::Scene::SceneManager::GetInstance().IsSceneUnloading()) {
        return false;
    }

    const RTBEngine::Math::Vector3 planarDirection = NormalizePlanarDirection(direction);
    if (!HasPlanarDirection(planarDirection)) {
        return false;
    }

    HideAimPreview();

    ClampSettings();
    EnsureReferences();

    beamDirection = planarDirection;
    elapsed = 0.0f;
    tickTimer = 0.0f;
    active = true;
    SetEnabled(true);
    SetUpdateTickEnabled(true);

    if (beamTrail) {
        beamTrail->width = beamWidth;
        beamTrail->fadeAlphaAlongLength = true;
        beamTrail->SetGlobalAlphaScale(1.0f);
    }

    const float effectiveLength = ResolveEffectiveLength();
    UpdateBeamVisual(effectiveLength);
    ApplyDamageTick(effectiveLength);
    return true;
}

RTBEngine::Math::Vector3 PlayerSpecialBeamAttack::GetBeamOrigin(
    const RTBEngine::Math::Vector3& direction) const
{
    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetFeetWorld(owner);
    origin = CharacterCombatOrigins::ApplyPlanarDirectionOffset(
        origin,
        direction,
        trailForwardOffset);
    origin.y += trailHeightOffset;
    return origin;
}

RTBEngine::Math::Vector3 PlayerSpecialBeamAttack::GetCombatOrigin(
    const RTBEngine::Math::Vector3& direction) const
{
    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetCapsuleCenterWorld(owner);
    origin = CharacterCombatOrigins::ApplyPlanarDirectionOffset(
        origin,
        direction,
        trailForwardOffset);
    origin.y += damageHeightOffset;
    return origin;
}

float PlayerSpecialBeamAttack::ResolveEffectiveLength() const
{
    return ResolveEffectiveLengthForDirection(GetCombatOrigin(beamDirection), beamDirection);
}

float PlayerSpecialBeamAttack::ResolveEffectiveLengthForDirection(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& direction) const
{
    if (!owner || !HasPlanarDirection(direction)) {
        return 0.0f;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld =
        CharacterCombatUtils::ResolvePhysicsWorld(owner);
    if (!physicsWorld) {
        return beamLength;
    }

    CharacterCombatUtils::PlanarEnvironmentClipQuery clipQuery;
    clipQuery.physicsWorld = physicsWorld;
    clipQuery.instigator = owner;
    clipQuery.origin = origin;
    clipQuery.direction = direction;
    clipQuery.maxLength = beamLength;
    clipQuery.castRadius = beamRadius;
    clipQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Default");
    return CharacterCombatUtils::ResolvePlanarEnvironmentClipLength(clipQuery);
}

void PlayerSpecialBeamAttack::ApplyDamageTick(float effectiveLength)
{
    if (!owner || damagePerTick <= 0.0f || effectiveLength <= 0.0f) {
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

    const RTBEngine::Math::Vector3 origin = GetCombatOrigin(beamDirection);

    CharacterCombatUtils::HostileOverlapQuery overlapQuery;
    overlapQuery.physicsWorld = physicsWorld;
    overlapQuery.instigator = owner;
    overlapQuery.origin = origin;
    overlapQuery.direction = beamDirection;
    overlapQuery.distance = effectiveLength;
    overlapQuery.radius = damageRadius;
    overlapQuery.ignoreSameTeam = ignoreSameTeam;
    overlapQuery.layerMask = CharacterCombatUtils::GetPhysicsLayerBit("Characters");

    const std::vector<CharacterCombatUtils::HostileOverlapHit> hits =
        CharacterCombatUtils::OverlapHostileTargets(overlapQuery);

    for (const CharacterCombatUtils::HostileOverlapHit& hit : hits) {
        if (!hit.health || hit.health->IsDead()) {
            continue;
        }

        HealthComponent::DamageContext damageContext;
        damageContext.amount = damagePerTick;
        damageContext.instigator = owner;
        damageContext.hitPoint = hit.hitPoint;
        damageContext.hitDirection = beamDirection;
        damageContext.knockbackStrength = 0.0f;
        hit.health->TakeDamage(damagePerTick, damageContext);
    }
}

void PlayerSpecialBeamAttack::UpdateBeamVisual(float effectiveLength)
{
    if (!beamTrail || !owner) {
        return;
    }

    const RTBEngine::Math::Vector3 visualOrigin = GetBeamOrigin(beamDirection);
    const RTBEngine::Math::Vector3 end = visualOrigin + beamDirection * effectiveLength;
    const RTBEngine::Math::Vector3 points[] = { visualOrigin, end };

    beamTrail->width = beamWidth;
    beamTrail->SetPoints(points, 2);
    beamTrail->SetVisible(true);
}

void PlayerSpecialBeamAttack::HideBeamVisual()
{
    if (!beamTrail) {
        return;
    }

    beamTrail->SetVisible(false);
    beamTrail->ClearPoints();
}

void PlayerSpecialBeamAttack::OnUpdate(float deltaTime)
{
    if (!active || !owner) {
        StopBeam();
        return;
    }

    elapsed += std::max(0.0f, deltaTime);
    tickTimer += std::max(0.0f, deltaTime);

    const float effectiveLength = ResolveEffectiveLength();

    while (tickTimer >= tickInterval) {
        tickTimer -= tickInterval;
        ApplyDamageTick(effectiveLength);
    }

    if (elapsed >= duration) {
        StopBeam();
    }
}

void PlayerSpecialBeamAttack::OnLateUpdate(float /*deltaTime*/)
{
    if (!active || !owner) {
        return;
    }

    const float effectiveLength = ResolveEffectiveLength();
    UpdateBeamVisual(effectiveLength);
}

void PlayerSpecialBeamAttack::StopBeam()
{
    if (!active && !beamTrail) {
        SetUpdateTickEnabled(false);
        return;
    }

    active = false;
    elapsed = 0.0f;
    tickTimer = 0.0f;
    beamDirection = RTBEngine::Math::Vector3::Zero();
    HideBeamVisual();
    SetUpdateTickEnabled(false);
}