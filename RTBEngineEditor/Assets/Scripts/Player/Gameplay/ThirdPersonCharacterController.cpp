#include "ThirdPersonCharacterController.h"

#include "CharacterDefinition.h"
#include "HealthComponent.h"
#include <RTBEngine/Scene/NetworkIdentity.h>
#include "OnlineGameNetMessages.h"

#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include "PauseMenuController.h"
#include "PlayerBasicAttackDriver.h"
#include "PlayerCameraBasis.h"
#include "PlayerCombatNet.h"
#include "PlayerFollowCamera.h"
#include "PlayerPawnMotor.h"
#include "PlayerSpecialAttackCharge.h"
#include "PlayerSpecialAttackUtil.h"
#include "PlayerRegistry.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Animation/AnimationClip.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/CameraComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/TrailRenderer.h>
#include <RTBEngine/Scene/OcclusionTarget.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Physics/PhysicsUtils.h>
#include <RTBEngine/Physics/PhysicsWorld.h>
#include <RTBEngine/UI/Elements/UIJoystick.h>

#include <algorithm>
#include <cmath>

using ThisClass = ThirdPersonCharacterController;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;
    constexpr float kDegToRad = kPi / 180.0f;
    constexpr float kDirectionEpsilon = 0.0001f;
    constexpr const char* kAnimIdle = "Idle";
    constexpr const char* kAnimDeath = "Death";

    float ClampAngleDegrees(float angle)
    {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }

    float MoveTowardsAngleDegrees(float current, float target, float maxDelta)
    {
        const float delta = ClampAngleDegrees(target - current);
        if (std::abs(delta) <= maxDelta) {
            return target;
        }

        return current + (delta > 0.0f ? maxDelta : -maxDelta);
    }

    bool HasMovementInput(const RTBEngine::Math::Vector3& value)
    {
        return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
    }

    void SetOcclusionTargetEnabled(RTBEngine::Scene::GameObject* gameObject, bool enabled)
    {
        if (!gameObject) {
            return;
        }

        if (RTBEngine::Scene::OcclusionTarget* target = gameObject->GetComponent<RTBEngine::Scene::OcclusionTarget>()) {
            target->targetEnabled = enabled;
        }
    }

    RTBEngine::Scene::GameObject* FindNextAliveTeammatePawn(const RTBEngine::Scene::GameObject* localOwner, int localPlayerSlot)
    {
        RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
        if (!scene || localPlayerSlot < 0) {
            return nullptr;
        }

        const std::vector<RTBEngine::Online::OnlineUserId> members =
            RTBEngine::Online::OnlineGameplayNet::GetOrderedLobbyMembers();
        if (members.size() < 2) {
            return nullptr;
        }

        const std::size_t memberCount = members.size();
        for (std::size_t offset = 1; offset < memberCount; ++offset) {
            const int candidateSlot = static_cast<int>((static_cast<std::size_t>(localPlayerSlot) + offset) % memberCount);
            RTBEngine::Scene::GameObject* candidatePawn =
                PlayerRegistry::GetInstance().FindBySlot(candidateSlot);
            if (!candidatePawn || candidatePawn == localOwner) {
                continue;
            }

            ThirdPersonCharacterController* controller =
                candidatePawn->GetComponent<ThirdPersonCharacterController>();
            if (!controller || controller->GetTeam() != static_cast<int>(CharacterTeam::Player)) {
                continue;
            }

            HealthComponent* health = controller->GetHealth();
            if (health && !health->IsDead()) {
                return candidatePawn;
            }
        }

        return nullptr;
    }

}

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_GAMEOBJECT(cameraObject)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_SERIALIZED_RANGE(team, 0, 8)
    RTB_PROPERTY_COMPONENT(animator, Animator)
RTB_END_REGISTER(ThirdPersonCharacterController)

void ThirdPersonCharacterController::OnStart()
{
    replicatedAnimatorReady = false;
    locomotionAnimator.ResetReplicatedMotionSample();
    ClampSettings();
    CacheGameplayReferences();
    ValidateCharacterHealth();
    DisableCompetingCameraController();
    RebindHealthSubscription();
    if (IsLocallyControlled()) {
        ApplyCameraFollowTransform();
        RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);
    }

    if (health && health->IsDead()) {
        HandleDeath(health->GetLastDeathEvent());
    }

    EnsureAnimationReady();
}

