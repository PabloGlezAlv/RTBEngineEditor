#include "PlayerCameraFollow.h"

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/GameObject.h>

namespace PlayerCameraFollow {
    namespace {
        constexpr float kPi = 3.14159265358979323846f;
        constexpr float kDegToRad = kPi / 180.0f;
        constexpr float kFixedCameraYawDegrees = 0.0f;
        constexpr float kFixedCameraPitchDegrees = 50.0f;

        RTBEngine::Math::Quaternion MakeOrbitRotation()
        {
            return RTBEngine::Math::Quaternion::FromEulerAngles(
                kFixedCameraPitchDegrees * kDegToRad,
                kFixedCameraYawDegrees * kDegToRad,
                0.0f);
        }
    }

    void ApplyFollow(
        RTBEngine::Scene::GameObject* owner,
        RTBEngine::Scene::GameObject* cameraObject,
        float cameraDistance,
        const RTBEngine::Math::Vector3& cameraFocusOffset)
    {
        if (!cameraObject) {
            return;
        }

        const RTBEngine::Math::Quaternion orbitRotation = MakeOrbitRotation();
        const RTBEngine::Math::Quaternion ownerWorldRotation = owner->GetWorldRotation();
        const RTBEngine::Math::Vector3 worldFocusOffset = ownerWorldRotation * cameraFocusOffset;
        const bool cameraIsChildOfOwner = (cameraObject->GetParent() == owner);

        if (cameraIsChildOfOwner) {
            const RTBEngine::Math::Quaternion localOrbitRotation =
                ownerWorldRotation.Inverse() * orbitRotation;
            const RTBEngine::Math::Vector3 localForward =
                localOrbitRotation * RTBEngine::Math::Vector3::Forward();
            const RTBEngine::Math::Vector3 localCameraPosition =
                cameraFocusOffset - localForward * cameraDistance;

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

    void ApplySpectate(
        RTBEngine::Scene::GameObject* owner,
        RTBEngine::Scene::GameObject* cameraObject,
        RTBEngine::Scene::GameObject* targetPawn,
        float cameraDistance,
        const RTBEngine::Math::Vector3& cameraFocusOffset)
    {
        if (!cameraObject || !targetPawn) {
            return;
        }

        const RTBEngine::Math::Quaternion orbitRotation = MakeOrbitRotation();
        const RTBEngine::Math::Quaternion targetWorldRotation = targetPawn->GetWorldRotation();
        const RTBEngine::Math::Vector3 worldFocusOffset = targetWorldRotation * cameraFocusOffset;
        const RTBEngine::Math::Vector3 worldFocusPoint =
            targetPawn->GetWorldPosition() + worldFocusOffset;
        const RTBEngine::Math::Vector3 worldForward =
            orbitRotation * RTBEngine::Math::Vector3::Forward();
        const RTBEngine::Math::Vector3 worldCameraPosition =
            worldFocusPoint - worldForward * cameraDistance;
        const bool cameraIsChildOfOwner = (cameraObject->GetParent() == owner);

        if (cameraIsChildOfOwner) {
            const RTBEngine::Math::Vector3 ownerWorldPosition = owner->GetWorldPosition();
            const RTBEngine::Math::Quaternion ownerWorldRotation = owner->GetWorldRotation();
            const RTBEngine::Math::Vector3 localCameraPosition =
                ownerWorldRotation.Inverse() * (worldCameraPosition - ownerWorldPosition);
            const RTBEngine::Math::Quaternion localOrbitRotation =
                ownerWorldRotation.Inverse() * orbitRotation;

            cameraObject->GetTransform().SetPosition(localCameraPosition);
            cameraObject->GetTransform().SetRotation(localOrbitRotation);
            return;
        }

        cameraObject->GetTransform().SetPosition(worldCameraPosition);
        cameraObject->GetTransform().SetRotation(orbitRotation);
    }

}
