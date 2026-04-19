#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

namespace RTBEngine {
    namespace Animation {
        class Animator;
    }

    namespace ECS {
        class GameObject;
    }

    namespace Physics {
        class PhysicsWorld;
    }
}

class HealthComponent;

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

    RTBEngine::ECS::GameObject* targetObject = nullptr;
    RTBEngine::ECS::GameObject* attackOriginObject = nullptr;
    RTBEngine::Animation::Animator* animator = nullptr;
    float moveSpeed = 2.6f;
    float turnSpeed = 540.0f;
    float attackRange = 1.35f;
    float attackCooldown = 0.85f;
    float attackDamage = 12.0f;
    float attackHitDelay = 2.0f;
    float attackSphereRadius = 0.45f;
    float attackSphereDistance = 0.95f;
    std::string walkAnimationFbx = "Assets/Models/walking.fbx";
    std::string attackAnimationFbx = "Assets/Models/attack.fbx";

    RTB_COMPONENT(EnemyMeleeAI)

private:
    struct AnimationSlotState {
        std::string sourceFbx;
        bool ready = false;
    };

    enum class State {
        Idle,
        Chasing,
        Attacking,
        Cooldown
    };

    State state = State::Idle;
    float cooldownRemaining = 0.0f;
    float attackElapsed = 0.0f;
    bool attackHitExecuted = false;
    RTBEngine::Animation::Animator* registeredAnimator = nullptr;
    bool missingAnimatorWarningShown = false;
    AnimationSlotState walkSlotState;
    AnimationSlotState attackSlotState;
    std::string targetObjectUuid;
    RTBEngine::ECS::GameObject* lastCapturedTarget = nullptr;

    void ClampSettings();
    void CaptureTargetIdentity();
    void ResolveTarget();
    void ResolveAnimator();
    void ConfigurePhysicsBody() const;
    void RegisterAnimationSlots();
    void RegisterAnimationSlot(const char* slotLabel,
                               const std::string& sourceFbx,
                               const char* alias,
                               AnimationSlotState& slotState);
    void UpdateState(float deltaTime);
    void UpdateAttack(float deltaTime);
    void UpdateMovement(float deltaTime);
    void StartAttack();
    void FinishAttack();
    bool HasValidTarget() const;
    bool IsTargetAlive() const;
    HealthComponent* ResolveTargetHealth() const;
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    bool IsWithinTargetHierarchy(RTBEngine::ECS::GameObject* candidate) const;
    float GetPlanarDistanceToTarget() const;
    RTBEngine::Math::Vector3 GetPlanarDirectionToTarget() const;
    RTBEngine::Math::Vector3 GetAttackOriginWorldPosition() const;
    void PlayWalkLoop();
    bool PerformAttackSphereCast(RTBEngine::Math::Vector3* outHitPoint = nullptr);
};
