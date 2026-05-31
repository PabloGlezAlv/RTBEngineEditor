#include "ThirdPersonCharacterController.h"

#include "HealthComponent.h"
#include <RTBEngine/ECS/NetworkIdentity.h>
#include "OnlineGameNetMessages.h"

#include <RTBEngine/ECS/NetworkIdentity.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include "PauseMenuController.h"
#include "ProjectileAttackAbility.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/CameraComponent.h>
#include <RTBEngine/ECS/CapsuleColliderComponent.h>
#include <RTBEngine/ECS/FreeLookCamera.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/SphereColliderComponent.h>
#include <RTBEngine/ECS/TrailRenderer.h>
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
    constexpr float kFixedCameraYawDegrees = 0.0f;
    constexpr float kFixedCameraPitchDegrees = 50.0f;
    constexpr const char* kIdleAlias = "ThirdPerson.Idle";
    constexpr const char* kWalkAlias = "ThirdPerson.Walk";
    constexpr const char* kRunAlias = "ThirdPerson.Run";
    constexpr const char* kAttackAlias = "ThirdPerson.Attack";
    constexpr const char* kDeathAlias = "ThirdPerson.Death";

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

    void GetPlanarMovementBasis(const RTBEngine::ECS::GameObject* referenceObject,
                                RTBEngine::Math::Vector3& outForward,
                                RTBEngine::Math::Vector3& outRight)
    {
        outForward = RTBEngine::Math::Vector3::Forward();
        outRight = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());

        if (!referenceObject) {
            return;
        }

        const RTBEngine::Math::Matrix4 worldMatrix = referenceObject->GetWorldMatrix();
        outForward = RTBEngine::Math::Vector3(worldMatrix[8], 0.0f, worldMatrix[10]);

        if (outForward.LengthSquared() <= kDirectionEpsilon) {
            outForward = RTBEngine::Math::Vector3::Forward();
        } else {
            outForward.Normalize();
        }

        outRight = outForward.Cross(RTBEngine::Math::Vector3::Up());
        if (outRight.LengthSquared() <= kDirectionEpsilon) {
            outRight = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
        } else {
            outRight.Normalize();
        }
    }

}

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_GAMEOBJECT(cameraObject)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_RANGE(team, 0, 8)
    RTB_PROPERTY_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(sprintMultiplier, 1.0f, 4.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 0.0f, 1440.0f)
    RTB_PROPERTY_RANGE(cameraDistance, 0.5f, 20.0f)
    RTB_PROPERTY(cameraFocusOffset)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_COMPONENT(projectileAttack, ProjectileAttackAbility)
    RTB_PROPERTY_COMPONENT(attackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(attackAimTrail, TrailRenderer)
    RTB_PROPERTY_FBX(idleAnimationFbx)
    RTB_PROPERTY_FBX(walkAnimationFbx)
    RTB_PROPERTY_FBX(runAnimationFbx)
    RTB_PROPERTY_FBX(attackAnimationFbx)
    RTB_PROPERTY_FBX(deathAnimationFbx)
RTB_END_REGISTER(ThirdPersonCharacterController)

void ThirdPersonCharacterController::OnStart()
{
    hasReplicatedMotionSample = false;
    ClampSettings();
    ResolveHealth();
    ResolveAnimator();
    ResolveProjectileAttack();
    ResolveAttackAimTrail();
    RegisterAnimationSlots();
    ResolveCameraObject();
    ConfigurePhysicsBody();
    DisableCompetingCameraController();
    RebindHealthSubscription();
    RebindAttackJoystickSubscription();
    UpdateAnimatorLocomotion(false, false);
    if (IsLocallyControlled()) {
        ApplyCameraFollowTransform();
    }
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);

    if (health && health->IsDead()) {
        HandleDeath(health->GetLastDeathEvent());
    }
}

