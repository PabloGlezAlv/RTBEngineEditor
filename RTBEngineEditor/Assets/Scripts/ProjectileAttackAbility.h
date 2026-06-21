#pragma once

#include "CharacterAbility.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace GameNet {
    struct ProjectileSpawnSnapshot;
}

namespace RTBEngine {
    namespace ECS {
        class AudioSourceComponent;
        class GameObject;
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

    void OnValidate() override;

    RTBEngine::Math::Vector3 attackOriginOffset = RTBEngine::Math::Vector3(0.0f, 0.55f, 0.0f);
    RTBEngine::ECS::GameObject* launchOriginObject = nullptr;
    float launchForwardOffset = 0.35f;
    std::string projectileModel;
    std::string projectileTexture;
    float damage = 25.0f;
    float hitDelay = 0.35f;
    float recoveryDuration = 0.5f;
    float projectileSpeed = 8.0f;
    float projectileLifetime = 0.65f;
    float projectileRadius = 0.275f;
    bool destroyOnHit = true;
    int maxHits = 1;
    bool ignoreSameTeam = true;
    RTBEngine::ECS::AudioSourceComponent* fireAudio = nullptr;
    RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;
    std::string impactParticlePrefabRef;

    RTB_COMPONENT(ProjectileAttackAbility)

public:
    bool FireNow(RTBEngine::ECS::GameObject* instigator,
                 const RTBEngine::Math::Vector3& attackDirection,
                 RTBEngine::Physics::PhysicsWorld* physicsWorld = nullptr);
    static bool SpawnFromNetworkSnapshot(const GameNet::ProjectileSpawnSnapshot& snapshot);
    RTBEngine::Math::Vector3 GetLaunchOrigin(RTBEngine::ECS::GameObject* instigator,
                                             const RTBEngine::Math::Vector3& attackDirection) const;
    float GetTravelDistance() const;
    float GetLaunchClearance(RTBEngine::ECS::GameObject* instigator) const;
    float GetHitDelaySeconds() const { return hitDelay; }
    float GetRecoverySeconds() const { return recoveryDuration; }
    float GetDamageAmount() const { return damage; }
    float GetProjectileRadius() const { return projectileRadius; }

protected:
    float GetCooldownDuration() const override { return 0.0f; }
    float GetHitDelayDuration() const override { return hitDelay; }
    float GetRecoveryDuration() const override { return recoveryDuration; }
    bool CanActivateAbility(RTBEngine::ECS::GameObject* instigator,
                            const RTBEngine::Math::Vector3& direction) const override;
    void ExecuteAbilityHit() override;

private:
    void ClampSettings();
    bool SpawnProjectile(RTBEngine::ECS::GameObject* instigator,
                         const RTBEngine::Math::Vector3& attackDirection,
                         RTBEngine::Physics::PhysicsWorld* physicsWorld,
                         bool broadcastOnlineSpawn,
                         const RTBEngine::Math::Vector3* spawnOriginOverride = nullptr);
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld(RTBEngine::ECS::GameObject* instigator) const;
    static RTBEngine::ECS::GameObject* FindPawnByPlayerSlot(int playerSlot);
};
