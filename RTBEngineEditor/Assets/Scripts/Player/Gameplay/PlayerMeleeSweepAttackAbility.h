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

class PlayerMeleeSweepAttackAbility : public CharacterAbility
{
public:
    PlayerMeleeSweepAttackAbility() = default;
    ~PlayerMeleeSweepAttackAbility() override = default;

    void OnValidate() override;

    RTBEngine::Math::Vector3 attackOriginOffset = RTBEngine::Math::Vector3(0.0f, 1.05f, 0.18f);
    float cooldown = 0.0f;
    float damage = 12.0f;
    float hitDelay = 0.10f;
    float recoveryDuration = 1.50f;
    float sphereRadius = 0.45f;
    float sphereDistance = 1.20f;
    float tickInterval = 0.50f;
    float knockbackStrength = 2.5f;
    bool ignoreSameTeam = true;
    RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;

    RTB_COMPONENT(PlayerMeleeSweepAttackAbility)

public:
    void SetMeleeCombatOverrides(float damageAmount,
                                 float range,
                                 float radius,
                                 float tickSeconds,
                                 float knockback);

    float GetDamageAmount() const { return damage; }
    float GetMeleeRange() const { return sphereDistance; }

    bool HasValidAttack() const override { return damage > 0.0f && sphereDistance > 0.0f; }
    bool ConsumesAmmo() const override { return true; }
    AimVisualKind GetAimVisualKind() const override { return AimVisualKind::MeleeRange; }
    float GetAimRangeForVisual() const override { return sphereDistance; }

protected:
    float GetCooldownDuration() const override { return cooldown; }
    float GetHitDelayDuration() const override { return hitDelay; }
    float GetRecoveryDuration() const override { return recoveryDuration; }
    float GetTickInterval() const override { return tickInterval; }
    bool CanActivateAbility(RTBEngine::ECS::GameObject* instigator,
                            const RTBEngine::Math::Vector3& direction) const override;
    void OnAbilityStarted() override;
    void ExecuteAbilityHit() override;

private:
    void ClampSettings();
    bool ApplySweepHits(RTBEngine::ECS::GameObject* instigator,
                        const RTBEngine::Math::Vector3& attackDirection);
};
