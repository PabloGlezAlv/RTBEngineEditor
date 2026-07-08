#pragma once

#include "CharacterAbility.h"
#include "CharacterDefinition.h"
#include "HealthComponent.h"
#include "ICharacterStatReceiver.h"

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

class PlayerMeleeSweepAttackAbility : public CharacterAbility, public ICharacterStatReceiver
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
    int tickCount = 1;
    float knockbackStrength = 0.0f;
    bool ignoreSameTeam = true;
    RTBEngine::ECS::AudioSourceComponent* hitAudio = nullptr;

    RTB_COMPONENT(PlayerMeleeSweepAttackAbility)

public:
    void SetMeleeCombatOverrides(float damageAmount,
                                 float range,
                                 float radius,
                                 float tickSeconds,
                                 int definitionTickCount,
                                 float knockback);

    void ApplyCharacterStats(const CharacterDefinition& definition) override;

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
    int GetTickCount() const override;
    bool CanActivateAbility(RTBEngine::ECS::GameObject* instigator,
                            const RTBEngine::Math::Vector3& direction) const override;
    void OnAbilityStarted() override;
    void ExecuteAbilityHit() override;

private:
    void ClampSettings();
    float GetDamagePerHit() const;
    bool ApplySweepHits(RTBEngine::ECS::GameObject* instigator,
                        const RTBEngine::Math::Vector3& attackDirection,
                        float hitDamage);

    float totalAttackDamage = 0.0f;
    float damagePerHit = 0.0f;
    int attackTickCount = 1;
};
