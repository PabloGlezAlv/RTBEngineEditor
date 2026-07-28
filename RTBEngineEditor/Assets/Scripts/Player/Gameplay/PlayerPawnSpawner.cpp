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

    if (!controller->cameraObject) {
        RTB_WARN("[PlayerPawnSpawner] cameraObject is not assigned on spawned player pawn.");
        return;
    }

    if (controller->cameraObject->GetParent() != spawnedPawn) {
        controller->cameraObject->SetParent(spawnedPawn);
    }

    RTBEngine::Scene::Scene* scene =
        RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    if (auto* cameraComponent =
            controller->cameraObject->GetComponent<RTBEngine::Scene::CameraComponent>()) {
        scene->SetMainCamera(cameraComponent);
        cameraComponent->SyncNow();
    }
}

void WireSpawnedPlayerReferences(
    RTBEngine::Scene::GameObject* spawnedPawn,
    OnlinePlayerManager* onlinePlayerManager,
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

    controller->attackJoystick = attackJoystick;
    if (!controller->attackJoystick) {
        RTB_WARN("[PlayerPawnSpawner] attackJoystick is not assigned on PlayerPawnSpawner.");
    }

    if (auto* specialCharge = spawnedPawn->GetComponent<PlayerSpecialAttackCharge>()) {
        specialCharge->specialAttackJoystick = specialAttackJoystick;
        specialCharge->readyIcon = specialAttackReadyIcon;
        if (!specialCharge->specialAttackJoystick) {
            RTB_WARN("[PlayerPawnSpawner] specialAttackJoystick is not assigned on PlayerPawnSpawner.");
        }
        if (!specialCharge->readyIcon) {
            RTB_WARN("[PlayerPawnSpawner] specialAttackReadyIcon is not assigned on PlayerPawnSpawner.");
        }
        specialCharge->RefreshAfterSpawn();
    }

    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
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
    RTB_PROPERTY_COMPONENT(attackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(specialAttackJoystick, UIJoystick)
    RTB_PROPERTY_COMPONENT(specialAttackReadyIcon, UIImage)
RTB_END_REGISTER(PlayerPawnSpawner)

void PlayerPawnSpawner::OnAwake()
{
    if (!owner) {
        return;
    }

    // Drop stale pawns left behind after Play→Stop scene reloads.
    PlayerRegistry::GetInstance().Clear();

    // Avoid duplicating a scene-authored Player (editor load runs Awake on spawners).
    if (RTBEngine::Scene::Scene* activeScene =
            RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
        for (const auto& gameObjectPtr : activeScene->GetGameObjects()) {
            RTBEngine::Scene::GameObject* candidate = gameObjectPtr.get();
            if (!candidate || candidate->GetParent() != nullptr) {
                continue;
            }
            if (candidate->GetName() == "Player") {
                spawnedPawn = candidate;
                WireSpawnedPlayerReferences(
                    spawnedPawn,
                    onlinePlayerManager,
                    attackJoystick,
                    specialAttackJoystick,
                    specialAttackReadyIcon);
                if (onlinePlayerManager) {
                    onlinePlayerManager->localPlayerObject = spawnedPawn;
                }
                if (roundManager) {
                    roundManager->playerObject = spawnedPawn;
                }
                PlayerRegistry::GetInstance().RegisterPlayerPawn(spawnedPawn);
                owner->SetActive(false);
                return;
            }
        }
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
    WireSpawnedPlayerReferences(
        spawnedPawn,
        onlinePlayerManager,
        attackJoystick,
        specialAttackJoystick,
        specialAttackReadyIcon);

    if (onlinePlayerManager) {
        onlinePlayerManager->localPlayerObject = spawnedPawn;
    }

    if (roundManager) {
        roundManager->playerObject = spawnedPawn;
    }

    PlayerRegistry::GetInstance().RegisterPlayerPawn(spawnedPawn);

    owner->SetActive(false);
}
