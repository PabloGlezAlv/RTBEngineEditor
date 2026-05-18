#pragma once

#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

namespace RTBEngine {
    namespace Animation {
        class Animator;
    }

    namespace ECS {
        class GameObject;
        class TrailRenderer;
    }

    namespace Physics {
        class PhysicsWorld;
    }

    namespace UI {
        class UIJoystick;
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
    float cameraDistance = 11.0f;
    RTBEngine::Math::Vector3 cameraFocusOffset = RTBEngine::Math::Vector3(0.0f, 1.2f, 0.0f);
    RTBEngine::Animation::Animator* animator = nullptr;
    float attackCooldown = 0.8f;
    float attackDamage = 25.0f;
    float attackHitDelay = 0.35f;
    float projectileSpeed = 8.0f;
    float projectileLifetime = 0.65f;
    float projectileRadius = 0.275f;
    RTBEngine::UI::UIJoystick* attackJoystick = nullptr;
    RTBEngine::ECS::TrailRenderer* attackAimTrail = nullptr;
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
    RTBEngine::UI::UIJoystick* subscribedAttackJoystick = nullptr;
    RTBEngine::Core::EventSubscription attackJoystickReleaseSubscription;
    RTBEngine::Math::Vector3 activeAttackDirection = RTBEngine::Math::Vector3::Zero();

    void ClampSettings();
    void ConfigurePhysicsBody() const;
    void ResolveCameraObject();
    void ResolveHealth();
    void ResolveAnimator();
    void ResolveAttackAimTrail();
    void DisableCompetingCameraController() const;
    void ApplyCameraFollowTransform();
    void RegisterAnimationSlots();
    void RegisterAnimationSlot(const char* slotLabel,
                               const std::string& sourceFbx,
                               const char* alias,
                               AnimationSlotState& slotState);
    void RebindHealthSubscription();
    void UnsubscribeFromHealth();
    void RebindAttackJoystickSubscription();
    void UnsubscribeFromAttackJoystick();
    void HandleJoystickAttackReleased(const RTBEngine::Math::Vector2& joystickValue);
    void UpdateAttackAimTrail();
    void HideAttackAimTrail();
    void UpdateAttack(float deltaTime);
    void UpdateAttackFacingLock(float deltaTime);
    void UpdateMovement(float deltaTime);
    void UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning);
    void StartAttack(const RTBEngine::Math::Vector3& attackDirection);
    void SpawnAttackProjectile();
    void FinishAttack();
    void HandleDeath(const HealthComponent::DeathEvent& eventData);
    void FaceAttackDirection(const RTBEngine::Math::Vector3& attackDirection);
    void StopPlanarMotion() const;
    RTBEngine::Math::Vector3 GetDesiredMoveDirection(bool& outIsRunning) const;
    RTBEngine::Physics::PhysicsWorld* ResolvePhysicsWorld() const;
    RTBEngine::Math::Vector3 GetAttackCastOrigin() const;
    RTBEngine::Math::Vector3 GetProjectileLaunchOrigin(const RTBEngine::Math::Vector3& attackDirection) const;
    float GetProjectileTravelDistance() const;
    float GetProjectileLaunchClearance() const;
    RTBEngine::Math::Vector3 GetAttackDirectionFromJoystick(const RTBEngine::Math::Vector2& joystickValue) const;
    RTBEngine::Math::Vector3 GetActiveAttackDirection() const;
};
