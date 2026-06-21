#pragma once

#include "CharacterAbility.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace GameNet {
    struct ProjectileSpawnSnapshot;
}

namespace RTBEngine {
    namespace ECS {
        class AudioSourceComponent;
        class GameObject;
        class Prefab;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class ProjectileAttackAbility : public CharacterAbility
{
public:
    ProjectileAttackAbility() = default;
    ~ProjectileAttackAbility() override = default;

    void OnStart() override;
    void OnValidate() override;

    std::string projectilePrefabRef = "Assets/Prefabs/Arrow Projectile.prefab";
    float attackOriginHeightOffset = 0.55f;
    float launchForwardOffset = 0.35f;
    RTBEngine::ECS::AudioSourceComponent* fireAudio = nullptr;
    RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;

    RTB_COMPONENT(ProjectileAttackAbility)

public:
    bool FireNow(RTBEngine::ECS::GameObject* instigator,
                 const RTBEngine::Math::Vector3& attackDirection,
                 RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr);
    static bool SpawnFromNetworkSnapshot(const GameNet::ProjectileSpawnSnapshot& snapshot);
    RTBEngine::Math::Vector3 GetLaunchOrigin(RTBEngine::ECS::GameObject* instigator,
                                             const RTBEngine::Math::Vector3& attackDirection) const;
    float GetTravelDistance() const;
    float GetDamageAmount() const { return cachedDamage; }
    bool HasValidProjectilePrefab() const { return projectileSpawnPrefab != nullptr && cachedDamage > 0.0f; }

protected:
    float GetCooldownDuration() const override { return 0.0f; }
    float GetHitDelayDuration() const override { return 0.0f; }
    float GetRecoveryDuration() const override { return 0.0f; }
    bool CanActivateAbility(RTBEngine::ECS::GameObject* instigator,
                            const RTBEngine::Math::Vector3& direction) const override;
    void ExecuteAbilityHit() override;

private:
    RTBEngine::ECS::Prefab* projectileSpawnPrefab = nullptr;
    float cachedTravelDistance = 0.0f;
    float cachedProjectileRadius = 0.05f;
    float cachedDamage = 0.0f;

    void ClampSettings();
    void ResolveProjectilePrefab();
    void RefreshCachedProjectileStats();
    bool SpawnProjectile(RTBEngine::ECS::GameObject* instigator,
                         const RTBEngine::Math::Vector3& attackDirection,
                         RTBEngine::Physics::PhysicsWorld* physicsWorld,
                         bool broadcastOnlineSpawn,
                         const RTBEngine::Math::Vector3* spawnOriginOverride = nullptr,
                         const GameNet::ProjectileSpawnSnapshot* networkSnapshot = nullptr);
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld(RTBEngine::ECS::GameObject* instigator) const;
    static RTBEngine::ECS::GameObject* FindPawnByPlayerSlot(int playerSlot);
};
