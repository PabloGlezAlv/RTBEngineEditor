#pragma once

#include "HealthComponent.h"

#include <RTBEngine/ECS/Entity.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>
#include <vector>

namespace RTBEngine {
    namespace Scene {
        class AudioSourceComponent;
        class GameObject;
        class TrailRenderer;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class ProjectileComponent : public RTBEngine::Scene::Component
{
public:
    struct ProjectileRuntimeContext {
        RTBEngine::Scene::GameObject* instigator = nullptr;
        RTBEngine::Scene::AudioSourceComponent* hitAudio = nullptr;
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3::Forward();
        int instigatorTeam = 0;
        bool applyDamage = true;
    };

    struct ProjectileConfig {
        RTBEngine::Scene::GameObject* instigator = nullptr;
        RTBEngine::Scene::AudioSourceComponent* hitAudio = nullptr;
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3::Forward();
        float speed = 8.0f;
        float maxDistance = 1.15f;
        float radius = 0.55f;
        float damage = 25.0f;
        float knockbackStrength = 0.0f;
        int instigatorTeam = 0;
        bool ignoreSameTeam = true;
        bool destroyOnHit = true;
        int maxHits = 1;
        bool applyDamage = true;
        std::string impactParticlePrefabRef;
        std::string trailFadePrefabRef;
    };

    ProjectileComponent() = default;
    ~ProjectileComponent() override = default;

    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float deltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnValidate() override;
    void OnDestroy() override;

    void BeginFlight(const ProjectileRuntimeContext& context);
    void Initialize(const ProjectileConfig& config);

    static float ResolveMaxDistance(float speed, float lifetime, float maxDistance);
    float GetTravelDistance() const { return maxDistance; }

    float speed = 8.0f;
    float lifetime = 0.85f;
    float maxDistance = 1.15f;
    float radius = 0.55f;
    float damage = 25.0f;
    float knockbackStrength = 0.0f;
    int instigatorTeam = 0;
    bool ignoreSameTeam = true;
    bool destroyOnHit = true;
    int maxHits = 1;
    bool applyDamage = true;
    bool enableFlightTrail = true;
    std::string impactParticlePrefabRef;
    std::string trailFadePrefabRef;

    RTB_COMPONENT(ProjectileComponent)

private:
    RTBEngine::Scene::GameObject* instigator = nullptr;
    RTBEngine::Scene::AudioSourceComponent* hitAudio = nullptr;
    RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
    RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3::Forward();
    float fixedHeight = 0.0f;
    float distanceTravelled = 0.0f;
    int appliedHitCount = 0;
    bool initialized = false;
    bool pendingDestroy = false;
    RTBEngine::Scene::TrailRenderer* flightTrail = nullptr;
    std::vector<HealthComponent*> hitTargets;
    RTBEngine::ECS::Entity ecsEntity = RTBEngine::ECS::kNullEntity;

    void ClampSettings();
    void InitializeFromOwnerTransform();
    void EnsureFlightTrail();
    void UpdateFlightTrail(const RTBEngine::Math::Vector3& position);
    void ReleaseTrailForFadeout();
    void SpawnImpactParticles();
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    HealthComponent* ResolveHitHealth(RTBEngine::Scene::GameObject* hitObject) const;
    bool HasAlreadyHit(HealthComponent* target) const;
    bool HandleSweepHit(const RTBEngine::Math::Vector3& previousPosition,
                        const RTBEngine::Math::Vector3& nextPosition,
                        RTBEngine::Math::Vector3& outResolvedPosition);
    bool ProcessEcsHit(RTBEngine::Scene::GameObject* hitObject,
                       const RTBEngine::Math::Vector3& hitPoint);
    void SyncEcsSimulation(float deltaTime);
    void CreateEcsEntity();
    void DestroyEcsEntity();
    void DestroyProjectile();
};
