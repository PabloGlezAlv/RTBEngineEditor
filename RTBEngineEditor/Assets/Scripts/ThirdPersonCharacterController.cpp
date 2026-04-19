#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/CameraComponent.h>
#include <RTBEngine/ECS/FreeLookCamera.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <algorithm>
#include <cmath>

using ThisClass = ThirdPersonCharacterController;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;
    constexpr float kDegToRad = kPi / 180.0f;
    constexpr const char* kIdleAlias = "ThirdPerson.Idle";
    constexpr const char* kWalkAlias = "ThirdPerson.Walk";
    constexpr const char* kRunAlias = "ThirdPerson.Run";

    float ClampAngleDegrees(float angle) {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }

    float MoveTowardsAngleDegrees(float current, float target, float maxDelta) {
        const float delta = ClampAngleDegrees(target - current);
        if (std::abs(delta) <= maxDelta) {
            return target;
        }
        return current + (delta > 0.0f ? maxDelta : -maxDelta);
    }

    bool HasMovementInput(const RTBEngine::Math::Vector3& value) {
        return std::abs(value.x) > 0.0001f || std::abs(value.z) > 0.0001f;
    }

    void GetPlanarMovementBasis(
        const RTBEngine::ECS::GameObject* referenceObject,
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

        if (outForward.LengthSquared() <= 0.0001f) {
            outForward = RTBEngine::Math::Vector3::Forward();
        } else {
            outForward.Normalize();
        }

        // Match the engine's negated-yaw convention: "right" is forward x up.
        outRight = outForward.Cross(RTBEngine::Math::Vector3::Up());
        if (outRight.LengthSquared() <= 0.0001f) {
            outRight = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
        } else {
            outRight.Normalize();
        }
    }

    void ReleaseLoadedModelMeshes(RTBEngine::Rendering::ModelData& data)
    {
        for (RTBEngine::Rendering::Mesh* mesh : data.meshes) {
            delete mesh;
        }
        data.meshes.clear();
    }
}

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_GAMEOBJECT(cameraObject)
    RTB_PROPERTY_RANGE(moveSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(sprintMultiplier, 1.0f, 4.0f)
    RTB_PROPERTY_RANGE(turnSpeed, 0.0f, 1440.0f)
    RTB_PROPERTY_RANGE(mouseSensitivity, 0.01f, 1.0f)
    RTB_PROPERTY_RANGE(cameraDistance, 0.5f, 20.0f)
    RTB_PROPERTY_RANGE(minCameraDistance, 0.5f, 20.0f)
    RTB_PROPERTY_RANGE(maxCameraDistance, 0.5f, 20.0f)
    RTB_PROPERTY_RANGE(zoomStep, 0.05f, 2.0f)
    RTB_PROPERTY_RANGE(minPitch, -89.0f, 89.0f)
    RTB_PROPERTY_RANGE(maxPitch, -89.0f, 89.0f)
    RTB_PROPERTY(cameraFocusOffset)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_FBX(idleAnimationFbx)
    RTB_PROPERTY_FBX(walkAnimationFbx)
    RTB_PROPERTY_FBX(runAnimationFbx)
RTB_END_REGISTER(ThirdPersonCharacterController)

void ThirdPersonCharacterController::OnStart()
{
    ClampSettings();
    RegisterAnimationSlots();
    ResolveCameraObject();
    SyncCameraFromCurrentTransform();
    DisableCompetingCameraController();
    UpdateAnimatorLocomotion(false, false);
    UpdateCameraOrbit();
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(true);
}

void ThirdPersonCharacterController::OnUpdate(float deltaTime)
{
    if (!owner) {
        return;
    }

    ClampSettings();
    RegisterAnimationSlots();
    ResolveCameraObject();
    DisableCompetingCameraController();
    UpdateMovement(deltaTime);
    UpdateCameraOrbit();
}

void ThirdPersonCharacterController::OnValidate()
{
    ClampSettings();
    RegisterAnimationSlots();
    ResolveCameraObject();
    SyncCameraFromCurrentTransform();
    DisableCompetingCameraController();
    UpdateAnimatorLocomotion(false, false);
    UpdateCameraOrbit();
}

void ThirdPersonCharacterController::ClampSettings()
{
    moveSpeed = std::max(0.0f, moveSpeed);
    sprintMultiplier = std::max(1.0f, sprintMultiplier);
    turnSpeed = std::max(0.0f, turnSpeed);
    mouseSensitivity = std::max(0.01f, mouseSensitivity);
    minCameraDistance = std::max(0.1f, minCameraDistance);
    maxCameraDistance = std::max(minCameraDistance, maxCameraDistance);
    cameraDistance = std::clamp(cameraDistance, minCameraDistance, maxCameraDistance);
    zoomStep = std::max(0.01f, zoomStep);
    minPitch = std::clamp(minPitch, -89.0f, 89.0f);
    maxPitch = std::clamp(maxPitch, -89.0f, 89.0f);
    if (minPitch > maxPitch) {
        std::swap(minPitch, maxPitch);
    }
    cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);
}