void ThirdPersonCharacterController::OnUpdate(float deltaTime)
{
    if (!IsLocallyControlled() || dead) {
        HideCombatAimVisuals();
        return;
    }

    if (basicAttackDriver && basicAttackDriver->IsAttacking()) {
        basicAttackDriver->HideAimVisuals();
        basicAttackDriver->UpdateLocalPredictedVisual(deltaTime);
        return;
    }

    if (animator && !animator->AreBoneGOsCreated()) {
        EnsureAnimationReady();
    }
}

void ThirdPersonCharacterController::OnFixedUpdate(float fixedDeltaTime)
{
    // Client local pawn: send input to host and drive animator locally (transform comes from network).
    if (dead) {
        HideCombatAimVisuals();
        StopPlanarMotion();
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() &&
        IsLocallyControlled()) {
        if (specialAttack && specialAttack->IsActive()) {
            StopPlanarMotion();
            specialAttack->ApplyMovementLock(fixedDeltaTime);
            UpdateAnimatorLocomotion(false, false);
            SendNetworkInput();
            return;
        }

        if (IsStunned()) {
            StopPlanarMotion();
            UpdateAnimatorLocomotion(false, false);
            SendNetworkInput();
            return;
        }

        if (specialAttackCharge) {
            RTBEngine::Math::Vector3 specialAimDirection = RTBEngine::Math::Vector3::Zero();
            if (specialAttackCharge->TryGetSpecialAimDirection(specialAimDirection)) {
                UpdateSpecialAttackAimingMovement(fixedDeltaTime, specialAimDirection);
                SendNetworkInput();
                return;
            }
        }

        if (basicAttackDriver && basicAttackDriver->IsAiming()) {
            basicAttackDriver->UpdateFixedAiming(fixedDeltaTime);
        }
        SendNetworkInput();
        return;
    }

    // Proxies without host simulation skip movement (animator for clients runs in LateUpdate).
    if (!HasSimulationAuthority()) {
        return;
    }

    if (specialAttack && specialAttack->IsActive()) {
        HideCombatAimVisuals();
        StopPlanarMotion();
        specialAttack->ApplyMovementLock(fixedDeltaTime);
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    if (IsStunned()) {
        HideCombatAimVisuals();
        StopPlanarMotion();
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    if (specialAttackCharge) {
        RTBEngine::Math::Vector3 specialAimDirection = RTBEngine::Math::Vector3::Zero();
        if (specialAttackCharge->TryGetSpecialAimDirection(specialAimDirection)) {
            UpdateSpecialAttackAimingMovement(fixedDeltaTime, specialAimDirection);
            return;
        }
    }

    if (PauseMenuController::IsAnyMenuOpen()) {
        if (basicAttackDriver) {
            basicAttackDriver->HideAimVisuals();
            basicAttackDriver->CancelForMenu();
        }
        StopPlanarMotion();
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    if (basicAttackDriver && basicAttackDriver->IsAttacking()) {
        basicAttackDriver->UpdateFixedAttacking(fixedDeltaTime);
        return;
    }

    if (basicAttackDriver && basicAttackDriver->IsAiming()) {
        basicAttackDriver->UpdateFixedAiming(fixedDeltaTime);
        return;
    }

    if (basicAttackDriver &&
        RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        basicAttackDriver->TryProcessRemoteAttack();
    }

    UpdateMovement(fixedDeltaTime);
}

void ThirdPersonCharacterController::OnLateUpdate(float deltaTime)
{
    if (!IsLocallyControlled()) {
        HideCombatAimVisuals();
        // Runs after NetworkTransform when that component is listed earlier on the pawn.
        if (UsesReplicatedAnimator()) {
            if (!replicatedAnimatorReady && animator) {
                replicatedAnimatorReady = true;
            }
            UpdateAnimatorFromReplicatedMotion(deltaTime);
        }
    } else {
        DisableCompetingCameraController();

        if (dead) {
            RTBEngine::Scene::GameObject* spectateTarget = nullptr;
            if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() && owner) {
                const RTBEngine::Scene::NetworkIdentity* identity = GetNetworkIdentity();
                const int localSlot = identity ? identity->networkPlayerSlot : -1;
                spectateTarget = FindNextAliveTeammatePawn(owner, localSlot);
            }

            if (spectateTarget) {
                ApplySpectateCameraFollow(spectateTarget);
            } else if (followCamera && followCamera->ShouldHoldFollowWhileDead()) {
                followCamera->ApplyFrozenTransform();
            }

            return;
        }

        ApplyCameraFollowTransform();

        if (basicAttackDriver) {
            basicAttackDriver->UpdateLocalAimInput(deltaTime);
        }
    }
}

void ThirdPersonCharacterController::OnValidate()
{
    ClampSettings();
    CacheGameplayReferences();
    ValidateCharacterHealth();
    DisableCompetingCameraController();
    UpdateAnimatorLocomotion(false, false);
    if (IsLocallyControlled()) {
        ApplyCameraFollowTransform();
    }
}

void ThirdPersonCharacterController::OnDestroy()
{
    UnsubscribeFromHealth();
    if (owner) {
        PlayerRegistry::GetInstance().Unregister(owner);
    }
}

void ThirdPersonCharacterController::ClampSettings()
{
    team = std::max(0, team);
}

void ThirdPersonCharacterController::CacheGameplayReferences()
{
    specialAttack = ResolvePlayerSpecialAttack(owner);
    specialAttackCharge = owner->GetComponent<PlayerSpecialAttackCharge>();
    pawnMotor = owner->GetComponent<PlayerPawnMotor>();
    followCamera = nullptr;
    if (cameraObject) {
        followCamera = cameraObject->GetComponent<PlayerFollowCamera>();
    }
    CacheCharacterBaseReferences();

    basicAttackDriver = owner->GetComponent<PlayerBasicAttackDriver>();
    if (basicAttackDriver) {
        basicAttackDriver->BindController(this);
    }
    locomotionAnimator.Bind(animator);
}

void ThirdPersonCharacterController::DisableCompetingCameraController()
{
    if (followCamera) {
        followCamera->DisableCompetingCameraController();
    }
}

void ThirdPersonCharacterController::ForceDeathState()
{
    if (health && health->IsDead()) {
        HandleDeath(health->GetLastDeathEvent());
        return;
    }

    HealthComponent::DeathEvent eventData;
    HandleDeath(eventData);
}

void ThirdPersonCharacterController::ReviveFromDeath()
{
    if (!dead) {
        return;
    }

    dead = false;
    if (followCamera) {
        followCamera->ClearFreeze();
    }
    if (basicAttackDriver) {
        basicAttackDriver->ResetAfterRevive();
    }
    SetOcclusionTargetEnabled(owner, true);

    UpdateAnimatorLocomotion(false, false);
}

void ThirdPersonCharacterController::ApplyCameraFollowTransform()
{
    if (!followCamera) {
        return;
    }

    if (dead && followCamera->ShouldHoldFollowWhileDead() && IsLocallyControlled()) {
        return;
    }

    followCamera->ApplyFollow(owner);
}

void ThirdPersonCharacterController::ApplySpectateCameraFollow(RTBEngine::Scene::GameObject* targetPawn)
{
    if (followCamera) {
        followCamera->ApplySpectate(owner, targetPawn);
    }
}

RTBEngine::Animation::Animator* ThirdPersonCharacterController::EnsureAnimator()
{
    if (!animator && owner) {
        animator = owner->GetComponentInChildren<RTBEngine::Animation::Animator>();
        locomotionAnimator.Bind(animator);
    }

    return animator;
}

void ThirdPersonCharacterController::HideCombatAimVisuals()
{
    if (basicAttackDriver) {
        basicAttackDriver->HideAimVisuals();
    }
}

bool ThirdPersonCharacterController::IsCombatBusy() const
{
    return basicAttackDriver && basicAttackDriver->IsBusy();
}

void ThirdPersonCharacterController::QueueNetworkAttack(
    const RTBEngine::Math::Vector3& attackDirection)
{
    ++networkAttackSequence;
    pendingNetworkAttackDirection = attackDirection;
}

void ThirdPersonCharacterController::EnsureAnimationReady()
{
    if (!animator) {
        return;
    }

    RTBEngine::Scene::Scene* scene =
        RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (scene && animator->HasBones() && !animator->AreBoneGOsCreated()) {
        animator->CreateBoneGameObjects(scene);
    }

    if (!animator->HasKey(kAnimIdle)) {
        RTB_WARN("[ThirdPersonCharacterController] Idle animation key is not configured on '" +
                 owner->GetName() + "'. Locomotion animations will not play.");
    }

    ForceStartLocomotionAnimation();
}

void ThirdPersonCharacterController::ForceStartLocomotionAnimation()
{
    locomotionAnimator.ForceStartLocomotionAnimation(owner);
}

void ThirdPersonCharacterController::UpdateAnimatorFromReplicatedMotion(float deltaTime)
{
    const float baseMoveSpeed = pawnMotor ? pawnMotor->GetMoveSpeed() : 0.0f;
    const float sprintMultiplier = pawnMotor ? pawnMotor->GetSprintMultiplier() : 1.0f;
    locomotionAnimator.SampleReplicatedMotion(
        owner,
        deltaTime,
        !dead && !IsCombatBusy(),
        baseMoveSpeed,
        sprintMultiplier);
}

void ThirdPersonCharacterController::UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning)
{
    locomotionAnimator.UpdateLocomotion(!dead && !IsCombatBusy(), hasMovementInput, isRunning);
}

void ThirdPersonCharacterController::RefreshLocomotionAnimation()
{
    bool isRunning = false;
    const RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    UpdateAnimatorLocomotion(HasMovementInput(desiredMove), isRunning);
}

void ThirdPersonCharacterController::RebindHealthSubscription()
{
    RebindCharacterDeathSubscription();
}

void ThirdPersonCharacterController::UnsubscribeFromHealth()
{
    UnsubscribeCharacterDeath();
}

void ThirdPersonCharacterController::UpdateAimFacingToward(
    const RTBEngine::Math::Vector3& aimDirection,
    float deltaTime)
{
    if (!HasMovementInput(aimDirection)) {
        return;
    }

    RTBEngine::Math::Vector3 planarDirection = aimDirection;
    planarDirection.y = 0.0f;
    planarDirection.Normalize();

    const float targetYaw = -std::atan2(planarDirection.x, planarDirection.z) * kRadToDeg;
    RTBEngine::Math::Vector3 currentEuler = owner->GetTransform().GetRotation().ToEulerAngles();
    const float currentYaw = currentEuler.y * kRadToDeg;
    const float nextYaw = MoveTowardsAngleDegrees(
        currentYaw,
        targetYaw,
        (pawnMotor ? pawnMotor->GetTurnSpeed() : 0.0f) * deltaTime);
    const RTBEngine::Math::Quaternion nextRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, nextYaw * kDegToRad, 0.0f);

    owner->GetTransform().SetRotation(nextRotation);
    if (pawnMotor) {
        pawnMotor->SyncDynamicBodyRotation(nextRotation);
    }
}

