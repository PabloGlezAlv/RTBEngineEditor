#pragma once

#include "BouncingBallTrajectory.h"
#include "CharacterCombatOrigins.h"
#include "IPlayerSpecialAttack.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <string>

namespace RTBEngine {
    namespace Scene {
        class TrailRenderer;
        class Prefab;
        class GameObject;
    }
}

class BouncingBallProjectile;
class ThirdPersonCharacterController;

class PlayerSpecialBouncingBallAttack
    : public RTBEngine::Scene::Component,
      public IPlayerSpecialAttack {
public:
    PlayerSpecialBouncingBallAttack() = default;
    ~PlayerSpecialBouncingBallAttack() override = default;

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

    RTB_COMPONENT(PlayerSpecialBouncingBallAttack)

    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* pathPreviewTrail = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* bouncePreviewTrail = nullptr;
    RTB_SERIALIZE()
    std::string ballPrefabRef = "Assets/Prefabs/Combat/Projectiles/Bouncing Ball.prefab";
    RTB_SERIALIZE()
    float maxRange = 9.0f;
    RTB_SERIALIZE()
    float minAimStrength = 0.35f;
    RTB_SERIALIZE()
    float arcHeight = 2.2f;
    RTB_SERIALIZE()
    float ballSpeed = 12.0f;
    RTB_SERIALIZE()
    float damage = 22.0f;
    RTB_SERIALIZE()
    float knockbackStrength = 2.5f;
    RTB_SERIALIZE()
    float contactRadius = 0.40f;
    RTB_SERIALIZE()
    float bounceRadius0 = 2.4f;
    RTB_SERIALIZE()
    float bounceRadius1 = 1.7f;
    RTB_SERIALIZE()
    float bounceRadius2 = 1.1f;
    RTB_SERIALIZE()
    float launchHeightOffset = 0.95f;
    RTB_SERIALIZE()
    float launchForwardOffset = 0.35f;
    RTB_SERIALIZE()
    float groundSnapLift = 0.05f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;
    RTB_SERIALIZE()
    std::string bounceImpactPrefabRef =
        "Assets/Prefabs/Combat/Effects/Bounce Impact Aura.prefab";

private:
    bool active = false;
    bool previewActive = false;
    RTBEngine::Math::Vector3 aimDirection = RTBEngine::Math::Vector3::Zero();
    ThirdPersonCharacterController* controller = nullptr;
    CharacterCombatOrigins::ColliderBody colliderBody;
    RTBEngine::Scene::Prefab* ballPrefab = nullptr;
    BouncingBallProjectile* activeProjectile = nullptr;
    RTBEngine::Scene::GameObject* activeProjectileObject = nullptr;

    void ClampSettings();
    void CacheGameplayReferences();
    void ResolveBallPrefab();
    void ConfigurePreviewTrails() const;
    void HidePreviewTrails() const;
    void ShowPathPreview(const BouncingBallTrajectory::Path& path) const;
    void ShowBounceCirclesPreview(const BouncingBallTrajectory::Path& path) const;
    bool BuildAimPath(
        const RTBEngine::Math::Vector3& direction,
        float aimStrength,
        BouncingBallTrajectory::Path& outPath) const;
    RTBEngine::Math::Vector3 GetLaunchOrigin(const RTBEngine::Math::Vector3& direction) const;
    void ClearActiveProjectile();
};
