#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>
#include <vector>

namespace RTBEngine {
    namespace ECS {
        class AudioSourceComponent;
        class GameObject;
        class TrailRenderer;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class ProjectileComponent : public RTBEngine::ECS::Component
{
public:
    struct ProjectileConfig {
        RTBEngine::ECS::GameObject* instigator = nullptr;
        RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;
        RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3::Forward();
        float speed = 8.0f;
        float maxDistance = 1.15f;
        float radius = 0.55f;
        float damage = 25.0f;
        int instigatorTeam = 0;
        bool ignoreSameTeam = true;
        bool destroyOnHit = true;
        int maxHits = 1;
        bool applyDamage = true;
        std::string impactParticlePrefabRef;
    };

    ProjectileComponent() = default;
    ~ProjectileComponent() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;
    void OnDestroy() override;

    void Initialize(const ProjectileConfig& config);

    float speed = 8.0f;
    float maxDistance = 1.15f;
    float radius = 0.55f;
    float damage = 25.0f;
    int instigatorTeam = 0;
    bool ignoreSameTeam = true;
    bool destroyOnHit = true;
    int maxHits = 1;
    bool applyDamage = true;

    RTB_COMPONENT(ProjectileComponent)

private:
    RTBEngine::ECS::GameObject* instigator = nullptr;
    RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;
    RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr;
    RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3::Forward();
    float fixedHeight = 0.0f;
    float distanceTravelled = 0.0f;
    int appliedHitCount = 0;
    bool initialized = false;
    bool pendingDestroy = false;
    RTBEngine::ECS::TrailRenderer* flightTrail = nullptr;
    std::string impactParticlePrefabRef;
    std::vector<HealthComponent*> hitTargets;

    void ClampSettings();
    void InitializeFromOwnerTransform();
    void EnsureFlightTrail();
    void UpdateFlightTrail(const RTBEngine::Math::Vector3& position);
    void ReleaseTrailForFadeout();
    void SpawnImpactParticles();
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    HealthComponent* ResolveHitHealth(RTBEngine::ECS::GameObject* hitObject) const;
    bool HasAlreadyHit(HealthComponent* target) const;
    bool HandleSweepHit(const RTBEngine::Math::Vector3& previousPosition,
                        const RTBEngine::Math::Vector3& nextPosition,
                        RTBEngine::Math::Vector3& outResolvedPosition);
    void DestroyProjectile();
};
