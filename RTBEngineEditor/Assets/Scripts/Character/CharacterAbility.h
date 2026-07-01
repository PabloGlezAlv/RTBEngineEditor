#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

class CharacterAbility : public RTBEngine::ECS::Component
{
public:
    enum class AimVisualKind {
        None,
        RangedProjectile,
        MeleeRange
    };

    CharacterAbility() = default;
    ~CharacterAbility() override = default;

    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

    bool TryActivate(RTBEngine::ECS::GameObject* instigator,
                     const RTBEngine::Math::Vector3& direction);
    void CancelAbility();

    bool IsAbilityActive() const { return abilityActive; }
    bool IsCoolingDown() const { return cooldownRemaining > 0.0f; }
    float GetCooldownRemaining() const { return cooldownRemaining; }
    RTBEngine::Math::Vector3 GetActiveDirection() const { return activeDirection; }
    RTBEngine::ECS::GameObject* GetActiveInstigator() const { return activeInstigator; }

    virtual bool HasValidAttack() const { return true; }
    virtual bool ConsumesAmmo() const { return true; }
    virtual AimVisualKind GetAimVisualKind() const { return AimVisualKind::None; }
    virtual float GetAimRangeForVisual() const { return 0.0f; }

protected:
    virtual float GetCooldownDuration() const = 0;
    virtual float GetHitDelayDuration() const = 0;
    virtual float GetRecoveryDuration() const = 0;
    virtual float GetTickInterval() const { return 0.0f; }
    virtual bool CanActivateAbility(RTBEngine::ECS::GameObject* instigator,
                                    const RTBEngine::Math::Vector3& direction) const;
    virtual void OnAbilityStarted() {}
    virtual void ExecuteAbilityHit() = 0;
    virtual void OnAbilityFinished() {}

private:
    RTBEngine::ECS::GameObject* activeInstigator = nullptr;
    RTBEngine::Math::Vector3 activeDirection = RTBEngine::Math::Vector3::Forward();
    float cooldownRemaining = 0.0f;
    float activeElapsed = 0.0f;
    float tickAccumulator = 0.0f;
    bool abilityActive = false;
    bool hitExecuted = false;

    void FinishAbility();
    void ProcessActiveAbility(float safeDeltaTime);
    void ProcessSingleHitAbility(float safeDeltaTime);
    void ProcessTickedHitAbility(float safeDeltaTime);
};
