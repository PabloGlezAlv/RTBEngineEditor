#include "PlayerPawnSpawner.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "CharacterGameplaySpawner.h"
#include "OnlinePlayerManager.h"
#include "PlayerCharacterSelection.h"
#include "PlayerRegistry.h"
#include "RoundManager.h"

#include "ThirdPersonCharacterController.h"
#include "PlayerBasicAttackDriver.h"
#include "PlayerSpecialAttackCharge.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Scene/CameraComponent.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIImage.h>
#include <RTBEngine/UI/Elements/UIJoystick.h>

#include <string>

namespace {

void WireSpawnedPlayerCamera(RTBEngine::Scene::GameObject* spawnedPawn)
{
    if (!spawnedPawn) {
        return;
    }

    auto* controller = spawnedPawn->GetComponent<ThirdPersonCharacterController>();
    if (!controller) {
        return;
    }

    RTBEngine::Scene::GameObject* cameraObject = controller->GetCameraObject();
    if (!cameraObject) {
        RTB_WARN("[PlayerPawnSpawner] cameraObject is not assigned on spawned player pawn.");
        return;
    }

    if (cameraObject->GetParent() != spawnedPawn) {
        cameraObject->SetParent(spawnedPawn);
    }

    RTBEngine::Scene::Scene* scene =
        RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    if (auto* cameraComponent =
            cameraObject->GetComponent<RTBEngine::Scene::CameraComponent>()) {
        scene->SetMainCamera(cameraComponent);
        cameraComponent->SyncNow();
    }
}

void BindSpawnedPawnHud(
    RTBEngine::Scene::GameObject* spawnedPawn,
    RTBEngine::UI::UIJoystick* attackJoystick,
    RTBEngine::UI::UIJoystick* specialAttackJoystick,
    RTBEngine::UI::UIImage* specialAttackReadyIcon)
{
    if (auto* basicAttack = spawnedPawn->GetComponent<PlayerBasicAttackDriver>()) {
        basicAttack->SetAttackJoystick(attackJoystick);
        if (!basicAttack->GetAttackJoystick()) {
            RTB_WARN("[PlayerPawnSpawner] attackJoystick is not assigned on PlayerPawnSpawner.");
        }
    } else {
        RTB_WARN("[PlayerPawnSpawner] PlayerBasicAttackDriver is missing on the spawned pawn.");
    }

    if (auto* specialCharge = spawnedPawn->GetComponent<PlayerSpecialAttackCharge>()) {
        specialCharge->SetSpecialAttackJoystick(specialAttackJoystick);
        specialCharge->SetReadyIcon(specialAttackReadyIcon);
        if (!specialCharge->GetSpecialAttackJoystick()) {
            RTB_WARN("[PlayerPawnSpawner] specialAttackJoystick is not assigned on PlayerPawnSpawner.");
        }
        if (!specialCharge->GetReadyIcon()) {
            RTB_WARN("[PlayerPawnSpawner] specialAttackReadyIcon is not assigned on PlayerPawnSpawner.");
        }
    }
}

void FinishRuntimeSpawnedPawn(
    RTBEngine::Scene::GameObject* spawnedPawn,
    RTBEngine::UI::UIJoystick* attackJoystick,
    RTBEngine::UI::UIJoystick* specialAttackJoystick,
    RTBEngine::UI::UIImage* specialAttackReadyIcon)
{
    if (!spawnedPawn) {
        return;
    }

    auto* controller = spawnedPawn->GetComponent<ThirdPersonCharacterController>();
    if (!controller) {
        return;
    }

    WireSpawnedPlayerCamera(spawnedPawn);
    BindSpawnedPawnHud(
        spawnedPawn,
        attackJoystick,
        specialAttackJoystick,
        specialAttackReadyIcon);

    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (scene) {
        if (auto* animator = controller->GetAnimator();
            animator && !animator->AreBoneGOsCreated()) {
            animator->CreateBoneGameObjects(scene);
        }
    }

    if (auto* specialCharge = spawnedPawn->GetComponent<PlayerSpecialAttackCharge>()) {
        specialCharge->RefreshAfterSpawn();
    }

    controller->RefreshAfterSpawn();
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);
}

} // namespace

using ThisClass = PlayerPawnSpawner;

RTB_REGISTER_COMPONENT(PlayerPawnSpawner)
    RTB_PROPERTY_COMPONENT(onlinePlayerManager, OnlinePlayerManager)
    RTB_PROPERTY_COMPONENT(roundManager, RoundManager)
    RTB_PROPERTY_COMPONENT(attackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(specialAttackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(specialAttackReadyIcon, UIImage)
RTB_END_REGISTER(PlayerPawnSpawner)

void PlayerPawnSpawner::OnAwake()
{
    // Clear runs before OnStart subscriptions, so it only drops pawns from the previous scene.
    PlayerRegistry::GetInstance().Clear();
}

void PlayerPawnSpawner::OnStart()
{
    if (!owner) {
        return;
    }

    PlayerCharacterSelection& selection = PlayerCharacterSelection::GetInstance();
    selection.EnsureSelectionFromCatalog();

    CharacterDefinition* definition = selection.GetSelectedDefinition();
    if (!definition) {
        RTB_WARN("[PlayerPawnSpawner] No character definition available for spawn.");
        return;
    }

    if (definition->gameplayPrefabRef.empty()) {
        RTB_WARN("[PlayerPawnSpawner] Character '" + definition->characterId +
                 "' has no gameplayPrefabRef assigned.");
        return;
    }

    spawnedPawn = CharacterGameplaySpawner::InstantiateFromDefinition(
        *definition,
        owner->GetWorldPosition(),
        owner->GetWorldRotation());
    if (!spawnedPawn) {
        RTB_WARN("[PlayerPawnSpawner] Failed to instantiate gameplay prefab for '" +
                 definition->characterId + "'.");
        return;
    }

    spawnedPawn->SetName("Player");
    FinishRuntimeSpawnedPawn(
        spawnedPawn,
        attackJoystick,
        specialAttackJoystick,
        specialAttackReadyIcon);

    if (roundManager) {
        roundManager->SetLocalPlayerPawn(spawnedPawn);
    }

    if (onlinePlayerManager) {
        onlinePlayerManager->BindLocalPawn(spawnedPawn);
    }

    PlayerRegistry::GetInstance().RegisterPlayerPawn(spawnedPawn);
}
