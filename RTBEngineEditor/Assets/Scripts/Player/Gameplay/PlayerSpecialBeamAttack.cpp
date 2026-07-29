#include "PlayerSpecialBeamAttack.h"

#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "HealthComponent.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerSpecialBeamAttack;

RTB_REGISTER_COMPONENT(PlayerSpecialBeamAttack)
    RTB_PROPERTY_COMPONENT(beamTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(beamAuraTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(beamHaloTrail, TrailRenderer)
    RTB_PROPERTY_COMPONENT(aimPreviewTrail, TrailRenderer)
    RTB_PROPERTY_SERIALIZED_RANGE(duration, 0.1f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(tickInterval, 0.05f, 1.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damagePerTick, 0.0f, 100.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(beamLength, 0.5f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(beamRadius, 0.05f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damageRadius, 0.05f, 5.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(beamWidth, 0.05f, 5.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(previewWidth, 0.05f, 5.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(trailForwardOffset, 0.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(trailHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damageHeightOffset, -2.0f, 3.0f)
    RTB_PROPERTY_SERIALIZED(ignoreSameTeam)
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

void PlayerSpecialBeamAttack::CacheGameplayReferences()
{
    controller = owner ? owner->GetComponent<ThirdPersonCharacterController>() : nullptr;
    BindBeamPresenter();
}

void PlayerSpecialBeamAttack::BindBeamPresenter()
{
    beamPresenter.Bind(beamTrail, beamAuraTrail, beamHaloTrail, aimPreviewTrail);
    beamPresenter.ApplyWidths(beamWidth, previewWidth);
}

void PlayerSpecialBeamAttack::ValidateRequiredReferences() const
{
    if (!owner) {
        return;
    }

    if (!beamTrail) {
        RTB_WARN("[PlayerSpecialBeamAttack] beamTrail is not assigned on '" + owner->GetName() + "'.");
    }
    if (!beamAuraTrail) {
        RTB_WARN("[PlayerSpecialBeamAttack] beamAuraTrail is not assigned on '" + owner->GetName() + "'.");
    }
    if (!beamHaloTrail) {
        RTB_WARN("[PlayerSpecialBeamAttack] beamHaloTrail is not assigned on '" + owner->GetName() + "'.");
    }
    if (!aimPreviewTrail) {
        RTB_WARN("[PlayerSpecialBeamAttack] aimPreviewTrail is not assigned on '" +
                 owner->GetName() + "'.");
    }
}

void PlayerSpecialBeamAttack::OnStart()
{
    ClampSettings();
    CacheGameplayReferences();
    ValidateRequiredReferences();
    HideAimPreview();
    HideBeamVisual();
    SetUpdateTickEnabled(false);
}

void PlayerSpecialBeamAttack::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
}

void PlayerSpecialBeamAttack::OnDestroy()
{
    HideAimPreview();
    StopBeam();
    controller = nullptr;
}

void PlayerSpecialBeamAttack::UpdateAimPreview(const RTBEngine::Math::Vector3& direction)
{
    if (active || !owner) {
        HideAimPreview();
        return;
    }

    const RTBEngine::Math::Vector3 planarDirection =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    if (!CharacterCombatUtils::HasPlanarDirection(planarDirection)) {
        HideAimPreview();
        return;
    }

    ClampSettings();
    BindBeamPresenter();
    previewActive = true;
    const RTBEngine::Math::Vector3 visualOrigin = GetBeamOrigin(planarDirection);
    const float effectiveLength = ResolveEffectiveLengthForDirection(
        GetCombatOrigin(planarDirection),
        planarDirection);
    const RTBEngine::Math::Vector3 end = visualOrigin + planarDirection * effectiveLength;
    beamPresenter.ShowPreview(visualOrigin, end);
}

void PlayerSpecialBeamAttack::HideAimPreview()
{
    previewActive = false;
    beamPresenter.HidePreview();
}

void PlayerSpecialBeamAttack::ApplyMovementLock(float deltaTime)
{
    if (!active || !owner || !CharacterCombatUtils::HasPlanarDirection(beamDirection)) {
        return;
    }

    if (controller) {
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

    const RTBEngine::Math::Vector3 planarDirection =
        CharacterCombatUtils::NormalizePlanarDirection(direction);
    if (!CharacterCombatUtils::HasPlanarDirection(planarDirection)) {
        return false;
    }

    HideAimPreview();
    ClampSettings();
    CacheGameplayReferences();

    beamDirection = planarDirection;
    elapsed = 0.0f;
    tickTimer = 0.0f;
    active = true;
    SetEnabled(true);
    SetUpdateTickEnabled(true);

    frameEffectiveLength = ResolveEffectiveLength();
    UpdateBeamVisual(frameEffectiveLength);
    ApplyDamageTick(frameEffectiveLength);
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
    if (!owner || !CharacterCombatUtils::HasPlanarDirection(direction)) {
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
    if (!owner) {
        return;
    }

    BindBeamPresenter();
    const RTBEngine::Math::Vector3 visualOrigin = GetBeamOrigin(beamDirection);
    const RTBEngine::Math::Vector3 end = visualOrigin + beamDirection * effectiveLength;
    beamPresenter.ShowBeam(visualOrigin, end);
}

void PlayerSpecialBeamAttack::HideBeamVisual()
{
    beamPresenter.HideBeam();
}

void PlayerSpecialBeamAttack::OnUpdate(float deltaTime)
{
    if (!active || !owner) {
        StopBeam();
        return;
    }

    elapsed += std::max(0.0f, deltaTime);
    tickTimer += std::max(0.0f, deltaTime);

    frameEffectiveLength = ResolveEffectiveLength();

    while (tickTimer >= tickInterval) {
        tickTimer -= tickInterval;
        ApplyDamageTick(frameEffectiveLength);
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

    UpdateBeamVisual(frameEffectiveLength);
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
    frameEffectiveLength = 0.0f;
    beamDirection = RTBEngine::Math::Vector3::Zero();
    HideBeamVisual();
    SetUpdateTickEnabled(false);
}
