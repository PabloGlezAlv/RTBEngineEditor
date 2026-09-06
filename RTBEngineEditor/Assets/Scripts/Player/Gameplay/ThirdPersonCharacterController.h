#pragma once

#include "CharacterBase.h"
#include "CharacterCombatOrigins.h"
#include "HealthComponent.h"
#include "ICharacterStatReceiver.h"
#include "PlayerLocomotionAnimator.h"

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
class PlayerBasicAttackDriver;
class PlayerFollowCamera;
class PlayerPawnMotor;
class PlayerSpecialAttackCharge;
class IPlayerSpecialAttack;

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
    void ApplyPlanarKnockback(const RTBEngine::Math::Vector3& direction, float strength) override;
    void PlayReplicatedAttackVisual(const RTBEngine::Math::Vector3& attackDirection) override;
    void ApplyCombatAnimationOverrides(const std::string& aimDrawFbx,
                                       const std::string& aimLoopFbx,
                                       const std::string& attackFbx);
    void ApplyCharacterStats(const CharacterDefinition& definition) override;
    void RefreshAfterSpawn();

    bool IsDead() const { return dead; }
    bool IsCombatBusy() const;

    // Movement intents: callers supply the direction, the pawn decides how to apply it.
    void ApplyAimMovement(const RTBEngine::Math::Vector3& aimFacingDirection, float deltaTime);
    void ApplyAttackFacingLock(const RTBEngine::Math::Vector3& attackDirection, float deltaTime);
    void FaceAttackDirection(const RTBEngine::Math::Vector3& attackDirection);
    void FaceTowardPlanarDirection(const RTBEngine::Math::Vector3& direction, float deltaTime);
    void StopPlanarMotion();
    RTBEngine::Math::Vector3 GetPlanarAttackDirectionFromJoystick(
        const RTBEngine::Math::Vector2& joystickValue) const;

    void RefreshLocomotionAnimation();
    void ForceStartLocomotionAnimation();
    bool UsesReplicatedAnimator() const;
    void UpdateAnimatorFromReplicatedMotion(float deltaTime);
    RTBEngine::Animation::Animator* EnsureAnimator();

    void QueueNetworkAttack(const RTBEngine::Math::Vector3& attackDirection);

    RTBEngine::Scene::GameObject* GetCameraObject() const { return cameraObject; }
    void SetCameraObject(RTBEngine::Scene::GameObject* camera) { cameraObject = camera; }
    RTBEngine::Animation::Animator* GetAnimator() const { return animator; }
    RTBEngine::UI::UIJoystick* GetAttackJoystick() const;
    void SetAttackJoystick(RTBEngine::UI::UIJoystick* joystick);
    void ClearLocalOnlyInputAndCamera();

    RTB_COMPONENT(ThirdPersonCharacterController)
    RTB_INHERITS(PlayableCharacterController, CharacterBase)

    RTB_SERIALIZE()
    RTBEngine::Scene::GameObject* cameraObject = nullptr;
    RTB_SERIALIZE()
    HealthComponent* health = nullptr;
    RTB_SERIALIZE()
    int team = static_cast<int>(CharacterTeam::Player);
    RTB_SERIALIZE()
    RTBEngine::Animation::Animator* animator = nullptr;

private:
    bool dead = false;

    void ClampSettings();
    void CacheGameplayReferences();
    void DisableCompetingCameraController();
    void ApplyCameraFollowTransform();
    void ApplySpectateCameraFollow(RTBEngine::Scene::GameObject* targetPawn);
    void EnsureAnimationReady();
    void HideCombatAimVisuals();
    void RebindHealthSubscription();
    void UnsubscribeFromHealth();
    void UpdateSpecialAttackAimingMovement(float deltaTime, const RTBEngine::Math::Vector3& aimDirection);
    void UpdateMovement(float deltaTime);
    void UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning);
    void HandleDeath(const HealthComponent::DeathEvent& eventData);
    void HandleCharacterDeath(const HealthComponent::DeathEvent& eventData) override;
    HealthComponent*& AccessHealthSlot() override;
    HealthComponent* PeekHealthSlot() const override;
    int GetCharacterTeam() const override;
    RTBEngine::Math::Vector3 GetDesiredMoveDirection(bool& outIsRunning) const;
    void UpdateAimFacingToward(const RTBEngine::Math::Vector3& aimDirection, float deltaTime);
    void SendNetworkInput();

    IPlayerSpecialAttack* specialAttack = nullptr;
    PlayerSpecialAttackCharge* specialAttackCharge = nullptr;
    PlayerPawnMotor* pawnMotor = nullptr;
    PlayerFollowCamera* followCamera = nullptr;
    PlayerBasicAttackDriver* basicAttackDriver = nullptr;
    PlayerLocomotionAnimator locomotionAnimator;

    std::uint32_t inputSequenceNumber = 0;
    std::uint32_t networkAttackSequence = 0;
    RTBEngine::Math::Vector3 pendingNetworkAttackDirection = RTBEngine::Math::Vector3::Zero();
    bool replicatedAnimatorReady = false;
};
