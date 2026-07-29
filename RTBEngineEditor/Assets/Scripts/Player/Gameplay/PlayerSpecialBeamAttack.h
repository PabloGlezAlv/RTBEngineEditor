#pragma once

#include "PlayerSpecialBeamPresenter.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
    }
}

class ThirdPersonCharacterController;

class PlayerSpecialBeamAttack : public RTBEngine::Scene::Component
{
public:
    PlayerSpecialBeamAttack() = default;
    ~PlayerSpecialBeamAttack() override = default;

    bool TryActivate(const RTBEngine::Math::Vector3& direction);
    bool IsActive() const { return active; }
    void UpdateAimPreview(const RTBEngine::Math::Vector3& direction);
    void HideAimPreview();
    void ApplyMovementLock(float deltaTime);

    void OnStart() override;
    void OnValidate() override;
    void OnUpdate(float deltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnDestroy() override;

    RTB_COMPONENT(PlayerSpecialBeamAttack)

    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* beamTrail = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* beamAuraTrail = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* beamHaloTrail = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* aimPreviewTrail = nullptr;
    RTB_SERIALIZE()
    float duration = 5.0f;
    RTB_SERIALIZE()
    float tickInterval = 0.1f;
    RTB_SERIALIZE()
    float damagePerTick = 1.0f;
    RTB_SERIALIZE()
    float beamLength = 8.0f;
    RTB_SERIALIZE()
    float beamRadius = 0.45f;
    RTB_SERIALIZE()
    float damageRadius = 0.80f;
    RTB_SERIALIZE()
    float beamWidth = 1.60f;
    RTB_SERIALIZE()
    float previewWidth = 1.10f;
    RTB_SERIALIZE()
    float trailForwardOffset = 0.40f;
    RTB_SERIALIZE()
    float trailHeightOffset = 0.0f;
    RTB_SERIALIZE()
    float damageHeightOffset = 0.0f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;

    bool active = false;
    bool previewActive = false;
    float elapsed = 0.0f;
    float tickTimer = 0.0f;
    float frameEffectiveLength = 0.0f;
    RTBEngine::Math::Vector3 beamDirection = RTBEngine::Math::Vector3::Zero();
    ThirdPersonCharacterController* controller = nullptr;
    PlayerSpecialBeamPresenter beamPresenter;

    void ClampSettings();
    void CacheGameplayReferences();
    void BindBeamPresenter();
    void ValidateRequiredReferences() const;
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
