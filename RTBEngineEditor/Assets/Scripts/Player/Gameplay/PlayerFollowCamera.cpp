#include "PlayerFollowCamera.h"

#include "PlayerCameraFollow.h"

#include <RTBEngine/Scene/FreeLookCamera.h>
#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>

using ThisClass = PlayerFollowCamera;

RTB_REGISTER_COMPONENT(PlayerFollowCamera)
    RTB_PROPERTY_SERIALIZED_RANGE(cameraDistance, 0.5f, 20.0f)
    RTB_PROPERTY_SERIALIZED(cameraFocusOffset)
RTB_END_REGISTER(PlayerFollowCamera)

void PlayerFollowCamera::OnStart()
{
    ClampSettings();
    DisableCompetingCameraController();
}

void PlayerFollowCamera::OnValidate()
{
    ClampSettings();
}

void PlayerFollowCamera::ClampSettings()
{
    cameraDistance = std::max(0.1f, cameraDistance);
}

void PlayerFollowCamera::ApplyFollow(RTBEngine::Scene::GameObject* pawnOwner)
{
    if (!owner || !pawnOwner || deathCameraFrozen) {
        return;
    }

    PlayerCameraFollow::ApplyFollow(
        pawnOwner,
        owner,
        cameraDistance,
        cameraFocusOffset);
}

void PlayerFollowCamera::ApplySpectate(
    RTBEngine::Scene::GameObject* pawnOwner,
    RTBEngine::Scene::GameObject* targetPawn)
{
    if (!owner || !pawnOwner) {
        return;
    }

    PlayerCameraFollow::ApplySpectate(
        pawnOwner,
        owner,
        targetPawn,
        cameraDistance,
        cameraFocusOffset);
}

void PlayerFollowCamera::FreezeAtCurrent()
{
    if (!owner) {
        return;
    }

    deathCameraFrozen = true;
    frozenCameraWorldPosition = owner->GetWorldPosition();
    frozenCameraWorldRotation = owner->GetWorldRotation();
}

void PlayerFollowCamera::ApplyFrozenTransform()
{
    if (!owner || !deathCameraFrozen) {
        return;
    }

    owner->GetTransform().SetPosition(frozenCameraWorldPosition);
    owner->GetTransform().SetRotation(frozenCameraWorldRotation);
}

void PlayerFollowCamera::ClearFreeze()
{
    deathCameraFrozen = false;
}

bool PlayerFollowCamera::ShouldHoldFollowWhileDead() const
{
    return deathCameraFrozen;
}

void PlayerFollowCamera::DisableCompetingCameraController()
{
    if (competingCameraDisabled) {
        return;
    }

    if (!freeLookCamera && owner) {
        freeLookCamera = owner->GetComponent<RTBEngine::Scene::FreeLookCamera>();
    }

    if (freeLookCamera) {
        freeLookCamera->SetUpdateTickEnabled(false);
    }

    competingCameraDisabled = true;
}
