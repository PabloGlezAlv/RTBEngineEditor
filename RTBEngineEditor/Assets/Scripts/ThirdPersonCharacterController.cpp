#include "ThirdPersonCharacterController.h"

#include "HealthComponent.h"
#include "PauseMenuController.h"

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
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Online/OnlineSystem.h>
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
    constexpr float kAttackCompletionEpsilon = 0.0001f;
    constexpr float kFallbackAttackDuration = 0.5f;
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

    bool HasLocalGameplayAuthority()
    {
        RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
        RTBEngine::Online::IOnlineLobby* lobby = online.GetLobby();
        if (!online.IsInitialized() || !lobby || lobby->GetCurrentLobby().lobbyId.empty()) {
            return true;
        }

        return lobby->GetCurrentLobby().isOwner;
    }
}

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_GAMEOBJECT(cameraObject)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY(attackOriginOffset)
    RTB_PROPERTY_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(sprintMultiplier, 1.0f, 4.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 0.0f, 1440.0f)
    RTB_PROPERTY_RANGE(cameraDistance, 0.5f, 20.0f)
    RTB_PROPERTY(cameraFocusOffset)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_RANGE(attackRange, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(attackCooldown, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(attackDamage, 0.0f, 100.0f)
    RTB_PROPERTY_RANGE(attackHitDelay, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(attackSphereRadius, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(attackSphereDistance, 0.05f, 5.0f)
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
    ClampSettings();
    ResolveHealth();
    ResolveAnimator();
    ResolveAttackAimTrail();
    RegisterAnimationSlots();
    ResolveCameraObject();
    ConfigurePhysicsBody();
    DisableCompetingCameraController();
    RebindHealthSubscription();
    RebindAttackJoystickSubscription();
    UpdateAnimatorLocomotion(false, false);
    ApplyCameraFollowTransform();
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

    if (!HasLocalGameplayAuthority()) {
        HideAttackAimTrail();
        return;
    }

    ClampSettings();
    ResolveHealth();
    ResolveAnimator();
    ResolveAttackAimTrail();
    RegisterAnimationSlots();
    ResolveCameraObject();
    DisableCompetingCameraController();
    RebindHealthSubscription();

    cooldownRemaining = std::max(0.0f, cooldownRemaining - deltaTime);

    if (state == State::Dead) {
        HideAttackAimTrail();
        return;
    }

    if (state == State::Attacking) {
        HideAttackAimTrail();
        UpdateAttack(deltaTime);
        return;
    }

    UpdateAttackAimTrail();
}

void ThirdPersonCharacterController::OnFixedUpdate(float fixedDeltaTime)
{
    if (!owner) {
        return;
    }

    if (!HasLocalGameplayAuthority()) {
        HideAttackAimTrail();
        StopPlanarMotion();
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    ClampSettings();
    ResolveHealth();
    ResolveCameraObject();
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

    UpdateMovement(fixedDeltaTime);
}

void ThirdPersonCharacterController::OnLateUpdate(float /*deltaTime*/)
{
    if (!owner) {
        return;
    }

    ResolveCameraObject();
    DisableCompetingCameraController();
    ApplyCameraFollowTransform();

    if (state != State::Attacking) {
        return;
    }

    const bool hasAttackAnimation =
        animator && attackSlotState.ready && animator->GetClip(kAttackAlias);

    if (!hasAttackAnimation) {
        if (attackHitExecuted &&
            attackElapsed + kAttackCompletionEpsilon >= attackHitDelay + kFallbackAttackDuration) {
            FinishAttack();
        }
        return;
    }

    if (animator->GetCurrentClipName() == kAttackAlias && animator->IsPlaying()) {
        return;
    }

    FinishAttack();
}

void ThirdPersonCharacterController::OnValidate()
{
    ClampSettings();
    ResolveHealth();
    ResolveAnimator();
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
    ApplyCameraFollowTransform();
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
    moveSpeed = std::max(0.0f, moveSpeed);
    sprintMultiplier = std::max(1.0f, sprintMultiplier);
    turnSpeed = std::max(0.0f, turnSpeed);
    cameraDistance = std::max(0.1f, cameraDistance);

    attackRange = std::max(0.1f, attackRange);
    attackCooldown = std::max(0.0f, attackCooldown);
    attackDamage = std::max(0.0f, attackDamage);
    attackHitDelay = std::max(0.0f, attackHitDelay);
    attackSphereRadius = std::max(0.05f, attackSphereRadius);
    attackSphereDistance = std::max(0.05f, attackSphereDistance);
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
    if (health || !owner) {
        return;
    }

    health = owner->GetComponent<HealthComponent>();
}

void ThirdPersonCharacterController::ResolveAnimator()
{
    if (animator || !owner) {
        return;
    }

    animator = owner->GetComponentInChildren<RTBEngine::Animation::Animator>();
}

void ThirdPersonCharacterController::ResolveAttackAimTrail()
{
    if (attackAimTrail || !owner) {
        return;
    }

    attackAimTrail = owner->GetComponent<RTBEngine::ECS::TrailRenderer>();
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

void ThirdPersonCharacterController::ApplyCameraFollowTransform()
{
    if (!owner || !cameraObject) {
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
    if (subscribedHealth == health && deathSubscription.IsValid()) {
        return;
    }

    UnsubscribeFromHealth();

    if (!health) {
        return;
    }

    subscribedHealth = health;
    deathSubscription = health->SubscribeToDeath(
        [this](const HealthComponent::DeathEvent& eventData) {
            HandleDeath(eventData);
        });
}

void ThirdPersonCharacterController::UnsubscribeFromHealth()
{
    deathSubscription.Reset();
    subscribedHealth = nullptr;
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

    if (!HasLocalGameplayAuthority()) {
        return;
    }

    if (attackDamage <= 0.0f ||
        cooldownRemaining > 0.0f ||
        PauseMenuController::IsAnyMenuOpen()) {
        return;
    }

    StartAttack(GetAttackDirectionFromJoystick(joystickValue));
}

void ThirdPersonCharacterController::UpdateAttackAimTrail()
{
    ResolveAttackAimTrail();

    if (!attackAimTrail) {
        return;
    }

    if (!attackJoystick ||
        !attackJoystick->IsDragging() ||
        attackDamage <= 0.0f ||
        cooldownRemaining > 0.0f ||
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

    const RTBEngine::Math::Vector3 start = GetAttackCastOrigin();
    const RTBEngine::Math::Vector3 end =
        start + attackDirection * std::min(attackSphereDistance, attackRange);
    const RTBEngine::Math::Vector3 points[] = { start, end };

    attackAimTrail->useWorldSpace = true;
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

void ThirdPersonCharacterController::UpdateAttack(float deltaTime)
{
    attackElapsed = std::max(0.0f, attackElapsed + deltaTime);
    if (attackHitExecuted || attackElapsed + kAttackCompletionEpsilon < attackHitDelay) {
        return;
    }

    attackHitExecuted = true;

    HealthComponent* targetHealth = nullptr;
    RTBEngine::Math::Vector3 hitPoint;
    RTBEngine::Math::Vector3 hitDirection;
    if (!PerformAttackSphereCast(&targetHealth, &hitPoint, &hitDirection) || !targetHealth) {
        return;
    }

    HealthComponent::DamageContext damageContext;
    damageContext.amount = attackDamage;
    damageContext.instigator = owner;
    damageContext.hitPoint = hitPoint;
    damageContext.hitDirection = hitDirection;
    targetHealth->TakeDamage(attackDamage, damageContext);
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

    if (state != State::Locomotion) {
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

    state = State::Attacking;
    cooldownRemaining = attackCooldown;
    attackElapsed = 0.0f;
    attackHitExecuted = false;

    if (animator && attackSlotState.ready && animator->GetClip(kAttackAlias)) {
        animator->Play(kAttackAlias, false);
    }
}

void ThirdPersonCharacterController::FinishAttack()
{
    if (state != State::Attacking) {
        return;
    }

    attackElapsed = 0.0f;
    attackHitExecuted = false;
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
    cooldownRemaining = 0.0f;
    attackElapsed = 0.0f;
    attackHitExecuted = false;
    activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    HideAttackAimTrail();
    StopPlanarMotion();

    if (animator && deathSlotState.ready && animator->GetClip(kDeathAlias)) {
        animator->Play(kDeathAlias, false);
    }
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

RTBEngine::Physics::PhysicsWorld* ThirdPersonCharacterController::ResolvePhysicsWorld() const
{
    if (!owner) {
        return nullptr;
    }

    if (auto* rbComp = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>()) {
        if (rbComp->HasRigidBody() && rbComp->GetRigidBody()) {
            return rbComp->GetRigidBody()->GetPhysicsWorld();
        }
    }

    return nullptr;
}

RTBEngine::Math::Vector3 ThirdPersonCharacterController::GetAttackCastOrigin() const
{
    if (!owner) {
        return RTBEngine::Math::Vector3::Zero();
    }

    return owner->GetWorldPosition() + (owner->GetWorldRotation() * attackOriginOffset);
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

HealthComponent* ThirdPersonCharacterController::ResolveHitHealth(RTBEngine::ECS::GameObject* hitObject) const
{
    for (RTBEngine::ECS::GameObject* current = hitObject; current; current = current->GetParent()) {
        if (current == owner) {
            return nullptr;
        }

        if (HealthComponent* targetHealth = current->GetComponent<HealthComponent>()) {
            if (targetHealth != health && !targetHealth->IsDead()) {
                return targetHealth;
            }
            return nullptr;
        }
    }

    return nullptr;
}

bool ThirdPersonCharacterController::PerformAttackSphereCast(HealthComponent** outHealth,
                                                             RTBEngine::Math::Vector3* outHitPoint,
                                                             RTBEngine::Math::Vector3* outHitDirection)
{
    if (outHealth) {
        *outHealth = nullptr;
    }

    if (!owner) {
        return false;
    }

    RTBEngine::Physics::PhysicsWorld* physicsWorld = ResolvePhysicsWorld();
    if (!physicsWorld) {
        return false;
    }

    RTBEngine::Math::Vector3 castStart = GetAttackCastOrigin();
    RTBEngine::Math::Vector3 castDirection = GetActiveAttackDirection();

    const RTBEngine::Math::Vector3 castEnd = castStart + castDirection * std::min(attackSphereDistance, attackRange);
    RTBEngine::Physics::PhysicsQueryHit hit;
    RTBEngine::Physics::PhysicsQueryOptions queryOptions;
    queryOptions.ignoredObject = owner;
    queryOptions.ignoreIgnoredObjectHierarchy = true;
    queryOptions.ignoreTriggers = true;

    if (!physicsWorld->SphereCastClosest(castStart, castEnd, attackSphereRadius, hit, queryOptions)) {
        return false;
    }

    HealthComponent* targetHealth = ResolveHitHealth(hit.gameObject);
    if (!targetHealth) {
        return false;
    }

    if (outHealth) {
        *outHealth = targetHealth;
    }
    if (outHitPoint) {
        *outHitPoint = hit.point;
    }
    if (outHitDirection) {
        *outHitDirection = castDirection;
    }

    return true;
}