void ThirdPersonCharacterController::OnUpdate(float deltaTime)
{
    if (!owner) {
        return;
    }

    if (!IsLocallyControlled()) {
        HideAttackAimTrail();
        return;
    }

    ClampSettings();
    ResolveHealth();
    ResolveAnimator();
    ResolveProjectileAttack();
    ResolveAttackAimTrail();
    RegisterAnimationSlots();
    ResolveCameraObject();
    DisableCompetingCameraController();
    RebindHealthSubscription();

    if (state == State::Dead) {
        HideAttackAimTrail();
        return;
    }

    if (state == State::Attacking) {
        HideAttackAimTrail();
        UpdatePredictedAttackVisual(deltaTime);
        return;
    }
}

void ThirdPersonCharacterController::OnFixedUpdate(float fixedDeltaTime)
{
    if (!owner) {
        return;
    }

    ResolveAnimator();
    RegisterAnimationSlots();

    // Client local pawn: send input to host and drive animator locally (transform comes from network).
    if (state == State::Dead) {
        HideAttackAimTrail();
        StopPlanarMotion();
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() &&
        IsLocallyControlled()) {
        SendNetworkInput();
        UpdateAnimatorFromLocalInput();
        return;
    }

    // Proxies without host simulation skip movement (animator for clients runs in LateUpdate).
    if (!HasSimulationAuthority()) {
        return;
    }

    ClampSettings();
    ResolveHealth();
    ResolveCameraObject();
    ResolveProjectileAttack();
    ConfigurePhysicsBody();

    if (state == State::Dead) {
        HideAttackAimTrail();
        StopPlanarMotion();
        return;
    }

    if (PauseMenuController::IsAnyMenuOpen()) {
        HideAttackAimTrail();
        StopPlanarMotion();
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    if (state == State::Attacking) {
        HideAttackAimTrail();
        UpdateAttackFacingLock(fixedDeltaTime);
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        TryProcessRemoteAttackInput();
    }

    UpdateMovement(fixedDeltaTime);
}

void ThirdPersonCharacterController::OnLateUpdate(float deltaTime)
{
    if (!owner) {
        return;
    }

    if (!IsLocallyControlled()) {
        HideAttackAimTrail();
        // Runs after NetworkTransform when that component is listed earlier on the pawn.
        if (UsesReplicatedAnimator()) {
            ResolveAnimator();
            RegisterAnimationSlots();
            UpdateAnimatorFromReplicatedMotion(deltaTime);
        }
        return;
    }

    ResolveCameraObject();
    DisableCompetingCameraController();

    if (state == State::Dead && deathCameraFrozen) {
        if (cameraObject) {
            cameraObject->GetTransform().SetPosition(frozenCameraWorldPosition);
            cameraObject->GetTransform().SetRotation(frozenCameraWorldRotation);
        }
        return;
    }

    ApplyCameraFollowTransform();

    if (state == State::Locomotion) {
        UpdateAttackAimTrail();
        return;
    }

    if (state != State::Attacking) {
        return;
    }

    const bool hasAttackAnimation =
        animator && attackSlotState.ready && animator->GetClip(kAttackAlias);

    if (!hasAttackAnimation) {
        if (!projectileAttack || !projectileAttack->IsAbilityActive()) {
            FinishAttack();
        }
        return;
    }

    if ((animator->GetCurrentClipName() == kAttackAlias && animator->IsPlaying()) ||
        (projectileAttack && projectileAttack->IsAbilityActive())) {
        return;
    }

    FinishAttack();
}

void ThirdPersonCharacterController::OnValidate()
{
    ClampSettings();
    ResolveHealth();
    ResolveAnimator();
    ResolveProjectileAttack();
    ResolveAttackAimTrail();
    if (animator) {
        RegisterAnimationSlots();
    }
    ResolveCameraObject();
    ConfigurePhysicsBody();
    DisableCompetingCameraController();
    RebindAttackJoystickSubscription();
    HideAttackAimTrail();
    UpdateAnimatorLocomotion(false, false);
    if (IsLocallyControlled()) {
        ApplyCameraFollowTransform();
    }
}

void ThirdPersonCharacterController::OnDestroy()
{
    HideAttackAimTrail();
    UnsubscribeFromAttackJoystick();
    UnsubscribeFromHealth();
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);
}

void ThirdPersonCharacterController::ClampSettings()
{
    team = std::max(0, team);
    moveSpeed = std::max(0.0f, moveSpeed);
    sprintMultiplier = std::max(1.0f, sprintMultiplier);
    turnSpeed = std::max(0.0f, turnSpeed);
    cameraDistance = std::max(0.1f, cameraDistance);
}

