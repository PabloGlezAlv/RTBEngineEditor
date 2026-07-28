#pragma once

#include "CharacterAbility.h"
#include "CharacterDefinition.h"
#include "HealthComponent.h"
#include "ICharacterStatReceiver.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace Scene {
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
    bool CanActivateAbility(RTBEngine::Scene::GameObject* instigator,
                            const RTBEngine::Math::Vector3& direction) const override;
    void OnAbilityStarted() override;
    void ExecuteAbilityHit() override;

    RTB_COMPONENT(PlayerMeleeSweepAttackAbility)

    RTB_SERIALIZE()
    RTBEngine::Math::Vector3 attackOriginOffset = RTBEngine::Math::Vector3(0.0f, 1.05f, 0.18f);
    RTB_SERIALIZE()
    float cooldown = 0.0f;
    RTB_SERIALIZE()
    float damage = 12.0f;
    RTB_SERIALIZE()
    float hitDelay = 0.10f;
    RTB_SERIALIZE()
    float recoveryDuration = 1.50f;
    RTB_SERIALIZE()
    float sphereRadius = 0.45f;
    RTB_SERIALIZE()
    float sphereDistance = 1.20f;
    RTB_SERIALIZE()
    float tickInterval = 0.50f;
    RTB_SERIALIZE()
    int tickCount = 1;
    RTB_SERIALIZE()
    float knockbackStrength = 0.0f;
    RTB_SERIALIZE()
    bool ignoreSameTeam = true;
    RTB_SERIALIZE()
    RTBEngine::Scene::AudioSourceComponent* hitAudio = nullptr;

    void ClampSettings();
    float GetDamagePerHit() const;
    bool ApplySweepHits(RTBEngine::Scene::GameObject* instigator,
                        const RTBEngine::Math::Vector3& attackDirection,
                        float hitDamage);

    float totalAttackDamage = 0.0f;
    float damagePerHit = 0.0f;
    int attackTickCount = 1;
};
