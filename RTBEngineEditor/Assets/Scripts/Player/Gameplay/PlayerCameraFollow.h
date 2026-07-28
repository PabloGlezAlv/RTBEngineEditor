#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
    }
}

namespace PlayerCameraFollow {

    void ApplyFollow(
        RTBEngine::Scene::GameObject* owner,
        RTBEngine::Scene::GameObject* cameraObject,
        float cameraDistance,
        const RTBEngine::Math::Vector3& cameraFocusOffset);

    void ApplySpectate(
        RTBEngine::Scene::GameObject* owner,
        RTBEngine::Scene::GameObject* cameraObject,
        RTBEngine::Scene::GameObject* targetPawn,
        float cameraDistance,
        const RTBEngine::Math::Vector3& cameraFocusOffset);

}
