#pragma once

#include "BouncingBallTrajectory.h"
#include "HealthComponent.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <string>
#include <vector>

namespace RTBEngine {
    namespace Physics {
        class PhysicsWorld;
    }
    namespace Scene {
        class GameObject;
        class Prefab;
    }
}

class BouncingBallProjectile : public RTBEngine::Scene::Component {
public:
    struct LaunchConfig {
        RTBEngine::Scene::GameObject* instigator = nullptr;
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        BouncingBallTrajectory::Path path;
        float speed = 12.0f;
        float damage = 22.0f;
        float knockbackStrength = 2.5f;
        float contactRadius = 0.40f;
        bool ignoreSameTeam = true;
        std::string bounceImpactPrefabRef =
            "Assets/Prefabs/Combat/Effects/Bounce Impact Aura.prefab";
    };

    BouncingBallProjectile() = default;
    ~BouncingBallProjectile() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

    void Launch(const LaunchConfig& config);
    bool HasFinished() const { return finished; }

    RTB_COMPONENT(BouncingBallProjectile)

    RTB_SERIALIZE()
    float speed = 12.0f;
    RTB_SERIALIZE()
    float damage = 22.0f;
    RTB_SERIALIZE()
    float knockbackStrength = 2.5f;
    RTB_SERIALIZE()
    float contactRadius = 0.40f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;
    RTB_SERIALIZE()
    std::string bounceImpactPrefabRef =
        "Assets/Prefabs/Combat/Effects/Bounce Impact Aura.prefab";

private:
    bool launched = false;
    bool finished = false;
    bool pendingDestroy = false;
    float distanceTravelled = 0.0f;
    int nextBounceIndex = 0;
    RTBEngine::Scene::GameObject* instigator = nullptr;
    RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
    RTBEngine::Scene::Prefab* bounceImpactPrefab = nullptr;
    BouncingBallTrajectory::Path path;
    std::vector<HealthComponent*> hitTargets;
    RTBEngine::Math::Vector3 previousPosition = RTBEngine::Math::Vector3::Zero();

    void ClampSettings();
    void ResolveBounceImpactPrefab();
    bool HasAlreadyHit(HealthComponent* target) const;
    float CurrentAoeRadius() const;
    void SpawnImpactFx(
        const RTBEngine::Math::Vector3& center,
        float radius) const;
    void ApplyAreaDamage(
        const RTBEngine::Math::Vector3& center,
        float radius,
        const RTBEngine::Math::Vector3& hitDirection);
    bool TryHitWallAlongSegment(
        const RTBEngine::Math::Vector3& from,
        const RTBEngine::Math::Vector3& to,
        RTBEngine::Math::Vector3& outHitPoint) const;
    bool TryHitEnemiesAlongSegment(
        const RTBEngine::Math::Vector3& from,
        const RTBEngine::Math::Vector3& to);
    void TriggerBounce(int bounceIndex);
    void FinishAndDestroy();
};
