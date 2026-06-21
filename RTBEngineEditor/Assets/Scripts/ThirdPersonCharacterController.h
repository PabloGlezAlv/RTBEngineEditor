#pragma once

#include "CharacterBase.h"
#include "HealthComponent.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <string>

#include <cstdint>

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
        class RigidBody;
    }

    namespace UI {
        class UIJoystick;
    }
}

class ProjectileAttackAbility;

class ThirdPersonCharacterController : public PlayableCharacterController
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

    void ForceDeathState();
    void ReviveFromDeath();
    void AddPlanarKnockback(const RTBEngine::Math::Vector3& direction, float strength);

    RTBEngine::ECS::GameObject* cameraObject = nullptr;
    HealthComponent* health = nullptr;
    int team = static_cast<int>(CharacterTeam::Player);
    float moveSpeed = 4.0f;
    float sprintMultiplier = 1.75f;
    float turnSpeed = 720.0f;
    float cameraDistance = 11.0f;
    RTBEngine::Math::Vector3 cameraFocusOffset = RTBEngine::Math::Vector3(0.0f, 1.2f, 0.0f);
    RTBEngine::Animation::Animator* animator = nullptr;
    ProjectileAttackAbility* projectileAttack = nullptr;
    RTBEngine::UI::UIJoystick* attackJoystick = nullptr;
    RTBEngine::ECS::TrailRenderer* attackAimTrail = nullptr;
    float aimTrailForwardOffset = 0.40f;
    float aimTrailHeightOffset = 0.0f;
    RTBEngine::ECS::GameObject* aimArrowVisual = nullptr;
    std::string idleAnimationFbx;
    std::string walkAnimationFbx;
    std::string runAnimationFbx;
    std::string aimDrawAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Draw";
    std::string aimLoopAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Aiming_Idle";
    std::string attackAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_CombatRanged.fbx|Ranged_Bow_Release";
    std::string deathAnimationFbx = "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Death_A";

    RTB_COMPONENT(ThirdPersonCharacterController)

private:
    struct AnimationSlotState {
        std::string sourceFbx;
        bool ready = false;
    };

    enum class State {
        Locomotion,
        Aiming,
        Attacking,
        Dead
    };

    enum class AimPhase {
        Draw,
        Hold
    };

    State state = State::Locomotion;
    AimPhase aimPhase = AimPhase::Draw;
    bool wasDraggingJoystick = false;
    RTBEngine::Animation::Animator* registeredAnimator = nullptr;
    bool missingAnimatorWarningShown = false;
    AnimationSlotState idleSlotState;
    AnimationSlotState walkSlotState;
    AnimationSlotState runSlotState;
    AnimationSlotState aimDrawSlotState;
    AnimationSlotState aimLoopSlotState;
    AnimationSlotState attackSlotState;
    AnimationSlotState deathSlotState;
    RTBEngine::UI::UIJoystick* subscribedAttackJoystick = nullptr;
    RTBEngine::Core::EventSubscription attackJoystickReleaseSubscription;
    RTBEngine::Math::Vector3 activeAttackDirection = RTBEngine::Math::Vector3::Zero();

    void ClampSettings();
    void ResolveCameraObject();
    void ResolveHealth();
    void ResolveAnimator();
    void ResolveProjectileAttack();
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
    bool CanStartAiming() const;
    void TryBeginAiming();
    void UpdateAimingState(float deltaTime);
    void FinishAiming();
    void UpdateAimFacing(float deltaTime);
    void UpdateAimingMovement(float deltaTime);
    void SetAimArrowVisible(bool visible);
    void UpdateAttackAimTrail();
    void HideAttackAimTrail();
    void UpdateAttackFacingLock(float deltaTime);
    void UpdateMovement(float deltaTime);
    void UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning);
    bool UsesReplicatedAnimator() const;
    void UpdateAnimatorFromLocalInput();
    void UpdateAnimatorFromReplicatedMotion(float deltaTime);
    void StartAttack(const RTBEngine::Math::Vector3& attackDirection);
    void FinishAttack();
    void HandleDeath(const HealthComponent::DeathEvent& eventData);
    void HandleCharacterDeath(const HealthComponent::DeathEvent& eventData) override;
    HealthComponent*& AccessHealthSlot() override;
    HealthComponent* PeekHealthSlot() const override;
    int GetCharacterTeam() const override;
    void FaceAttackDirection(const RTBEngine::Math::Vector3& attackDirection);
    void StopPlanarMotion() const;
    RTBEngine::Math::Vector3 GetDesiredMoveDirection(bool& outIsRunning) const;
    RTBEngine::Math::Vector3 GetAimTrailWorldOrigin(const RTBEngine::Math::Vector3& attackDirection) const;
    float GetProjectileTravelDistance() const;
    RTBEngine::Math::Vector3 GetAttackDirectionFromJoystick(const RTBEngine::Math::Vector2& joystickValue) const;
    RTBEngine::Math::Vector3 GetActiveAttackDirection() const;
    void SendNetworkInput();
    void TryProcessRemoteAttackInput();
    void PlayPredictedAttackVisual(const RTBEngine::Math::Vector3& attackDirection);
    void UpdatePredictedAttackVisual(float deltaTime);
    void ApplyDynamicPlanarMotion(RTBEngine::Physics::RigidBody* rigidBody,
                                  const RTBEngine::Math::Vector3& moveDirection,
                                  const RTBEngine::Math::Vector3& facingDirection,
                                  float moveSpeed,
                                  float deltaTime,
                                  float turnSpeedDegrees = -1.0f);
    void ApplyExternalKnockbackVelocity(RTBEngine::Physics::RigidBody* rigidBody, float deltaTime);

    RTBEngine::Math::Vector3 externalPlanarVelocity = RTBEngine::Math::Vector3::Zero();
    float externalPlanarDecay = 10.0f;
    std::uint32_t inputSequenceNumber = 0;
    std::uint32_t networkAttackSequence = 0;
    std::uint32_t lastProcessedRemoteAttackSequence = 0;
    RTBEngine::Math::Vector3 pendingNetworkAttackDirection = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 lastReplicatedWorldPosition = RTBEngine::Math::Vector3::Zero();
    bool hasReplicatedMotionSample = false;
    bool deathCameraFrozen = false;
    RTBEngine::Math::Vector3 frozenCameraWorldPosition = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Quaternion frozenCameraWorldRotation = RTBEngine::Math::Quaternion::Identity();
};
