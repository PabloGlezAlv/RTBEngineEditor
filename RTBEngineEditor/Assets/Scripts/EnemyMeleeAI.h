#pragma once

#include "CharacterBase.h"
#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
        class NavAgentComponent;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class EnemyAnimationDriver;
class EnemyLocomotionController;
class EnemyTargetTracker;
class MeleeSphereAttackAbility;

class EnemyMeleeAI : public AICharacterController
{
public:
    EnemyMeleeAI() = default;
    ~EnemyMeleeAI() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnValidate() override;
    void OnDestroy() override;

    HealthComponent* health = nullptr;
    int team = static_cast<int>(CharacterTeam::Enemy);
    EnemyTargetTracker* targetTracker = nullptr;
    EnemyAnimationDriver* animationDriver = nullptr;
    EnemyLocomotionController* locomotion = nullptr;
    MeleeSphereAttackAbility* meleeAttack = nullptr;
    RTBEngine::ECS::NavAgentComponent* navAgent = nullptr;
    float attackRange = 1.35f;
    float preferredAttackDistance = 1.05f;
    float hitReactDuration = 0.4f;
    float deathHoldDuration = 0.75f;
    float shrinkDuration = 0.85f;

    void PlayReplicatedAttack(std::uint32_t attackSequence);

    RTB_COMPONENT(EnemyMeleeAI)

private:
    enum class State {
        Idle,
        Chasing,
        Repositioning,
        Attacking,
        HitReact,
        Dying,
        Shrinking,
        Dead
    };

    State state = State::Idle;
    float hitReactRemaining = 0.0f;
    float deathHoldRemaining = 0.0f;
    float shrinkElapsed = 0.0f;
    bool deathPoseLocked = false;
    RTBEngine::Math::Vector3 initialScale = RTBEngine::Math::Vector3::One();
    RTBEngine::Math::Vector3 lastReplicatedPosition = RTBEngine::Math::Vector3::Zero();
    bool hasReplicatedPosition = false;
    std::uint32_t networkAttackSequence = 0;
    std::uint32_t lastProcessedReplicatedAttackSequence = 0;
    bool hasRepositionDestination = false;
    RTBEngine::Math::Vector3 repositionDestination = RTBEngine::Math::Vector3::Zero(); // Set once in EnterRepositioning
    HealthComponent* subscribedHealth = nullptr;
    RTBEngine::Core::EventSubscription damageTakenSubscription;
    RTBEngine::Core::EventSubscription deathSubscription;

    void ClampSettings();
    void ResolveDependencies();
    void ResolveMeleeAttack();
    void RebindHealthSubscriptions();
    void UnsubscribeFromHealth();
    void UpdateState();
    void UpdateHitReact(float deltaTime);
    void UpdateDying(float deltaTime);
    void UpdateShrinking(float deltaTime);
    void StartAttack();
    void FinishAttack();
    void EnterIdle();
    void EnterChasing();
    void EnterRepositioning();
    bool HasValidCombatSetup() const;
    void UpdateReplicatedLocomotionAnimation(float deltaTime);
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    void HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData);
    void HandleDeath(const HealthComponent::DeathEvent& eventData);
    void HandleCharacterDeath(const HealthComponent::DeathEvent& eventData) override;
    HealthComponent*& AccessHealthSlot() override;
    HealthComponent* PeekHealthSlot() const override;
    int GetCharacterTeam() const override;
};