void ThirdPersonCharacterController::ApplyAimMovement(
    const RTBEngine::Math::Vector3& requestedAimFacing,
    float deltaTime)
{
    bool isRunning = false;
    const RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    const bool hasMovementInput = HasMovementInput(desiredMove);

    RTBEngine::Math::Vector3 aimFacingDirection = requestedAimFacing;
    if (!HasMovementInput(aimFacingDirection) && hasMovementInput) {
        aimFacingDirection = desiredMove;
    }

    const bool useDynamicRigidBody = pawnMotor && pawnMotor->UsesDynamicRigidBody();
    const float speed = pawnMotor && hasMovementInput
        ? pawnMotor->ComputePlanarSpeed(isRunning)
        : 0.0f;

    if (useDynamicRigidBody) {
        pawnMotor->ApplyDynamicPlanarMotion(
            desiredMove,
            aimFacingDirection,
            speed,
            deltaTime);
        return;
    }

    UpdateAimFacingToward(aimFacingDirection, deltaTime);

    if (hasMovementInput) {
        owner->GetTransform().SetPosition(
            owner->GetTransform().GetPosition() + desiredMove * speed * deltaTime);
    }
}

void ThirdPersonCharacterController::UpdateSpecialAttackAimingMovement(
    float deltaTime,
    const RTBEngine::Math::Vector3& aimDirection)
{
    if (!HasMovementInput(aimDirection)) {
        return;
    }

    bool isRunning = false;
    const RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    const bool hasMovementInput = HasMovementInput(desiredMove);

    RTBEngine::Math::Vector3 aimFacingDirection = aimDirection;
    if (!HasMovementInput(aimFacingDirection) && hasMovementInput) {
        aimFacingDirection = desiredMove;
    }

    const bool useDynamicRigidBody = pawnMotor && pawnMotor->UsesDynamicRigidBody();
    const float speed = pawnMotor && hasMovementInput
        ? pawnMotor->ComputePlanarSpeed(isRunning)
        : 0.0f;

    if (useDynamicRigidBody) {
        pawnMotor->ApplyDynamicPlanarMotion(
            desiredMove,
            aimFacingDirection,
            speed,
            deltaTime);
        return;
    }

    UpdateAimFacingToward(aimFacingDirection, deltaTime);

    if (hasMovementInput) {
        owner->GetTransform().SetPosition(
            owner->GetTransform().GetPosition() + desiredMove * speed * deltaTime);
    }
}

