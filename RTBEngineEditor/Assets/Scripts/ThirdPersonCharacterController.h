#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
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

class ThirdPersonCharacterController : public RTBEngine::ECS::Component
{
public:
    ThirdPersonCharacterController() = default;
    ~ThirdPersonCharacterController() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnValidate() override;
    void OnDestroy() override;

    RTBEngine::ECS::GameObject* cameraObject = nullptr;
    HealthComponent* health = nullptr;
    RTBEngine::Math::Vector3 attackOriginOffset = RTBEngine::Math::Vector3(0.0f, 1.0f, 0.0f);
    float moveSpeed = 4.0f;
    float sprintMultiplier = 1.75f;
    float turnSpeed = 720.0f;
    float mouseSensitivity = 0.18f;
    float cameraDistance = 4.5f;
    float minCameraDistance = 2.0f;
    float maxCameraDistance = 7.0f;
    float zoomStep = 0.6f;
    float minPitch = -20.0f;
    float maxPitch = 65.0f;
    RTBEngine::Math::Vector3 cameraFocusOffset = RTBEngine::Math::Vector3(0.0f, 1.6f, 0.0f);
    RTBEngine::Animation::Animator* animator = nullptr;
    float attackRange = 1.75f;
    float attackCooldown = 0.8f;
    float attackDamage = 25.0f;
    float attackHitDelay = 0.35f;
    float attackSphereRadius = 0.55f;
    float attackSphereDistance = 1.15f;
    std::string idleAnimationFbx;
    std::string walkAnimationFbx;
    std::string runAnimationFbx;
    std::string attackAnimationFbx = "Assets/Models/AnimationsPlayer/great sword basic atack.fbx";
    std::string deathAnimationFbx = "Assets/Models/AnimationsPlayer/two handed sword death.fbx";

    RTB_COMPONENT(ThirdPersonCharacterController)

private:
    struct AnimationSlotState {
        std::string sourceFbx;
        bool ready = false;
    };

    enum class State {
        Locomotion,
        Attacking,
        Dead
    };

    State state = State::Locomotion;
    float cameraYaw = 0.0f;
    float cameraPitch = 18.0f;
    float cooldownRemaining = 0.0f;
    float attackElapsed = 0.0f;
    bool attackHitExecuted = false;
    RTBEngine::Animation::Animator* registeredAnimator = nullptr;
    bool missingAnimatorWarningShown = false;
    AnimationSlotState idleSlotState;
    AnimationSlotState walkSlotState;
    AnimationSlotState runSlotState;
    AnimationSlotState attackSlotState;
    AnimationSlotState deathSlotState;
    HealthComponent* subscribedHealth = nullptr;
    RTBEngine::Core::EventSubscription deathSubscription;

    void ClampSettings();
    void ConfigurePhysicsBody() const;
    void ResolveCameraObject();
    void ResolveHealth();
    void ResolveAnimator();
    void DisableCompetingCameraController() const;
    void SyncCameraFromCurrentTransform();
    void ApplyCameraOrbitTransform();
    void RegisterAnimationSlots();
    void RegisterAnimationSlot(const char* slotLabel,
                               const std::string& sourceFbx,
                               const char* alias,
                               AnimationSlotState& slotState);
    void RebindHealthSubscription();
    void UnsubscribeFromHealth();
    void UpdateAttackInput();
    void UpdateAttack(float deltaTime);
    void UpdateMovement(float deltaTime);
    void UpdateCameraOrbit();
    void UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning);
    void StartAttack();
    void FinishAttack();
    void HandleDeath(const HealthComponent::DeathEvent& eventData);
    void StopPlanarMotion() const;
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    RTBEngine::Math::Vector3 GetAttackCastOrigin() const;
    HealthComponent* ResolveHitHealth(RTBEngine::ECS::GameObject* hitObject) const;
    bool PerformAttackSphereCast(HealthComponent** outHealth,
                                 RTBEngine::Math::Vector3* outHitPoint,
                                 RTBEngine::Math::Vector3* outHitDirection);
};
