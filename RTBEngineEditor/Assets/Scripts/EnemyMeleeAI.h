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
    std::string targetName = "Player GO";
    RTBEngine::Animation::Animator* animator = nullptr;
    float moveSpeed = 2.6f;
    float turnSpeed = 540.0f;
    float attackRange = 1.35f;
    float attackCooldown = 0.85f;
    float attackDamage = 12.0f;
    float attackSphereRadius = 0.45f;
    float attackSphereDistance = 0.95f;
    std::string attackHandBoneName = "mixamorig:RightHand";
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
    RTBEngine::Animation::Animator* registeredAnimator = nullptr;
    bool missingAnimatorWarningShown = false;
    AnimationSlotState walkSlotState;
    AnimationSlotState attackSlotState;

    void ClampSettings();
    void ResolveTarget();
    void ResolveAnimator();
    void ConfigurePhysicsBody() const;
    void RegisterAnimationSlots();
    void RegisterAnimationSlot(const char* slotLabel,
                               const std::string& sourceFbx,
                               const char* alias,
                               AnimationSlotState& slotState);
    void UpdateState(float deltaTime);
    void UpdateMovement(float deltaTime);
    void StartAttack();
    void FinishAttack();
    bool HasValidTarget() const;
    bool IsTargetAlive() const;
    HealthComponent* ResolveTargetHealth() const;
    float GetPlanarDistanceToTarget() const;
    RTBEngine::Math::Vector3 GetPlanarDirectionToTarget() const;
    RTBEngine::Math::Vector3 GetAttackHandWorldPosition() const;
    RTBEngine::Math::Vector3 GetFallbackHandWorldPosition() const;
    void PlayWalkLoop();
    bool PerformAttackSphereCast(RTBEngine::Math::Vector3* outHitPoint = nullptr) const;
    bool SphereCastIntersectsGameObject(RTBEngine::ECS::GameObject* candidate,
                                        const RTBEngine::Math::Vector3& castStart,
                                        const RTBEngine::Math::Vector3& castEnd,
                                        float castRadius,
                                        RTBEngine::Math::Vector3* outHitPoint) const;

    static float DistanceSquaredPointToSegment(const RTBEngine::Math::Vector3& point,
                                               const RTBEngine::Math::Vector3& segmentStart,
                                               const RTBEngine::Math::Vector3& segmentEnd);
    static float DistanceSquaredSegmentToSegment(const RTBEngine::Math::Vector3& segmentAStart,
                                                 const RTBEngine::Math::Vector3& segmentAEnd,
                                                 const RTBEngine::Math::Vector3& segmentBStart,
                                                 const RTBEngine::Math::Vector3& segmentBEnd);
};
