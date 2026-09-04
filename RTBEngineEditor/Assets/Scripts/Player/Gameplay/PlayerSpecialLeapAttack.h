#pragma once

#include "IPlayerSpecialAttack.h"
#include "CharacterCombatUtils.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <string>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
        class RigidBodyComponent;
        class Prefab;
    }
}

class ThirdPersonCharacterController;

// Melee special: Primo-style leap slam with circle+path aim preview.
class PlayerSpecialLeapAttack : public RTBEngine::Scene::Component, public IPlayerSpecialAttack {
public:
    PlayerSpecialLeapAttack() = default;
    ~PlayerSpecialLeapAttack() override = default;

    bool TryActivate(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength = 1.0f) override;
    bool IsActive() const override { return active; }
    void UpdateAimPreview(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength = 1.0f) override;
    void HideAimPreview() override;
    void ApplyMovementLock(float deltaTime) override;

    void OnStart() override;
    void OnValidate() override;
    void OnUpdate(float deltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnDestroy() override;

    RTB_COMPONENT(PlayerSpecialLeapAttack)

    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* jumpPathPreviewTrail = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* impactPreviewTrail = nullptr;
    RTB_SERIALIZE()
    float maxRange = 7.0f;
    RTB_SERIALIZE()
    float minAimStrength = 0.35f;
    RTB_SERIALIZE()
    float impactRadius = 2.25f;
    RTB_SERIALIZE()
    float leapDuration = 0.55f;
    RTB_SERIALIZE()
    float leapHeight = 2.6f;
    RTB_SERIALIZE()
    float damage = 28.0f;
    RTB_SERIALIZE()
    float stunDuration = 1.35f;
    RTB_SERIALIZE()
    float knockbackStrength = 4.0f;
    RTB_SERIALIZE()
    float castRadius = 0.45f;
    RTB_SERIALIZE()
    float trailForwardOffset = 0.15f;
    RTB_SERIALIZE()
    float trailHeightOffset = 0.05f;
    RTB_SERIALIZE()
    float impactHeightOffset = 0.9f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;
    RTB_SERIALIZE()
    std::string landingAuraPrefabRef = "Assets/Prefabs/Combat/Effects/Leap Landing Aura.prefab";

private:
    bool active = false;
    bool previewActive = false;
    bool impactApplied = false;
    float elapsed = 0.0f;
    RTBEngine::Math::Vector3 leapDirection = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 leapStart = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 leapEnd = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 leapImpactCenter = RTBEngine::Math::Vector3::Zero();
    ThirdPersonCharacterController* controller = nullptr;
    CharacterCombatUtils::ActorPhysicsPose physicsPose;
    RTBEngine::Scene::Prefab* landingAuraPrefab = nullptr;

    void ClampSettings();
    void CacheGameplayReferences();
    void ResolveLandingAuraPrefab();
    void ValidateRequiredReferences() const;
    void ConfigurePreviewTrails() const;
    void ShowJumpPathPreview(
        const RTBEngine::Math::Vector3& start,
        const RTBEngine::Math::Vector3& end) const;
    void ShowImpactCirclePreview(const RTBEngine::Math::Vector3& center) const;
    void HidePreviewTrails() const;
    RTBEngine::Math::Vector3 GetFeetOrigin(const RTBEngine::Math::Vector3& direction) const;
    void SnapLandingToGround(RTBEngine::Math::Vector3& feetPosition) const;
    void ResolveLeapEndpoints(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength,
        RTBEngine::Math::Vector3& outFeetStart,
        RTBEngine::Math::Vector3& outFeetEnd,
        RTBEngine::Math::Vector3& outRootStart,
        RTBEngine::Math::Vector3& outRootEnd) const;
    void ApplyLeapPose(float normalizedT);
    void SpawnLandingAura() const;
    void ApplyLandingImpact();
    void FinishLeap();
    void SetActorWorldPosition(const RTBEngine::Math::Vector3& position);
};