void ThirdPersonCharacterController::ApplyAttackFacingLock(
    const RTBEngine::Math::Vector3& attackDirection,
    float deltaTime)
{
    if (!HasMovementInput(attackDirection)) {
        StopPlanarMotion();
        return;
    }

    bool isRunning = false;
    RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    const bool hasMovementInput = HasMovementInput(desiredMove);
    const float speed = pawnMotor && hasMovementInput
        ? pawnMotor->ComputePlanarSpeed(isRunning)
        : 0.0f;

    const bool useDynamicRigidBody = pawnMotor && pawnMotor->UsesDynamicRigidBody();

    FaceAttackDirection(attackDirection);

    if (useDynamicRigidBody) {
        pawnMotor->ApplyDynamicPlanarMotion(
            desiredMove,
            attackDirection,
            speed,
            deltaTime,
            0.0f);
        return;
    }

    if (hasMovementInput) {
        owner->GetTransform().SetPosition(
            owner->GetTransform().GetPosition() + desiredMove * speed * deltaTime);
    }
}

void ThirdPersonCharacterController::UpdateMovement(float deltaTime)
{
    bool isRunning = false;
    RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    const bool hasMovementInput = HasMovementInput(desiredMove);
    const bool useDynamicRigidBody = pawnMotor && pawnMotor->UsesDynamicRigidBody();

    if (!hasMovementInput) {
        if (useDynamicRigidBody) {
            pawnMotor->ApplyDynamicPlanarMotion(
                RTBEngine::Math::Vector3::Zero(),
                RTBEngine::Math::Vector3::Zero(),
                0.0f,
                deltaTime);
        }

        UpdateAnimatorLocomotion(false, false);
        return;
    }

    const float speed = pawnMotor ? pawnMotor->ComputePlanarSpeed(isRunning) : 0.0f;

    if (useDynamicRigidBody) {
        // Facing is applied only via planar angular velocity — calling UpdateAimFacingToward
        // here fought the rigid body and caused stuttering turns.
        pawnMotor->ApplyDynamicPlanarMotion(
            desiredMove,
            desiredMove,
            speed,
            deltaTime);
    } else {
        const float targetYaw = -std::atan2(desiredMove.x, desiredMove.z) * kRadToDeg;
        RTBEngine::Math::Vector3 currentEuler = owner->GetTransform().GetRotation().ToEulerAngles();
        const float currentYaw = currentEuler.y * kRadToDeg;
        const float turnRate = pawnMotor ? pawnMotor->GetTurnSpeed() : 0.0f;
        const float nextYaw = MoveTowardsAngleDegrees(currentYaw, targetYaw, turnRate * deltaTime);
        const RTBEngine::Math::Quaternion nextRotation =
            RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, nextYaw * kDegToRad, 0.0f);

        owner->GetTransform().SetPosition(
            owner->GetTransform().GetPosition() + desiredMove * speed * deltaTime);
        owner->GetTransform().SetRotation(nextRotation);
    }

    UpdateAnimatorLocomotion(true, isRunning);
}

