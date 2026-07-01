#pragma once

#include "CharacterAbility.h"
#include "HealthComponent.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace ECS {
        class AudioSourceComponent;
        class GameObject;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class MeleeSphereAttackAbility : public CharacterAbility
{
public:
    MeleeSphereAttackAbility() = default;
    ~MeleeSphereAttackAbility() override = default;

    void OnValidate() override;

    RTBEngine::Math::Vector3 attackOriginOffset = RTBEngine::Math::Vector3(0.0f, 1.05f, 0.18f);
    float cooldown = 0.85f;
    float damage = 12.0f;
    float hitDelay = 0.45f;
    float recoveryDuration = 0.45f;
    float sphereRadius = 0.45f;
    float sphereDistance = 0.95f;
    float knockbackStrength = 2.5f;
    bool ignoreSameTeam = true;
    RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;

    RTB_COMPONENT(MeleeSphereAttackAbility)

public:
    void SetTargetContext(RTBEngine::ECS::GameObject* targetRoot,
                          HealthComponent* targetHealth,
                          RTBEngine::Physics::PhysicsWorld* physicsWorld);
    void ClearTargetContext();
    float GetCooldownSeconds() const { return cooldown; }
    float GetHitDelaySeconds() const { return hitDelay; }
    float GetDamageAmount() const { return damage; }

protected:
    float GetCooldownDuration() const override { return cooldown; }
    float GetHitDelayDuration() const override { return hitDelay; }
    float GetRecoveryDuration() const override { return recoveryDuration; }
    bool CanActivateAbility(RTBEngine::ECS::GameObject* instigator,
                            const RTBEngine::Math::Vector3& direction) const override;
    void ExecuteAbilityHit() override;
    void OnAbilityFinished() override;

private:
    RTBEngine::ECS::GameObject* preparedTargetRoot = nullptr;
    HealthComponent* preparedTargetHealth = nullptr;
    RTBEngine::Physics::PhysicsWorld* preparedPhysicsWorld = nullptr;

    void ClampSettings();
    bool ApplySphereHit(RTBEngine::ECS::GameObject* instigator,
                        RTBEngine::ECS::GameObject* targetRoot,
                        HealthComponent* targetHealth,
                        RTBEngine::Physics::PhysicsWorld* physicsWorld,
                        const RTBEngine::Math::Vector3& fallbackDirection);
};