void ThirdPersonCharacterController::ConfigurePhysicsBody() const
{
    if (!owner) {
        return;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (!rbComp || !rbComp->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rbComp->GetRigidBody();
    RTBEngine::Physics::PhysicsUtils::ConfigurePlanarDynamicBody(rigidBody);
}

void ThirdPersonCharacterController::ResolveCameraObject()
{
    if (cameraObject) {
        return;
    }

    if (owner) {
        auto* childCamera = owner->GetComponentInChildren<RTBEngine::ECS::CameraComponent>();
        if (childCamera && childCamera->GetOwner() != owner) {
            cameraObject = childCamera->GetOwner();
            return;
        }
    }

    auto* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    auto* mainCamera = scene->GetMainCamera();
    if (!mainCamera || !mainCamera->GetOwner()) {
        return;
    }

    cameraObject = mainCamera->GetOwner();
}

void ThirdPersonCharacterController::ResolveHealth()
{
    ResolveCharacterHealth();
}

void ThirdPersonCharacterController::ResolveAnimator()
{
    if (animator || !owner) {
        return;
    }

    animator = owner->GetComponentInChildren<RTBEngine::Animation::Animator>();
}

void ThirdPersonCharacterController::ResolveProjectileAttack()
{
    if (projectileAttack || !owner) {
        return;
    }

    projectileAttack = owner->GetComponent<ProjectileAttackAbility>();
}

void ThirdPersonCharacterController::ResolveAttackAimTrail()
{
    if (attackAimTrail || !owner) {
        return;
    }

    attackAimTrail = owner->GetComponentInChildren<RTBEngine::ECS::TrailRenderer>();
}

void ThirdPersonCharacterController::DisableCompetingCameraController() const
{
    if (!cameraObject) {
        return;
    }

    auto* freeLook = cameraObject->GetComponent<RTBEngine::ECS::FreeLookCamera>();
    if (freeLook) {
        freeLook->SetUpdateTickEnabled(false);
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
    if (state != State::Dead) {
        return;
    }

    state = State::Locomotion;
    deathCameraFrozen = false;
    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    HideAttackAimTrail();

    ResolveAnimator();
    RegisterAnimationSlots();
    UpdateAnimatorLocomotion(false, false);
}

void ThirdPersonCharacterController::ApplyCameraFollowTransform()
{
    if (!owner || !cameraObject) {
        return;
    }

    if (state == State::Dead && deathCameraFrozen && IsLocallyControlled()) {
        return;
    }

    const RTBEngine::Math::Quaternion orbitRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(
            kFixedCameraPitchDegrees * kDegToRad,
            kFixedCameraYawDegrees * kDegToRad,
            0.0f);
    const RTBEngine::Math::Quaternion ownerWorldRotation = owner->GetWorldRotation();
    const RTBEngine::Math::Vector3 worldFocusOffset = ownerWorldRotation * cameraFocusOffset;
    const bool cameraIsChildOfOwner = (cameraObject->GetParent() == owner);

    if (cameraIsChildOfOwner) {
        const RTBEngine::Math::Quaternion localOrbitRotation = ownerWorldRotation.Inverse() * orbitRotation;
        const RTBEngine::Math::Vector3 localForward = localOrbitRotation * RTBEngine::Math::Vector3::Forward();
        const RTBEngine::Math::Vector3 localCameraPosition = cameraFocusOffset - localForward * cameraDistance;

        cameraObject->GetTransform().SetPosition(localCameraPosition);
        cameraObject->GetTransform().SetRotation(localOrbitRotation);
        return;
    }

    const RTBEngine::Math::Vector3 focusPoint = owner->GetWorldPosition() + worldFocusOffset;
    const RTBEngine::Math::Vector3 forward = orbitRotation * RTBEngine::Math::Vector3::Forward();
    const RTBEngine::Math::Vector3 cameraPosition = focusPoint - forward * cameraDistance;

    cameraObject->GetTransform().SetPosition(cameraPosition);
    cameraObject->GetTransform().SetRotation(orbitRotation);
}

void ThirdPersonCharacterController::RegisterAnimationSlots()
{
    const bool animatorChanged = (registeredAnimator != animator);
    if (animatorChanged) {
        registeredAnimator = animator;
        idleSlotState = {};
        walkSlotState = {};
        runSlotState = {};
        attackSlotState = {};
        deathSlotState = {};
    }

    if (!animator) {
        if (!missingAnimatorWarningShown &&
            (!idleAnimationFbx.empty() || !walkAnimationFbx.empty() || !runAnimationFbx.empty() ||
             !attackAnimationFbx.empty() || !deathAnimationFbx.empty())) {
            RTB_WARN("[ThirdPersonCharacterController] Assign an Animator component to use FBX animation slots.");
            missingAnimatorWarningShown = true;
        }
        return;
    }

    missingAnimatorWarningShown = false;

    RegisterAnimationSlot("Idle", idleAnimationFbx, kIdleAlias, idleSlotState);
    RegisterAnimationSlot("Walk", walkAnimationFbx, kWalkAlias, walkSlotState);
    RegisterAnimationSlot("Run", runAnimationFbx, kRunAlias, runSlotState);
    RegisterAnimationSlot("Attack", attackAnimationFbx, kAttackAlias, attackSlotState);
    RegisterAnimationSlot("Death", deathAnimationFbx, kDeathAlias, deathSlotState);
}

void ThirdPersonCharacterController::RegisterAnimationSlot(const char* slotLabel,
                                                           const std::string& sourceFbx,
                                                           const char* alias,
                                                           AnimationSlotState& slotState)
{
    if (slotState.sourceFbx == sourceFbx) {
        return;
    }

    slotState.sourceFbx = sourceFbx;
    slotState.ready = false;

    if (!animator || sourceFbx.empty()) {
        return;
    }

    if (!animator->LoadClipFromFbx(alias, sourceFbx)) {
        RTB_WARN(std::string("[ThirdPersonCharacterController] ") + slotLabel +
                 " slot FBX has no usable animation clip: " + sourceFbx);
        return;
    }

    slotState.ready = true;
}

void ThirdPersonCharacterController::RebindHealthSubscription()
{
    RebindCharacterDeathSubscription();
}

void ThirdPersonCharacterController::UnsubscribeFromHealth()
{
    UnsubscribeCharacterDeath();
}

void ThirdPersonCharacterController::RebindAttackJoystickSubscription()
{
    if (subscribedAttackJoystick == attackJoystick && attackJoystickReleaseSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromAttackJoystick();

    if (!attackJoystick) {
        return;
    }

    subscribedAttackJoystick = attackJoystick;
    attackJoystickReleaseSubscription = attackJoystick->SubscribeToReleased(
        [this](const RTBEngine::Math::Vector2& joystickValue) {
            HandleJoystickAttackReleased(joystickValue);
        });
}

void ThirdPersonCharacterController::UnsubscribeFromAttackJoystick()
{
    attackJoystickReleaseSubscription.Reset();
    subscribedAttackJoystick = nullptr;
}

void ThirdPersonCharacterController::HandleJoystickAttackReleased(const RTBEngine::Math::Vector2& joystickValue)
{
    HideAttackAimTrail();

    if (!IsLocallyControlled()) {
        return;
    }

    if (!projectileAttack ||
        projectileAttack->GetDamageAmount() <= 0.0f ||
        projectileAttack->IsCoolingDown() ||
        PauseMenuController::IsAnyMenuOpen()) {
        return;
    }

    const RTBEngine::Math::Vector3 attackDirection = GetAttackDirectionFromJoystick(joystickValue);
    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        ++networkAttackSequence;
        pendingNetworkAttackDirection = attackDirection;
        PlayPredictedAttackVisual(attackDirection);
        return;
    }

    StartAttack(attackDirection);
}

void ThirdPersonCharacterController::UpdateAttackAimTrail()
{
    ResolveAttackAimTrail();

    if (!attackAimTrail) {
        return;
    }

    if (!attackJoystick ||
        !attackJoystick->IsDragging() ||
        !projectileAttack ||
        projectileAttack->GetDamageAmount() <= 0.0f ||
        projectileAttack->IsCoolingDown() ||
        state != State::Locomotion ||
        PauseMenuController::IsAnyMenuOpen()) {
        HideAttackAimTrail();
        return;
    }

    const RTBEngine::Math::Vector3 attackDirection =
        GetAttackDirectionFromJoystick(attackJoystick->GetValue());
    if (!HasMovementInput(attackDirection)) {
        HideAttackAimTrail();
        return;
    }

    const RTBEngine::Math::Vector3 start = GetProjectileLaunchOrigin(attackDirection);
    const RTBEngine::Math::Vector3 end = start + attackDirection * GetProjectileTravelDistance();
    const RTBEngine::Math::Vector3 points[] = {
        start,
        end
    };

    attackAimTrail->width = GetConfiguredProjectileRadius() * 2.0f;
    attackAimTrail->SetPoints(points, 2);
    attackAimTrail->SetVisible(true);
}

void ThirdPersonCharacterController::HideAttackAimTrail()
{
    if (!attackAimTrail) {
        return;
    }

    attackAimTrail->SetVisible(false);
    attackAimTrail->ClearPoints();
}

void ThirdPersonCharacterController::UpdateAttackFacingLock(float deltaTime)
{
    RTBEngine::Math::Vector3 attackDirection = GetActiveAttackDirection();
    if (!HasMovementInput(attackDirection)) {
        StopPlanarMotion();
        return;
    }

    FaceAttackDirection(attackDirection);

    bool isRunning = false;
    RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    const bool hasMovementInput = HasMovementInput(desiredMove);
    const float speed = hasMovementInput
        ? moveSpeed * (isRunning ? sprintMultiplier : 1.0f)
        : 0.0f;

    auto* rbComp = owner ? owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>() : nullptr;
    RTBEngine::Physics::RigidBody* rigidBody =
        (rbComp && rbComp->HasRigidBody()) ? rbComp->GetRigidBody() : nullptr;
    const bool useDynamicRigidBody =
        rigidBody && rigidBody->GetType() == RTBEngine::Physics::RigidBodyType::Dynamic;

    if (useDynamicRigidBody) {
        RTBEngine::Physics::PhysicsUtils::ApplyPlanarDynamicBodyMotion(
            rigidBody,
            desiredMove,
            attackDirection,
            speed,
            turnSpeed,
            deltaTime,
            owner->GetTransform().GetRotation());
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
    auto* rbComp = owner ? owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>() : nullptr;
    RTBEngine::Physics::RigidBody* rigidBody =
        (rbComp && rbComp->HasRigidBody()) ? rbComp->GetRigidBody() : nullptr;
    const bool useDynamicRigidBody =
        rigidBody && rigidBody->GetType() == RTBEngine::Physics::RigidBodyType::Dynamic;

    if (!hasMovementInput) {
        if (useDynamicRigidBody) {
            RTBEngine::Physics::PhysicsUtils::ApplyPlanarDynamicBodyMotion(
                rigidBody,
                RTBEngine::Math::Vector3::Zero(),
                RTBEngine::Math::Vector3::Zero(),
                0.0f,
                turnSpeed,
                deltaTime,
                owner->GetTransform().GetRotation());
        }

        UpdateAnimatorLocomotion(false, false);
        return;
    }

    const float speed = moveSpeed * (isRunning ? sprintMultiplier : 1.0f);

    if (useDynamicRigidBody) {
        RTBEngine::Physics::PhysicsUtils::ApplyPlanarDynamicBodyMotion(
            rigidBody,
            desiredMove,
            desiredMove,
            speed,
            turnSpeed,
            deltaTime,
            owner->GetTransform().GetRotation());
    } else {
        const float targetYaw = -std::atan2(desiredMove.x, desiredMove.z) * kRadToDeg;
        RTBEngine::Math::Vector3 currentEuler = owner->GetTransform().GetRotation().ToEulerAngles();
        const float currentYaw = currentEuler.y * kRadToDeg;
        const float nextYaw = MoveTowardsAngleDegrees(currentYaw, targetYaw, turnSpeed * deltaTime);
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
    return RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        !HasSimulationAuthority() &&
        !IsLocallyControlled();
}

void ThirdPersonCharacterController::UpdateAnimatorFromLocalInput()
{
    if (state == State::Dead) {
        return;
    }

    if (PauseMenuController::IsAnyMenuOpen()) {
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    bool isRunning = false;
    const RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);
    UpdateAnimatorLocomotion(HasMovementInput(desiredMove), isRunning);
}

void ThirdPersonCharacterController::UpdateAnimatorFromReplicatedMotion(float deltaTime)
{
    if (!owner || state != State::Locomotion) {
        return;
    }

    const RTBEngine::Math::Vector3 currentPosition = owner->GetWorldPosition();
    if (!hasReplicatedMotionSample) {
        lastReplicatedWorldPosition = currentPosition;
        hasReplicatedMotionSample = true;
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    RTBEngine::Math::Vector3 delta = currentPosition - lastReplicatedWorldPosition;
    delta.y = 0.0f;
    lastReplicatedWorldPosition = currentPosition;

    const float timestep = std::max(deltaTime, 0.0001f);
    const float planarSpeed = delta.Length() / timestep;
    const float walkThreshold = std::max(0.05f, moveSpeed * 0.12f);
    const float runThreshold = std::max(walkThreshold + 0.05f, moveSpeed * sprintMultiplier * 0.55f);

    const bool hasMovementInput = planarSpeed >= walkThreshold;
    const bool isRunning = planarSpeed >= runThreshold;
    UpdateAnimatorLocomotion(hasMovementInput, isRunning);
}

void ThirdPersonCharacterController::UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning)
{
    if (!owner || !animator || state != State::Locomotion) {
        return;
    }

    const char* targetClipAlias = nullptr;
    if (hasMovementInput) {
        if (isRunning && runSlotState.ready) {
            targetClipAlias = kRunAlias;
        } else if (walkSlotState.ready) {
            targetClipAlias = kWalkAlias;
        }
    } else if (idleSlotState.ready) {
        targetClipAlias = kIdleAlias;
    }

    if (!targetClipAlias || !animator->GetClip(targetClipAlias)) {
        return;
    }

    if (animator->GetCurrentClipName() == targetClipAlias && animator->IsPlaying()) {
        return;
    }

    animator->Play(targetClipAlias, true);
}

void ThirdPersonCharacterController::StartAttack(const RTBEngine::Math::Vector3& attackDirection)
{
    HideAttackAimTrail();

    if (state != State::Locomotion || !projectileAttack) {
        return;
    }

    RTBEngine::Math::Vector3 normalizedAttackDirection = attackDirection;
    normalizedAttackDirection.y = 0.0f;
    if (normalizedAttackDirection.LengthSquared() <= kDirectionEpsilon) {
        return;
    }

    normalizedAttackDirection.Normalize();
    activeAttackDirection = normalizedAttackDirection;
    FaceAttackDirection(activeAttackDirection);

    if (!projectileAttack->TryActivate(owner, activeAttackDirection)) {
        activeAttackDirection = RTBEngine::Math::Vector3::Zero();
        return;
    }

    state = State::Attacking;
    if (animator && attackSlotState.ready && animator->GetClip(kAttackAlias)) {
        animator->Play(kAttackAlias, false);
    }
}

void ThirdPersonCharacterController::FinishAttack()
{
    if (state != State::Attacking) {
        return;
    }

    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    state = State::Locomotion;
    HideAttackAimTrail();
}

void ThirdPersonCharacterController::HandleDeath(const HealthComponent::DeathEvent& /*eventData*/)
{
    if (state == State::Dead) {
        return;
    }

    state = State::Dead;
    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    if (projectileAttack) {
        projectileAttack->CancelAbility();
    }
    HideAttackAimTrail();
    StopPlanarMotion();

    ResolveCameraObject();
    if (IsLocallyControlled() && cameraObject) {
        deathCameraFrozen = true;
        frozenCameraWorldPosition = cameraObject->GetWorldPosition();
        frozenCameraWorldRotation = cameraObject->GetWorldRotation();
    }

    if (animator && deathSlotState.ready && animator->GetClip(kDeathAlias)) {
        animator->Play(kDeathAlias, false);
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() &&
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        if (const RTBEngine::ECS::NetworkIdentity* identity =
                owner ? owner->GetComponent<RTBEngine::ECS::NetworkIdentity>() : nullptr) {
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

void ThirdPersonCharacterController::FaceAttackDirection(const RTBEngine::Math::Vector3& attackDirection)
{
    if (!owner || !HasMovementInput(attackDirection)) {
        return;
    }

    RTBEngine::Math::Vector3 planarDirection = attackDirection;
    planarDirection.y = 0.0f;
    planarDirection.Normalize();

    const float targetYaw = -std::atan2(planarDirection.x, planarDirection.z) * kRadToDeg;
    const RTBEngine::Math::Quaternion targetRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, targetYaw * kDegToRad, 0.0f);

    owner->GetTransform().SetRotation(targetRotation);

    auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    RTBEngine::Physics::RigidBody* rigidBody =
        (rbComp && rbComp->HasRigidBody()) ? rbComp->GetRigidBody() : nullptr;
    if (rigidBody && rigidBody->GetType() == RTBEngine::Physics::RigidBodyType::Dynamic) {
        RTBEngine::Math::Vector3 centerOffset = RTBEngine::Math::Vector3::Zero();
        if (auto* capsule = owner->GetComponent<RTBEngine::ECS::CapsuleColliderComponent>()) {
            centerOffset = capsule->GetCenterOffset();
        } else if (auto* sphere = owner->GetComponent<RTBEngine::ECS::SphereColliderComponent>()) {
            centerOffset = sphere->GetCenterOffset();
        }
        rigidBody->SetWorldTransform(owner->GetWorldPosition() + (targetRotation * centerOffset), targetRotation);
        rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
    }
}

void ThirdPersonCharacterController::StopPlanarMotion() const
{
    if (!owner) {
        return;
    }

    auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (!rbComp || !rbComp->HasRigidBody()) {
        return;
    }

    RTBEngine::Physics::RigidBody* rigidBody = rbComp->GetRigidBody();
    if (!rigidBody) {
        return;
    }

    btVector3 velocity = rigidBody->GetLinearVelocity();
    velocity.setX(0.0f);
    velocity.setZ(0.0f);
    rigidBody->SetLinearVelocity(velocity);
    rigidBody->SetAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetDesiredMoveDirection(bool& outIsRunning) const
{
    outIsRunning = false;

    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    if (const RTBEngine::ECS::NetworkIdentity* identity = owner->GetComponent<RTBEngine::ECS::NetworkIdentity>()) {
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
    GetPlanarMovementBasis(cameraObject ? cameraObject : owner, forward, right);

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

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetProjectileLaunchOrigin(
    const RTBEngine::Math::Vector3& attackDirection) const
{
    if (projectileAttack) {
        return projectileAttack->GetLaunchOrigin(owner, attackDirection);
    }

    return RTBEngine::Math::Vector3::Zero();
}

float ThirdPersonCharacterController::GetProjectileTravelDistance() const
{
    if (projectileAttack) {
        return projectileAttack->GetTravelDistance();
    }

    return 0.0f;
}

float ThirdPersonCharacterController::GetConfiguredProjectileRadius() const
{
    return projectileAttack ? projectileAttack->GetProjectileRadius() : 0.0f;
}

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetAttackDirectionFromJoystick(
    const RTBEngine::Math::Vector2& joystickValue) const
{
    RTBEngine::Math::Vector3 forward = RTBEngine::Math::Vector3::Forward();
    RTBEngine::Math::Vector3 right = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
    GetPlanarMovementBasis(cameraObject ? cameraObject : owner, forward, right);

    RTBEngine::Math::Vector3 attackDirection = right * joystickValue.x + forward * joystickValue.y;
    attackDirection.y = 0.0f;
    if (attackDirection.LengthSquared() <= kDirectionEpsilon) {
        return RTBEngine::Math::Vector3::Zero();
    }

    attackDirection.Normalize();
    return attackDirection;
}

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetActiveAttackDirection() const
{
    RTBEngine::Math::Vector3 attackDirection = activeAttackDirection;
    attackDirection.y = 0.0f;
    if (attackDirection.LengthSquared() <= kDirectionEpsilon && owner) {
        attackDirection = owner->GetWorldRotation() * RTBEngine::Math::Vector3::Forward();
        attackDirection.y = 0.0f;
    }

    if (attackDirection.LengthSquared() <= kDirectionEpsilon) {
        return RTBEngine::Math::Vector3::Forward();
    }

    attackDirection.Normalize();
    return attackDirection;
}

void ThirdPersonCharacterController::SendNetworkInput()
{
    if (!owner || RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        return;
    }

    bool isRunning = false;
    const RTBEngine::Math::Vector3 desiredMove = GetDesiredMoveDirection(isRunning);

    RTBEngine::Online::OnlineGameplayNet::PlayerInputSnapshot snapshot;
    snapshot.senderUserId = RTBEngine::Online::OnlineGameplayNet::GetLocalUserId();
    snapshot.sequenceNumber = ++inputSequenceNumber;
    snapshot.moveX = desiredMove.x;
    snapshot.moveZ = desiredMove.z;
    snapshot.sprint = isRunning;
    RTBEngine::Online::OnlineGameplayNet::SendPlayerInput(snapshot);

    if (networkAttackSequence > 0) {
        GameNet::PlayerCombatInput combatInput;
        combatInput.senderUserId = snapshot.senderUserId;
        combatInput.attackSequence = networkAttackSequence;
        combatInput.attackDirX = pendingNetworkAttackDirection.x;
        combatInput.attackDirZ = pendingNetworkAttackDirection.z;
        GameNet::OnlineGameNetSubsystem::SendCombatInput(combatInput);
    }
}

void ThirdPersonCharacterController::TryProcessRemoteAttackInput()
{
    if (!owner || IsLocallyControlled() || !projectileAttack) {
        return;
    }

    RTBEngine::ECS::NetworkIdentity* identity = owner->GetComponent<RTBEngine::ECS::NetworkIdentity>();
    if (!identity || identity->networkOwnerUserId.empty()) {
        return;
    }

    GameNet::PlayerCombatInput remoteCombat;
    if (!GameNet::OnlineGameNetSubsystem::TryGetLatestCombatInputForUser(
            identity->networkOwnerUserId,
            remoteCombat)) {
        return;
    }

    if (remoteCombat.attackSequence == 0 ||
        remoteCombat.attackSequence == lastProcessedRemoteAttackSequence) {
        return;
    }

    lastProcessedRemoteAttackSequence = remoteCombat.attackSequence;

    RTBEngine::Math::Vector3 attackDirection(remoteCombat.attackDirX, 0.0f, remoteCombat.attackDirZ);
    if (!HasMovementInput(attackDirection)) {
        return;
    }

    StartAttack(attackDirection);
}

void ThirdPersonCharacterController::PlayPredictedAttackVisual(const RTBEngine::Math::Vector3& attackDirection)
{
    HideAttackAimTrail();

    if (state != State::Locomotion || !projectileAttack) {
        return;
    }

    RTBEngine::Math::Vector3 normalizedAttackDirection = attackDirection;
    normalizedAttackDirection.y = 0.0f;
    if (!HasMovementInput(normalizedAttackDirection)) {
        return;
    }

    normalizedAttackDirection.Normalize();
    activeAttackDirection = normalizedAttackDirection;
    FaceAttackDirection(activeAttackDirection);
    state = State::Attacking;

    if (animator && attackSlotState.ready && animator->GetClip(kAttackAlias)) {
        animator->Play(kAttackAlias, false);
    }

    predictedAttackVisualTimeRemaining =
        projectileAttack->GetHitDelaySeconds() + projectileAttack->GetRecoverySeconds();
}

void ThirdPersonCharacterController::UpdatePredictedAttackVisual(float deltaTime)
{
    if (predictedAttackVisualTimeRemaining <= 0.0f || state != State::Attacking) {
        return;
    }

    if (projectileAttack && projectileAttack->IsAbilityActive()) {
        return;
    }

    predictedAttackVisualTimeRemaining = std::max(0.0f, predictedAttackVisualTimeRemaining - deltaTime);
    if (predictedAttackVisualTimeRemaining <= 0.0f) {
        FinishAttack();
    }
}

