#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

enum class CharacterTeam {
    Neutral = 0,
    Player = 1,
    Enemy = 2
};

class CharacterBase : public RTBEngine::Scene::Component
{
public:
    CharacterBase() = default;
    ~CharacterBase() override = default;

    RTB_COMPONENT_TYPE(CharacterBase)

    int GetTeam() const { return GetCharacterTeam(); }
    HealthComponent* GetHealth() const { return PeekHealthSlot(); }
    bool IsCharacterDead() const;

protected:
    void ValidateCharacterHealth();
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

    RTB_COMPONENT_TYPE(PlayableCharacterController)

    virtual void PlayReplicatedAttackVisual(const RTBEngine::Math::Vector3& /*attackDirection*/) {}
};

class AICharacterController : public CharacterBase
{
public:
    AICharacterController() = default;
    ~AICharacterController() override = default;

    RTB_COMPONENT_TYPE(AICharacterController)
};
