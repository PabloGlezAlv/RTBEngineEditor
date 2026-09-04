#pragma once

#include "IPlayerSpecialAttack.h"
#include "PlayerSpecialBeamPresenter.h"
#include "CharacterCombatOrigins.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <string>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
    }
}

class EnergyBeamComponent;
class ThirdPersonCharacterController;

class PlayerSpecialBeamAttack : public RTBEngine::Scene::Component, public IPlayerSpecialAttack
{
public:
    PlayerSpecialBeamAttack() = default;
    ~PlayerSpecialBeamAttack() override = default;

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

    RTB_COMPONENT(PlayerSpecialBeamAttack)

    RTB_SERIALIZE()
    EnergyBeamComponent* energyBeam = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* aimPreviewTrail = nullptr;
    RTB_SERIALIZE()
    std::string beamAssetRef;
    RTB_SERIALIZE()
    float duration = 5.0f;
    RTB_SERIALIZE()
    float tickInterval = 0.1f;
    RTB_SERIALIZE()
    float damagePerTick = 1.0f;
    RTB_SERIALIZE()
    float beamLength = 8.0f;
    RTB_SERIALIZE()
    float beamGrowDuration = 0.35f;
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
    float frameMaxLength = 0.0f;
    float frameCurrentLength = 0.0f;
    RTBEngine::Math::Vector3 beamDirection = RTBEngine::Math::Vector3::Zero();
    ThirdPersonCharacterController* controller = nullptr;
    CharacterCombatOrigins::ColliderBody colliderBody;
    PlayerSpecialBeamPresenter beamPresenter;

    void ClampSettings();
    void CacheGameplayReferences();
    void BindBeamPresenter();
    void ApplyBeamLook();
    void ValidateRequiredReferences() const;
    void StopBeam();
    RTBEngine::Math::Vector3 GetBeamOrigin(const RTBEngine::Math::Vector3& direction) const;
    RTBEngine::Math::Vector3 GetCombatOrigin(const RTBEngine::Math::Vector3& direction) const;
    float ResolveEffectiveLength() const;
    float ResolveEffectiveLengthForDirection(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& direction) const;
    float ResolveGrownLength(float maxLength) const;
    void ApplyDamageTick(float effectiveLength);
    void UpdateBeamVisual(float effectiveLength);
    void HideBeamVisual();
};
