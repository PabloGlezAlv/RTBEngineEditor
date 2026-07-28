#pragma once

#include "CharacterBase.h"
#include "HealthComponent.h"
#include "ICharacterStatReceiver.h"

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

    namespace Scene {
        class FreeLookCamera;
        class GameObject;
        class NetworkIdentity;
        class RigidBodyComponent;
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

class CharacterAbility;
class CharacterDefinition;
class PlayerAmmoSystem;
class PlayerSpecialAttackCharge;
class PlayerSpecialBeamAttack;

class ThirdPersonCharacterController : public PlayableCharacterController, public ICharacterStatReceiver
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
    void PlayReplicatedAttackVisual(const RTBEngine::Math::Vector3& attackDirection);
    void ApplyCombatAnimationOverrides(const std::string& aimDrawFbx,
                                       const std::string& aimLoopFbx,
                                       const std::string& attackFbx);
    void ApplyCharacterStats(const CharacterDefinition& definition) override;
    void RefreshAfterSpawn();

    RTBEngine::Math::Vector3 GetPlanarAttackDirectionFromJoystick(
        const RTBEngine::Math::Vector2& joystickValue) const;
    void FaceTowardPlanarDirection(const RTBEngine::Math::Vector3& direction, float deltaTime);

    RTBEngine::Scene::GameObject* cameraObject = nullptr;
    HealthComponent* health = nullptr;
    int team = static_cast<int>(CharacterTeam::Player);
    float moveSpeed = 4.0f;
    float sprintMultiplier = 1.75f;
    float turnSpeed = 720.0f;
    float cameraDistance = 11.0f;
    RTBEngine::Math::Vector3 cameraFocusOffset = RTBEngine::Math::Vector3(0.0f, 1.2f, 0.0f);
    RTBEngine::Animation::Animator* animator = nullptr;
    CharacterAbility* attackAbility = nullptr;
    RTBEngine::UI::UIJoystick* attackJoystick = nullptr;
    RTBEngine::Scene::TrailRenderer* attackAimTrail = nullptr;
    float aimTrailForwardOffset = 0.40f;
    float aimTrailHeightOffset = 0.0f;
    float aimTrailWallClipRadius = 0.45f;
    RTBEngine::Scene::GameObject* aimArrowVisual = nullptr;

    RTB_COMPONENT(ThirdPersonCharacterController)

private:
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
    bool usingMouseAim = false;
    bool wasMouseAiming = false;
    RTBEngine::Math::Vector2 cachedAttackJoystickValue = RTBEngine::Math::Vector2::Zero();
    float attackStateElapsed = 0.0f;
    bool attackAbilitySafetyExpired = false;
    RTBEngine::Scripting::LatentActionHandle attackSafetyHandle;
    bool missingHealthWarningShown = false;
    bool missingAttackAbilityWarningShown = false;
    bool missingAnimatorWarningShown = false;
    bool missingCameraWarningShown = false;
    bool missingAttackAimTrailWarningShown = false;
    RTBEngine::UI::UIJoystick* subscribedAttackJoystick = nullptr;
    RTBEngine::Core::EventSubscription attackJoystickReleaseSubscription;
    RTBEngine::Core::EventSubscription aimDrawFinishedSubscription;
    RTBEngine::Core::EventSubscription attackFinishedSubscription;
    bool attackClipFinished = false;
    RTBEngine::Math::Vector3 activeAttackDirection = RTBEngine::Math::Vector3::Zero();

    void ClampSettings();
    void ValidateRequiredReferences();
    void CacheGameplayReferences();
    void DisableCompetingCameraController();
    void ApplyCameraFollowTransform();
    void ApplySpectateCameraFollow(RTBEngine::Scene::GameObject* targetPawn);
    void EnsureAnimationReady();
    void ForceStartLocomotionAnimation();
    void RebindHealthSubscription();
    void UnsubscribeFromHealth();
    void RebindAttackJoystickSubscription();
    void UnsubscribeFromAttackJoystick();
    void RebindAnimatorKeySubscriptions();
    void UnsubscribeFromAnimatorKeys();
    void HandleJoystickAttackReleased(const RTBEngine::Math::Vector2& joystickValue);
    void HandleAttackReleasedWithDirection(const RTBEngine::Math::Vector3& attackDirection);
    bool SupportsMouseAimInput() const;
    bool CanStartAiming() const;
    void TryBeginAiming();
    void UpdateAimingState(float deltaTime);
    void FinishAiming();
    void UpdateAimFacing(float deltaTime);
    void UpdateAimingMovement(float deltaTime);
    void UpdateSpecialAttackAimingMovement(float deltaTime, const RTBEngine::Math::Vector3& aimDirection);
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
    RTBEngine::Math::Vector3 GetAimTrailCombatOrigin(const RTBEngine::Math::Vector3& attackDirection) const;
    float GetAimTrailEffectiveLength(
        const RTBEngine::Math::Vector3& attackDirection,
        float maxLength) const;
    float GetAimRangeForVisual() const;
    RTBEngine::Math::Vector3 GetAttackDirectionFromJoystick(const RTBEngine::Math::Vector2& joystickValue) const;
    RTBEngine::Math::Vector3 GetAttackDirectionFromCamera() const;
    void UpdateAimFacingToward(const RTBEngine::Math::Vector3& aimDirection, float deltaTime);
    RTBEngine::Math::Vector3 GetActiveAttackDirection() const;
    void SendNetworkInput();
    void TryProcessRemoteAttackInput();
    void PlayPredictedAttackVisual(const RTBEngine::Math::Vector3& attackDirection);
    void UpdatePredictedAttackVisual(float deltaTime);
    void PollAttackCompletion(float deltaTime);
    void ScheduleAttackSafetyTimeout();
    void ClearAttackSafetyTimeout();
    void ApplyDynamicPlanarMotion(RTBEngine::Physics::RigidBody* rigidBody,
                                  const RTBEngine::Math::Vector3& moveDirection,
                                  const RTBEngine::Math::Vector3& facingDirection,
                                  float moveSpeed,
                                  float deltaTime,
                                  float turnSpeedDegrees = -1.0f);
    void ApplyExternalKnockbackVelocity(RTBEngine::Physics::RigidBody* rigidBody, float deltaTime);

    PlayerSpecialBeamAttack* specialBeamAttack = nullptr;
    PlayerSpecialAttackCharge* specialAttackCharge = nullptr;
    PlayerAmmoSystem* ammoSystem = nullptr;
    RTBEngine::Scene::RigidBodyComponent* rigidBodyComponent = nullptr;
    RTBEngine::Scene::NetworkIdentity* networkIdentity = nullptr;
    RTBEngine::Scene::FreeLookCamera* freeLookCamera = nullptr;
    bool competingCameraDisabled = false;

    RTBEngine::Math::Vector3 externalPlanarVelocity = RTBEngine::Math::Vector3::Zero();
    float externalPlanarDecay = 10.0f;
    std::uint32_t inputSequenceNumber = 0;
    std::uint32_t networkAttackSequence = 0;
    std::uint32_t lastProcessedRemoteAttackSequence = 0;
    RTBEngine::Math::Vector3 pendingNetworkAttackDirection = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 lastReplicatedWorldPosition = RTBEngine::Math::Vector3::Zero();
    bool hasReplicatedMotionSample = false;
    float replicatedPlanarSpeed = 0.0f;
    bool replicatedAnimatorReady = false;
    bool deathCameraFrozen = false;
    RTBEngine::Math::Vector3 frozenCameraWorldPosition = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Quaternion frozenCameraWorldRotation = RTBEngine::Math::Quaternion::Identity();
};
