#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/ECS/CameraComponent.h>
#include <RTBEngine/ECS/FreeLookCamera.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Input/KeyCode.h>
#include <RTBEngine/Input/MouseButton.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <algorithm>
#include <cmath>

using ThisClass = ThirdPersonCharacterController;

namespace {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;
    constexpr float kDegToRad = kPi / 180.0f;

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
}

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_GAMEOBJECT(cameraObject)
    RTB_PROPERTY(autoResolveMainCamera)
    RTB_PROPERTY(requireRightMouseForLook)
    RTB_PROPERTY(enableZoom)
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
    RTB_PROPERTY(syncAnimatorLocomotion)
    RTB_PROPERTY(idleClipName)
    RTB_PROPERTY(walkClipName)
    RTB_PROPERTY(runClipName)
RTB_END_REGISTER(ThirdPersonCharacterController)

void ThirdPersonCharacterController::OnStart()
{
    ClampSettings();
    ResolveCameraObject();
    SyncCameraFromCurrentTransform();
    DisableCompetingCameraController();
    UpdateCameraOrbit();
}

void ThirdPersonCharacterController::OnUpdate(float deltaTime)
{
    if (!owner) {
        return;
    }

    ClampSettings();
    ResolveCameraObject();
    DisableCompetingCameraController();
    UpdateMovement(deltaTime);
    UpdateCameraOrbit();
}

void ThirdPersonCharacterController::OnValidate()
{
    ClampSettings();
    ResolveCameraObject();
    SyncCameraFromCurrentTransform();
    DisableCompetingCameraController();
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

void ThirdPersonCharacterController::ResolveCameraObject()
{
    if (cameraObject || !autoResolveMainCamera) {
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
    const bool canLook = !requireRightMouseForLook ||
        input.IsMouseButtonPressed(RTBEngine::Input::MouseButton::Right);

    if (canLook) {
        cameraYaw += static_cast<float>(input.GetMouseDeltaX()) * mouseSensitivity;
        cameraPitch -= static_cast<float>(input.GetMouseDeltaY()) * mouseSensitivity;
        cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);
    }

    if (enableZoom) {
        const int scrollDelta = input.GetScrollDelta();
        if (scrollDelta != 0) {
            cameraDistance = std::clamp(
                cameraDistance - static_cast<float>(scrollDelta) * zoomStep,
                minCameraDistance,
                maxCameraDistance);
        }
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
    if (!syncAnimatorLocomotion || !owner) {
        return;
    }

    auto* animator = owner->GetComponent<RTBEngine::Animation::Animator>();
    if (!animator) {
        return;
    }

    const std::string* targetClipName = &idleClipName;
    if (hasMovementInput) {
        targetClipName = isRunning ? &runClipName : &walkClipName;
    }

    if (targetClipName->empty()) {
        return;
    }

    if (animator->GetCurrentClipName() == *targetClipName && animator->IsPlaying()) {
        return;
    }

    if (!animator->GetClip(*targetClipName)) {
        return;
    }

    animator->Play(*targetClipName, true);
}
