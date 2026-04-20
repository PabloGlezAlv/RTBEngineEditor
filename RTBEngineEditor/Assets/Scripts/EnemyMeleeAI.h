#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class EnemyAnimationDriver;
class EnemyLocomotionController;
class EnemyTargetTracker;

class EnemyMeleeAI : public RTBEngine::ECS::Component
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
    EnemyTargetTracker* targetTracker = nullptr;
    EnemyAnimationDriver* animationDriver = nullptr;
    EnemyLocomotionController* locomotion = nullptr;
    RTBEngine::ECS::GameObject* attackOriginObject = nullptr;
    float attackRange = 1.35f;
    float attackCooldown = 0.85f;
    float attackDamage = 12.0f;
    float attackHitDelay = 0.45f;
    float attackSphereRadius = 0.45f;
    float attackSphereDistance = 0.95f;
    float hitReactDuration = 0.4f;
    float deathHoldDuration = 0.75f;
    float shrinkDuration = 0.85f;

    RTB_COMPONENT(EnemyMeleeAI)

private:
    enum class State {
        Idle,
        Chasing,
        Attacking,
        HitReact,
        Dying,
        Shrinking,
        Dead
    };

    State state = State::Idle;
    float cooldownRemaining = 0.0f;
    float attackElapsed = 0.0f;
    float hitReactRemaining = 0.0f;
    float deathHoldRemaining = 0.0f;
    float shrinkElapsed = 0.0f;
    bool attackHitExecuted = false;
    bool deathPoseLocked = false;
    RTBEngine::Math::Vector3 initialScale = RTBEngine::Math::Vector3::One();
    HealthComponent* subscribedHealth = nullptr;
    RTBEngine::Core::EventSubscription damageTakenSubscription;
    RTBEngine::Core::EventSubscription deathSubscription;

    void ClampSettings();
    void ResolveDependencies();
    void RebindHealthSubscriptions();
    void UnsubscribeFromHealth();
    void UpdateState();
    void UpdateAttack(float deltaTime);
    void UpdateHitReact(float deltaTime);
    void UpdateDying(float deltaTime);
    void UpdateShrinking(float deltaTime);
    void StartAttack();
    void FinishAttack();
    void EnterIdle();
    void EnterChasing();
    bool HasValidCombatSetup() const;
    bool PerformAttackSphereCast(RTBEngine::Math::Vector3* outHitPoint = nullptr,
                                 RTBEngine::Math::Vector3* outHitDirection = nullptr);
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    void HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData);
    void HandleDeath(const HealthComponent::DeathEvent& eventData);
};