bool ThirdPersonCharacterController::UsesReplicatedAnimator() const
{
    // Online clients drive locomotion visuals from replicated transforms (host-authoritative),
    // including the local pawn on this machine.
    return RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !HasSimulationAuthority();
}

void ThirdPersonCharacterController::PlayReplicatedAttackVisual(
    const RTBEngine::Math::Vector3& attackDirection)
{
    if (basicAttackDriver) {
        basicAttackDriver->PlayReplicatedVisual(attackDirection);
    }
}

void ThirdPersonCharacterController::HandleDeath(const HealthComponent::DeathEvent& /*eventData*/)
{
    if (dead) {
        return;
    }

    dead = true;
    if (basicAttackDriver) {
        basicAttackDriver->CancelForDeath();
    }
    StopPlanarMotion();
    SetOcclusionTargetEnabled(owner, false);

    if (IsLocallyControlled() && followCamera) {
        const bool isOnline = RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby();
        const RTBEngine::Scene::NetworkIdentity* identity = GetNetworkIdentity();
        const int localSlot = identity ? identity->networkPlayerSlot : -1;
        const bool canSpectate = isOnline && FindNextAliveTeammatePawn(owner, localSlot) != nullptr;

        if (!canSpectate) {
            followCamera->FreezeAtCurrent();
        }
    }

    if (animator && animator->HasKey(kAnimDeath)) {
        animator->PlayKey(kAnimDeath, false);
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        if (const RTBEngine::Scene::NetworkIdentity* identity = GetNetworkIdentity()) {
            if (identity && identity->networkPlayerSlot >= 0) {
                GameNet::OnlineGameNetSubsystem::BroadcastPlayerDeath(identity->networkPlayerSlot);
            }
        }
    }
}

