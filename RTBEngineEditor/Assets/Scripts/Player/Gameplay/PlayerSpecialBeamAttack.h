#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
    }
}

class PlayerSpecialBeamAttack : public RTBEngine::Scene::Component
{
public:
    PlayerSpecialBeamAttack() = default;
    ~PlayerSpecialBeamAttack() override = default;

    RTBEngine::Scene::TrailRenderer* beamTrail = nullptr;
    RTBEngine::Scene::TrailRenderer* aimPreviewTrail = nullptr;

    float duration = 5.0f;
    float tickInterval = 0.1f;
    float damagePerTick = 1.0f;
    float beamLength = 8.0f;
    float beamRadius = 0.45f;
    float damageRadius = 0.80f;
    float beamWidth = 1.60f;
    float previewWidth = 1.10f;
    float trailForwardOffset = 0.40f;
    float trailHeightOffset = 0.0f;
    float damageHeightOffset = 0.0f;
    bool ignoreSameTeam = true;

    bool TryActivate(const RTBEngine::Math::Vector3& direction);
    bool IsActive() const { return active; }
    void UpdateAimPreview(const RTBEngine::Math::Vector3& direction);
    void HideAimPreview();
    void ApplyMovementLock(float deltaTime);

    RTB_COMPONENT(PlayerSpecialBeamAttack)

    void OnStart() override;
    void OnValidate() override;
    void OnUpdate(float deltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnDestroy() override;

private:
    bool active = false;
    bool previewActive = false;
    float elapsed = 0.0f;
    float tickTimer = 0.0f;
    RTBEngine::Math::Vector3 beamDirection = RTBEngine::Math::Vector3::Zero();

    void ClampSettings();
    void EnsureReferences();
    void StopBeam();
    RTBEngine::Math::Vector3 GetBeamOrigin(const RTBEngine::Math::Vector3& direction) const;
    RTBEngine::Math::Vector3 GetCombatOrigin(const RTBEngine::Math::Vector3& direction) const;
    float ResolveEffectiveLength() const;
    float ResolveEffectiveLengthForDirection(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& direction) const;
    void ApplyDamageTick(float effectiveLength);
    void UpdateBeamVisual(float effectiveLength);
    void HideBeamVisual();
};
