#include "PlayerSpecialBeamAttack.h"

#include "../../VFX/EnergyBeamAsset.h"
#include "../../VFX/EnergyBeamComponent.h"
#include "CharacterCombatOrigins.h"
#include "CharacterCombatUtils.h"
#include "CombatAuthority.h"
#include "HealthComponent.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Data/DataAsset.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>

#include <algorithm>
#include <cmath>

using ThisClass = PlayerSpecialBeamAttack;

RTB_REGISTER_COMPONENT(PlayerSpecialBeamAttack)
    RTB_PROPERTY_COMPONENT(energyBeam, EnergyBeamComponent)
    RTB_PROPERTY_COMPONENT(aimPreviewTrail, TrailRenderer)
    RTB_PROPERTY_DATA_ASSET(beamAssetRef)
    RTB_PROPERTY_SERIALIZED_RANGE(duration, 0.1f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(tickInterval, 0.05f, 1.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(damagePerTick, 0.0f, 100.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(beamLength, 0.5f, 30.0f)
    RTB_PROPERTY_SERIALIZED_RANGE(beamGrowDuration, 0.0f, 5.0f)
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
    beamGrowDuration = std::max(0.0f, beamGrowDuration);
    beamRadius = std::max(0.05f, beamRadius);
    damageRadius = std::max(beamRadius, damageRadius);
    beamWidth = std::max(0.05f, beamWidth);
    damageRadius = std::max(damageRadius, beamWidth * 0.5f);
}

void PlayerSpecialBeamAttack::CacheGameplayReferences()
{
    controller = owner->GetComponent<ThirdPersonCharacterController>();
    colliderBody = CharacterCombatOrigins::ResolveColliderBody(owner);
    BindBeamPresenter();
}

void PlayerSpecialBeamAttack::BindBeamPresenter()
{
    beamPresenter.Bind(energyBeam, aimPreviewTrail);
    ApplyBeamLook();
    beamPresenter.ApplyWidths(beamWidth, previewWidth);
}

void PlayerSpecialBeamAttack::ApplyBeamLook()
{
    EnergyBeamAsset* asset = nullptr;
    if (!beamAssetRef.empty()) {
        RTBEngine::Data::DataAsset* loadedAsset =
            RTBEngine::Core::ResourceManager::GetInstance().LoadDataAsset(beamAssetRef);
        asset = dynamic_cast<EnergyBeamAsset*>(loadedAsset);
        if (!asset) {
            RTB_WARN("[PlayerSpecialBeamAttack] Failed to load beam asset '" + beamAssetRef + "'.");
        }
    }

    if (asset) {
        beamPresenter.ApplyFromAsset(asset);
        return;
    }

    beamPresenter.ApplyDefaults();
}

void PlayerSpecialBeamAttack::ValidateRequiredReferences() const
{
    if (!energyBeam) {
        RTB_WARN("[PlayerSpecialBeamAttack] energyBeam is not assigned on '" + owner->GetName() + "'.");
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

void PlayerSpecialBeamAttack::UpdateAimPreview(
    const RTBEngine::Math::Vector3& direction,
    float /*aimStrength*/)
{
    if (active) {
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
    if (!active || !CharacterCombatUtils::HasPlanarDirection(beamDirection)) {
        return;
    }

    if (controller) {
        controller->FaceTowardPlanarDirection(beamDirection, deltaTime);
    }
}

bool PlayerSpecialBeamAttack::TryActivate(
    const RTBEngine::Math::Vector3& direction,
    float /*aimStrength*/)
{
    if (active) {
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

    // Beam starts at the character and grows outward; damage follows the tip.
    frameMaxLength = ResolveEffectiveLength();
    frameCurrentLength = ResolveGrownLength(frameMaxLength);
    UpdateBeamVisual(frameCurrentLength);
    if (frameCurrentLength > 0.0f) {
        ApplyDamageTick(frameCurrentLength);
    }
    return true;
}

RTBEngine::Math::Vector3 PlayerSpecialBeamAttack::GetBeamOrigin(
    const RTBEngine::Math::Vector3& direction) const
{
    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetFeetWorld(owner, colliderBody);
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
    RTBEngine::Math::Vector3 origin = CharacterCombatOrigins::GetCapsuleCenterWorld(owner, colliderBody);
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

float PlayerSpecialBeamAttack::ResolveGrownLength(float maxLength) const
{
    const float safeMax = std::max(0.0f, maxLength);
    if (safeMax <= 0.0f) {
        return 0.0f;
    }

    // 0 = instant full length; otherwise lerp origin -> tip over beamGrowDuration.
    if (beamGrowDuration <= 0.0f) {
        return safeMax;
    }

    const float growT = std::clamp(elapsed / beamGrowDuration, 0.0f, 1.0f);
    return safeMax * growT;
}

float PlayerSpecialBeamAttack::ResolveEffectiveLengthForDirection(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& direction) const
{
    if (!CharacterCombatUtils::HasPlanarDirection(direction)) {
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
    if (damagePerTick <= 0.0f || effectiveLength <= 0.0f) {
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
    if (effectiveLength <= 0.001f) {
        HideBeamVisual();
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
    if (!active) {
        StopBeam();
        return;
    }

    elapsed += std::max(0.0f, deltaTime);
    tickTimer += std::max(0.0f, deltaTime);

    frameMaxLength = ResolveEffectiveLength();
    frameCurrentLength = ResolveGrownLength(frameMaxLength);

    while (tickTimer >= tickInterval) {
        tickTimer -= tickInterval;
        ApplyDamageTick(frameCurrentLength);
    }

    if (elapsed >= duration) {
        StopBeam();
    }
}

void PlayerSpecialBeamAttack::OnLateUpdate(float /*deltaTime*/)
{
    if (!active) {
        return;
    }

    UpdateBeamVisual(frameCurrentLength);
}

void PlayerSpecialBeamAttack::StopBeam()
{
    if (!active && !energyBeam) {
        SetUpdateTickEnabled(false);
        return;
    }

    active = false;
    elapsed = 0.0f;
    tickTimer = 0.0f;
    frameMaxLength = 0.0f;
    frameCurrentLength = 0.0f;
    beamDirection = RTBEngine::Math::Vector3::Zero();
    HideBeamVisual();
    SetUpdateTickEnabled(false);
}