void ThirdPersonCharacterController::HandleCharacterDeath(const HealthComponent::DeathEvent& eventData)
{
    HandleDeath(eventData);
}

HealthComponent*& ThirdPersonCharacterController::AccessHealthSlot()
{
    return health;
}

HealthComponent* ThirdPersonCharacterController::PeekHealthSlot() const
{
    return health;
}

int ThirdPersonCharacterController::GetCharacterTeam() const
{
    return team;
}

void ThirdPersonCharacterController::ApplyPlanarKnockback(
    const RTBEngine::Math::Vector3& direction,
    float strength)
{
    AddPlanarKnockback(direction, strength);
}

void ThirdPersonCharacterController::AddPlanarKnockback(
    const RTBEngine::Math::Vector3& direction,
    float strength)
{
    if (pawnMotor) {
        pawnMotor->AddPlanarKnockback(direction, strength);
    }
}

void ThirdPersonCharacterController::FaceAttackDirection(const RTBEngine::Math::Vector3& attackDirection)
{
    if (!HasMovementInput(attackDirection)) {
        return;
    }

    RTBEngine::Math::Vector3 planarDirection = attackDirection;
    planarDirection.y = 0.0f;
    planarDirection.Normalize();

    const float targetYaw = -std::atan2(planarDirection.x, planarDirection.z) * kRadToDeg;
    const RTBEngine::Math::Quaternion targetRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, targetYaw * kDegToRad, 0.0f);

    owner->GetTransform().SetRotation(targetRotation);
    if (pawnMotor) {
        pawnMotor->SyncDynamicBodyRotation(targetRotation);
    }
}

void ThirdPersonCharacterController::StopPlanarMotion()
{
    if (pawnMotor) {
        pawnMotor->StopPlanarMotion();
    }
}

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetDesiredMoveDirection(bool& outIsRunning) const
{
    outIsRunning = false;

    if (const RTBEngine::Scene::NetworkIdentity* identity = GetNetworkIdentity()) {
        // Host simulating a remote client's pawn: read last PlayerInput from OnlineGameplayNet.
        if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
            RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() &&
            !identity->IsLocallyControlled()) {
            RTBEngine::Online::OnlineGameplayNet::PlayerInputSnapshot remoteInput;
            if (RTBEngine::Online::OnlineGameplayNet::TryGetLatestInputForUser(identity->networkOwnerUserId, remoteInput)) {
                RTBEngine::Math::Vector3 desiredMove(remoteInput.moveX, 0.0f, remoteInput.moveZ);
                if (!HasMovementInput(desiredMove)) {
                    return RTBEngine::Math::Vector3::Zero();
                }

                desiredMove.Normalize();
                outIsRunning = remoteInput.sprint;
                return desiredMove;
            }

            return RTBEngine::Math::Vector3::Zero();
        }
    }

    RTBEngine::Input::InputManager& input = RTBEngine::Input::InputManager::GetInstance();
    RTBEngine::Math::Vector3 forward = RTBEngine::Math::Vector3::Forward();
    RTBEngine::Math::Vector3 right = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
    PlayerCameraBasis::GetPlanarBasis(forward, right);

    RTBEngine::Math::Vector3 desiredMove = RTBEngine::Math::Vector3::Zero();
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::W)) desiredMove += forward;
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::S)) desiredMove -= forward;
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::D)) desiredMove += right;
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::A)) desiredMove -= right;

    if (!HasMovementInput(desiredMove)) {
        return RTBEngine::Math::Vector3::Zero();
    }

    desiredMove.Normalize();
    outIsRunning = input.IsKeyPressed(RTBEngine::Input::KeyCode::LeftShift);
    return desiredMove;
}

