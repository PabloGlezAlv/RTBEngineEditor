#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

enum class CharacterTeam {
    Neutral = 0,
    Player = 1,
    Enemy = 2
};

class CharacterBase : public RTBEngine::ECS::Component
{
public:
    CharacterBase() = default;
    ~CharacterBase() override = default;

    int GetTeam() const { return GetCharacterTeam(); }
    HealthComponent* GetHealth() const { return PeekHealthSlot(); }
    bool IsCharacterDead() const;

protected:
    void ResolveCharacterHealth();
    void RebindCharacterDeathSubscription();
    void UnsubscribeCharacterDeath();
    bool HasSimulationAuthority() const;
    bool IsLocallyControlled() const;
    bool HasLocalGameplayAuthority() const;

    virtual HealthComponent*& AccessHealthSlot() = 0;
    virtual HealthComponent* PeekHealthSlot() const = 0;
    virtual int GetCharacterTeam() const { return static_cast<int>(CharacterTeam::Neutral); }
    virtual void HandleCharacterDeath(const HealthComponent::DeathEvent& eventData) {}

private:
    HealthComponent* subscribedCharacterHealth = nullptr;
    RTBEngine::Core::EventSubscription characterDeathSubscription;
};

class PlayableCharacterController : public CharacterBase
{
public:
    PlayableCharacterController() = default;
    ~PlayableCharacterController() override = default;
};

class AICharacterController : public CharacterBase
{
public:
    AICharacterController() = default;
    ~AICharacterController() override = default;
};
