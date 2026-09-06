#pragma once

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace Scene {
        class FreeLookCamera;
        class GameObject;
    }
}

class PlayerFollowCamera : public RTBEngine::Scene::Component
{
public:
    PlayerFollowCamera() = default;
    ~PlayerFollowCamera() override = default;

    void OnStart() override;
    void OnValidate() override;

    void ApplyFollow(RTBEngine::Scene::GameObject* pawnOwner);
    void ApplySpectate(
        RTBEngine::Scene::GameObject* pawnOwner,
        RTBEngine::Scene::GameObject* targetPawn);
    void FreezeAtCurrent();
    void ApplyFrozenTransform();
    void ClearFreeze();
    bool IsFrozen() const { return deathCameraFrozen; }
    bool ShouldHoldFollowWhileDead() const;
    void DisableCompetingCameraController();

    RTB_COMPONENT(PlayerFollowCamera)

    RTB_SERIALIZE()
    float cameraDistance = 11.0f;
    RTB_SERIALIZE()
    RTBEngine::Math::Vector3 cameraFocusOffset = RTBEngine::Math::Vector3(0.0f, 1.2f, 0.0f);

private:
    void ClampSettings();

    RTBEngine::Scene::FreeLookCamera* freeLookCamera = nullptr;
    bool competingCameraDisabled = false;
    bool deathCameraFrozen = false;
    RTBEngine::Math::Vector3 frozenCameraWorldPosition = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Quaternion frozenCameraWorldRotation = RTBEngine::Math::Quaternion::Identity();
};