void ThirdPersonCharacterController::ApplyCombatAnimationOverrides(
    const std::string& aimDrawFbx,
    const std::string& aimLoopFbx,
    const std::string& attackFbx)
{
    if (basicAttackDriver) {
        basicAttackDriver->ApplyCombatAnimationOverrides(aimDrawFbx, aimLoopFbx, attackFbx);
    }
}

void ThirdPersonCharacterController::ApplyCharacterStats(const CharacterDefinition& definition)
{
    if (pawnMotor) {
        pawnMotor->SetMoveStats(
            definition.moveSpeed,
            definition.sprintMultiplier,
            definition.turnSpeed);
    }
    ApplyCombatAnimationOverrides(
        definition.aimDrawAnimationFbx,
        definition.aimLoopAnimationFbx,
        definition.attackAnimationFbx);
}

void ThirdPersonCharacterController::RefreshAfterSpawn()
{
    ClampSettings();
    CacheGameplayReferences();
    EnsureAnimationReady();
    DisableCompetingCameraController();
    RebindHealthSubscription();
    if (basicAttackDriver) {
        basicAttackDriver->BindController(this);
        basicAttackDriver->RefreshBindings();
    }

    RefreshLocomotionAnimation();
    ForceStartLocomotionAnimation();

    if (IsLocallyControlled()) {
        ApplyCameraFollowTransform();
    }
}

void ThirdPersonCharacterController::ClearLocalOnlyInputAndCamera()
{
    if (basicAttackDriver) {
        basicAttackDriver->SetAttackJoystick(nullptr);
    }
    if (cameraObject) {
        cameraObject->SetActive(false);
        cameraObject = nullptr;
    }
    followCamera = nullptr;
}

RTBEngine::UI::UIJoystick* ThirdPersonCharacterController::GetAttackJoystick() const
{
    return basicAttackDriver ? basicAttackDriver->GetAttackJoystick() : nullptr;
}

void ThirdPersonCharacterController::SetAttackJoystick(RTBEngine::UI::UIJoystick* joystick)
{
    if (basicAttackDriver) {
        basicAttackDriver->SetAttackJoystick(joystick);
    }
}

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetPlanarAttackDirectionFromJoystick(
    const RTBEngine::Math::Vector2& joystickValue) const
{
    if (basicAttackDriver) {
        return basicAttackDriver->GetPlanarAttackDirectionFromJoystick(joystickValue);
    }

    return RTBEngine::Math::Vector3::Zero();
}

void ThirdPersonCharacterController::FaceTowardPlanarDirection(
    const RTBEngine::Math::Vector3& direction,
    float deltaTime)
{
    if (!HasMovementInput(direction)) {
        return;
    }

    if (pawnMotor && pawnMotor->UsesDynamicRigidBody()) {
        pawnMotor->ApplyDynamicPlanarMotion(
            RTBEngine::Math::Vector3::Zero(),
            direction,
            0.0f,
            deltaTime);
        return;
    }

    UpdateAimFacingToward(direction, deltaTime);
}

void ThirdPersonCharacterController::SendNetworkInput()
{
    bool isRunning = false;
    const RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    PlayerCombatNet::SendLocalInput(
        owner,
        desiredMove,
        isRunning,
        inputSequenceNumber,
        networkAttackSequence,
        pendingNetworkAttackDirection);
}
