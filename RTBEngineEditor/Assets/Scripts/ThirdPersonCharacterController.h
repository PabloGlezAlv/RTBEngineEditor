#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <string>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

class ThirdPersonCharacterController : public RTBEngine::ECS::Component
{
public:
    ThirdPersonCharacterController() = default;
    ~ThirdPersonCharacterController() override = default;

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

    RTBEngine::ECS::GameObject* cameraObject = nullptr;
    bool autoResolveMainCamera = true;
    bool requireRightMouseForLook = true;
    bool enableZoom = true;
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
    bool syncAnimatorLocomotion = true;
    std::string idleClipName;
    std::string walkClipName;
    std::string runClipName;

    RTB_COMPONENT(ThirdPersonCharacterController)

private:
    float cameraYaw = 0.0f;
    float cameraPitch = 18.0f;

    void ClampSettings();
    void ResolveCameraObject();
    void DisableCompetingCameraController() const;
    void SyncCameraFromCurrentTransform();
    void UpdateMovement(float deltaTime);
    void UpdateCameraOrbit();
    void UpdateAnimatorLocomotion(bool hasMovementInput, bool isRunning);
};
