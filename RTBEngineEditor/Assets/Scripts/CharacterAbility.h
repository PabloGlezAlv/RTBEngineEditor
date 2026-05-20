#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

class CharacterAbility : public RTBEngine::ECS::Component
{
public:
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

protected:
    virtual float GetCooldownDuration() const = 0;
    virtual float GetHitDelayDuration() const = 0;
    virtual float GetRecoveryDuration() const = 0;
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
    bool abilityActive = false;
    bool hitExecuted = false;

    void FinishAbility();
};