void ThirdPersonCharacterController::RegisterAnimationSlots()
{
    const bool animatorChanged = (registeredAnimator != animator);
    if (animatorChanged) {
        registeredAnimator = animator;
        idleSlotState = {};
        walkSlotState = {};
        runSlotState = {};
    }

    if (!animator) {
        if (!missingAnimatorWarningShown &&
            (!idleAnimationFbx.empty() || !walkAnimationFbx.empty() || !runAnimationFbx.empty())) {
            RTB_WARN("[ThirdPersonCharacterController] Assign an Animator component to use FBX animation slots.");
            missingAnimatorWarningShown = true;
        }
        return;
    }

    missingAnimatorWarningShown = false;

    RegisterAnimationSlot("Idle", idleAnimationFbx, kIdleAlias, idleSlotState);
    RegisterAnimationSlot("Walk", walkAnimationFbx, kWalkAlias, walkSlotState);
    RegisterAnimationSlot("Run", runAnimationFbx, kRunAlias, runSlotState);
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

    RTBEngine::Rendering::ModelData modelData =
        RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(sourceFbx);

    if (modelData.animations.empty() || !modelData.animations.front()) {
        RTB_WARN(std::string("[ThirdPersonCharacterController] ") + slotLabel +
            " slot FBX has no usable animation clip: " + sourceFbx);
        ReleaseLoadedModelMeshes(modelData);
        return;
    }

    animator->AddClip(alias, modelData.animations.front());
    slotState.ready = true;
    ReleaseLoadedModelMeshes(modelData);
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

void ThirdPersonCharacterController::SyncCameraFromCurrentTransform()
{
    if (!cameraObject) {
        return;
    }

    RTBEngine::Math::Vector3 euler = cameraObject->GetWorldRotation().ToEulerAngles();
    cameraPitch = std::clamp(euler.x * kRadToDeg, minPitch, maxPitch);
    cameraYaw = euler.y * kRadToDeg;
}

void ThirdPersonCharacterController::UpdateMovement(float deltaTime)
{
    RTBEngine::Input::InputManager& input = RTBEngine::Input::InputManager::GetInstance();
    RTBEngine::Math::Vector3 forward = RTBEngine::Math::Vector3::Forward();
    RTBEngine::Math::Vector3 right = RTBEngine::Math::Vector3::Forward().Cross(RTBEngine::Math::Vector3::Up());
    GetPlanarMovementBasis(cameraObject ? cameraObject : owner, forward, right);

    RTBEngine::Math::Vector3 desiredMove = RTBEngine::Math::Vector3::Zero();
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::W)) desiredMove += forward;
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::S)) desiredMove -= forward;
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::D)) desiredMove += right;
    if (input.IsKeyPressed(RTBEngine::Input::KeyCode::A)) desiredMove -= right;

    const bool hasMovementInput = HasMovementInput(desiredMove);
    const bool isRunning = hasMovementInput &&
        input.IsKeyPressed(RTBEngine::Input::KeyCode::LeftShift);

    if (!hasMovementInput) {
        UpdateAnimatorLocomotion(false, false);
        return;
    }

    desiredMove.Normalize();

    const float speed = moveSpeed *
        (isRunning ? sprintMultiplier : 1.0f);

    owner->GetTransform().SetPosition(
        owner->GetTransform().GetPosition() + desiredMove * speed * deltaTime);

    const float targetYaw = -std::atan2(desiredMove.x, desiredMove.z) * kRadToDeg;
    RTBEngine::Math::Vector3 currentEuler = owner->GetTransform().GetRotation().ToEulerAngles();
    const float currentYaw = currentEuler.y * kRadToDeg;
    const float nextYaw = MoveTowardsAngleDegrees(currentYaw, targetYaw, turnSpeed * deltaTime);

    owner->GetTransform().SetRotation(
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, nextYaw * kDegToRad, 0.0f));

    UpdateAnimatorLocomotion(true, isRunning);
}

void ThirdPersonCharacterController::UpdateCameraOrbit()
{
    if (!owner || !cameraObject) {
        return;
    }

    RTBEngine::Input::InputManager& input = RTBEngine::Input::InputManager::GetInstance();
    const bool canLook = true;

    if (canLook) {
        cameraYaw += static_cast<float>(input.GetMouseDeltaX()) * mouseSensitivity;
        cameraPitch -= static_cast<float>(input.GetMouseDeltaY()) * mouseSensitivity;
        cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);
    }

    const int scrollDelta = input.GetScrollDelta();
    if (scrollDelta != 0) {
        cameraDistance = std::clamp(
            cameraDistance - static_cast<float>(scrollDelta) * zoomStep,
            minCameraDistance,
            maxCameraDistance);
    }

    const RTBEngine::Math::Quaternion orbitRotation =
        RTBEngine::Math::Quaternion::FromEulerAngles(
            cameraPitch * kDegToRad,
            cameraYaw * kDegToRad,
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

void ThirdPersonCharacterController::UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning)
{
    if (!owner || !animator) {
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

    if (!targetClipAlias) {
        return;
    }

    if (animator->GetCurrentClipName() == targetClipAlias && animator->IsPlaying()) {
        return;
    }

    if (!animator->GetClip(targetClipAlias)) {
        return;
    }

    animator->Play(targetClipAlias, true);
}
