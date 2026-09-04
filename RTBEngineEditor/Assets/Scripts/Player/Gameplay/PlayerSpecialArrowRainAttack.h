#pragma once

#include "IPlayerSpecialAttack.h"
#include "CharacterCombatOrigins.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <string>
#include <vector>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
        class Prefab;
        class GameObject;
    }
}

class ThirdPersonCharacterController;

class PlayerSpecialArrowRainAttack
    : public RTBEngine::Scene::Component,
      public IPlayerSpecialAttack {
public:
    PlayerSpecialArrowRainAttack() = default;
    ~PlayerSpecialArrowRainAttack() override = default;

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

    RTB_COMPONENT(PlayerSpecialArrowRainAttack)

    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* pathPreviewTrail = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* areaPreviewTrail = nullptr;
    RTB_SERIALIZE()
    std::string arrowVisualPrefabRef =
        "Assets/Prefabs/Combat/Projectiles/Arrow Rain Visual.prefab";
    RTB_SERIALIZE()
    std::string impactAuraPrefabRef =
        "Assets/Prefabs/Combat/Effects/Arrow Rain Impact Aura.prefab";
    RTB_SERIALIZE()
    float maxRange = 9.0f;
    RTB_SERIALIZE()
    float minAimStrength = 0.35f;
    RTB_SERIALIZE()
    float rainRadius = 2.50f;
    RTB_SERIALIZE()
    float rainDuration = 1.10f;
    RTB_SERIALIZE()
    float impactDelay = 0.35f;
    RTB_SERIALIZE()
    float tickInterval = 0.22f;
    RTB_SERIALIZE()
    float damagePerTick = 8.0f;
    RTB_SERIALIZE()
    float knockbackStrength = 1.5f;
    RTB_SERIALIZE()
    int arrowCount = 10;
    RTB_SERIALIZE()
    float arrowFallHeight = 6.0f;
    RTB_SERIALIZE()
    float arrowFallDuration = 0.35f;
    RTB_SERIALIZE()
    float castRadius = 0.40f;
    RTB_SERIALIZE()
    float trailHeightOffset = 0.05f;
    RTB_SERIALIZE()
    float impactHeightOffset = 0.85f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;

private:
    struct FallingArrow {
        RTBEngine::Scene::GameObject* object = nullptr;
        RTBEngine::Math::Vector3 start = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 end = RTBEngine::Math::Vector3::Zero();
        float age = 0.0f;
        float duration = 0.35f;
        bool finished = false;
    };

    bool active = false;
    bool previewActive = false;
    bool impactAuraSpawned = false;
    bool arrowsSpawned = false;
    float elapsed = 0.0f;
    float tickTimer = 0.0f;
    RTBEngine::Math::Vector3 aimDirection = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 rainCenter = RTBEngine::Math::Vector3::Zero();
    ThirdPersonCharacterController* controller = nullptr;
    CharacterCombatOrigins::ColliderBody colliderBody;
    RTBEngine::Scene::Prefab* arrowVisualPrefab = nullptr;
    RTBEngine::Scene::Prefab* impactAuraPrefab = nullptr;
    std::vector<FallingArrow> fallingArrows;

    void ClampSettings();
    void CacheGameplayReferences();
    void ResolvePrefabs();
    void ValidateRequiredReferences() const;
    void ConfigurePreviewTrails() const;
    void HidePreviewTrails() const;
    void ShowAreaCirclePreview(const RTBEngine::Math::Vector3& center) const;
    bool ResolveRainCenter(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength,
        RTBEngine::Math::Vector3& outCenter) const;
    void SnapToGround(RTBEngine::Math::Vector3& position) const;
    void SpawnImpactAura() const;
    void SpawnAllFallingArrows();
    void UpdateFallingArrows(float deltaTime);
    void ClearFallingArrows();
    void ApplyRainDamageTick();
    void FinishRain();
    RTBEngine::Math::Vector3 SamplePointInRainCircle() const;
};
