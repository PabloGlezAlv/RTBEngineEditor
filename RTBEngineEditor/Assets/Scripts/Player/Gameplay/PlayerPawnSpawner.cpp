#include "PlayerPawnSpawner.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "CharacterGameplaySpawner.h"
#include "CharacterStatsApplier.h"
#include "OnlinePlayerManager.h"
#include "PlayerCharacterSelection.h"
#include "PlayerRegistry.h"
#include "RoundManager.h"

#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Input/InputManager.h>
#include <RTBEngine/Scene/CameraComponent.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIJoystick.h>

#include <string>

namespace {

RTBEngine::ECS::GameObject* FindChildByNameRecursive(
    RTBEngine::ECS::GameObject* root,
    const std::string& objectName)
{
    if (!root) {
        return nullptr;
    }

    if (root->GetName() == objectName) {
        return root;
    }

    for (RTBEngine::ECS::GameObject* child : root->GetChildren()) {
        if (RTBEngine::ECS::GameObject* match = FindChildByNameRecursive(child, objectName)) {
            return match;
        }
    }

    return nullptr;
}

RTBEngine::UI::UIJoystick* FindSceneAttackJoystick(RTBEngine::ECS::Scene* scene)
{
    if (!scene) {
        return nullptr;
    }

    RTBEngine::ECS::GameObject* joystickObject = scene->FindGameObject("AttackJoystick");
    if (!joystickObject) {
        return nullptr;
    }

    return joystickObject->GetComponent<RTBEngine::UI::UIJoystick>();
}

void WireSpawnedPlayerCamera(RTBEngine::ECS::GameObject* spawnedPawn)
{
    if (!spawnedPawn) {
        return;
    }

    auto* controller = spawnedPawn->GetComponent<ThirdPersonCharacterController>();
    if (!controller) {
        return;
    }

    if (!controller->cameraObject) {
        controller->cameraObject = FindChildByNameRecursive(spawnedPawn, "MainCamera");
    }

    if (!controller->cameraObject) {
        RTB_WARN("[PlayerPawnSpawner] MainCamera is not wired on spawned player pawn.");
        return;
    }

    if (controller->cameraObject->GetParent() != spawnedPawn) {
        controller->cameraObject->SetParent(spawnedPawn);
    }

    RTBEngine::ECS::Scene* scene =
        RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    if (auto* cameraComponent =
            controller->cameraObject->GetComponent<RTBEngine::ECS::CameraComponent>()) {
        scene->SetMainCamera(cameraComponent);
        cameraComponent->SyncNow();
    }
}

void WireSpawnedPlayerReferences(
    RTBEngine::ECS::GameObject* spawnedPawn,
    OnlinePlayerManager* onlinePlayerManager)
{
    if (!spawnedPawn) {
        return;
    }

    auto* controller = spawnedPawn->GetComponent<ThirdPersonCharacterController>();
    if (!controller) {
        return;
    }

    if (!controller->animator) {
        if (RTBEngine::ECS::GameObject* playerVisual = FindChildByNameRecursive(spawnedPawn, "Player")) {
            controller->animator = playerVisual->GetComponent<RTBEngine::Animation::Animator>();
        }
    }

    WireSpawnedPlayerCamera(spawnedPawn);

    if (!controller->attackJoystick) {
        if (RTBEngine::ECS::Scene* scene =
                RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene()) {
            controller->attackJoystick = FindSceneAttackJoystick(scene);
        }
    }

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (scene) {
        if (controller->animator && !controller->animator->AreBoneGOsCreated()) {
            controller->animator->CreateBoneGameObjects(scene);
        }
    }

    if (auto* statsApplier = spawnedPawn->GetComponent<CharacterStatsApplier>()) {
        PlayerCharacterSelection& selection = PlayerCharacterSelection::GetInstance();
        selection.EnsureSelectionFromCatalog();
        if (CharacterDefinition* definition = selection.GetSelectedDefinition()) {
            CharacterStatsApplier::ApplyDefinition(spawnedPawn, *definition);
        }
    }

    controller->RefreshAfterSpawn();
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);

    if (onlinePlayerManager) {
        onlinePlayerManager->localPlayerObject = spawnedPawn;
    }
}
} // namespace

using ThisClass = PlayerPawnSpawner;

RTB_REGISTER_COMPONENT(PlayerPawnSpawner)
    RTB_PROPERTY_COMPONENT(onlinePlayerManager, OnlinePlayerManager)
    RTB_PROPERTY_COMPONENT(roundManager, RoundManager)
RTB_END_REGISTER(PlayerPawnSpawner)

void PlayerPawnSpawner::OnAwake()
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

    const RTBEngine::Math::Vector3 spawnPosition = owner->GetWorldPosition();
    const RTBEngine::Math::Quaternion spawnRotation = owner->GetWorldRotation();

    spawnedPawn = CharacterGameplaySpawner::InstantiateFromDefinition(
        *definition,
        spawnPosition,
        spawnRotation);
    if (!spawnedPawn) {
        RTB_WARN("[PlayerPawnSpawner] Failed to instantiate gameplay prefab for '" +
                 definition->characterId + "'.");
        return;
    }

    spawnedPawn->SetName("Player");
    WireSpawnedPlayerReferences(spawnedPawn, onlinePlayerManager);

    if (onlinePlayerManager) {
        onlinePlayerManager->localPlayerObject = spawnedPawn;
    }

    if (roundManager) {
        roundManager->playerObject = spawnedPawn;
    }

    PlayerRegistry::GetInstance().RegisterPlayerPawn(spawnedPawn);

    owner->SetActive(false);
}
