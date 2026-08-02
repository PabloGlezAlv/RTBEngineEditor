#pragma once

#include "HealthComponent.h"
#include "IPlayerSpecialAttack.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <vector>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
        class RigidBodyComponent;
    }
}

class ThirdPersonCharacterController;

// Short directional dash that cuts through enemies along the path.
class PlayerSpecialDashAttack
    : public RTBEngine::Scene::Component,
      public IPlayerSpecialAttack {
public:
    PlayerSpecialDashAttack() = default;
    ~PlayerSpecialDashAttack() override = default;

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
    void OnDestroy() override;

    RTB_COMPONENT(PlayerSpecialDashAttack)

    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* pathPreviewTrail = nullptr;
    RTB_SERIALIZE()
    float maxRange = 3.50f;
    RTB_SERIALIZE()
    float minAimStrength = 0.30f;
    RTB_SERIALIZE()
    float dashDuration = 0.16f;
    RTB_SERIALIZE()
    float damage = 18.0f;
    RTB_SERIALIZE()
    float knockbackStrength = 2.0f;
    RTB_SERIALIZE()
    float hitRadius = 0.55f;
    RTB_SERIALIZE()
    float castRadius = 0.40f;
    RTB_SERIALIZE()
    float trailHeightOffset = 0.90f;
    RTB_SERIALIZE()
    float damageHeightOffset = 0.90f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;

private:
    bool active = false;
    bool previewActive = false;
    float elapsed = 0.0f;
    RTBEngine::Math::Vector3 dashDirection = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 dashStart = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 dashEnd = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 previousRootPosition = RTBEngine::Math::Vector3::Zero();
    ThirdPersonCharacterController* controller = nullptr;
    RTBEngine::Scene::RigidBodyComponent* rigidBodyComponent = nullptr;
    std::vector<HealthComponent*> hitTargets;

    void ClampSettings();
    void CacheGameplayReferences();
    void ValidateRequiredReferences() const;
    void ConfigurePreviewTrail() const;
    void HidePreviewTrail() const;
    void ShowPathPreview(
        const RTBEngine::Math::Vector3& start,
        const RTBEngine::Math::Vector3& end) const;
    bool ResolveDashEndpoints(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength,
        RTBEngine::Math::Vector3& outRootStart,
        RTBEngine::Math::Vector3& outRootEnd) const;
    void SnapRootToGround(RTBEngine::Math::Vector3& rootPosition) const;
    void ClearMotionVelocity() const;
    void SetActorWorldPosition(const RTBEngine::Math::Vector3& position);
    void ApplyDashPose(float normalizedT);
    void ApplyDamageAlongSegment(
        const RTBEngine::Math::Vector3& from,
        const RTBEngine::Math::Vector3& to);
    void FinishDash();
};
