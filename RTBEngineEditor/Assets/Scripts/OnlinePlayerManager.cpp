#include "OnlinePlayerManager.h"

#include <RTBEngine/ECS/NetworkIdentity.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Physics/RigidBody.h>

using ThisClass = OnlinePlayerManager;

RTB_REGISTER_COMPONENT(OnlinePlayerManager)
    RTB_PROPERTY_GAMEOBJECT(localPlayerObject)
    RTB_PROPERTY_RANGE(remoteSpawnOffsetX, 0.5f, 20.0f)
RTB_END_REGISTER(OnlinePlayerManager)

void OnlinePlayerManager::OnStart()
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    ConfigureOnlinePlayers();
}

void OnlinePlayerManager::ConfigureOnlinePlayers()
{
    if (!localPlayerObject) {
        RTB_WARN("[OnlinePlayerManager] Assign localPlayerObject before starting an online match.");
        return;
    }

    const std::vector<RTBEngine::Online::OnlineUserId> members = RTBEngine::Online::OnlineGameplayNet::GetOrderedLobbyMembers();
    if (members.size() < 2) {
        RTB_WARN("[OnlinePlayerManager] Expected at least two lobby members for online play.");
        return;
    }

    const RTBEngine::Online::OnlineUserId localUserId = RTBEngine::Online::OnlineGameplayNet::GetLocalUserId();
    const std::size_t localPlayerIndex = RTBEngine::Online::OnlineGameplayNet::GetLocalPlayerIndex();
    ConfigurePawn(localPlayerObject, members[localPlayerIndex], static_cast<int>(localPlayerIndex));

    for (std::size_t memberIndex = 0; memberIndex < members.size(); ++memberIndex) {
        const RTBEngine::Online::OnlineUserId& member = members[memberIndex];
        if (member == localUserId) {
            continue;
        }

        RTBEngine::ECS::GameObject* remotePawn = SpawnRemotePawn(
            member,
            static_cast<int>(memberIndex),
            remoteSpawnOffsetX * static_cast<float>(memberIndex));
        if (remotePawn) {
            spawnedRemotePawns.push_back(remotePawn);
        }
    }
}

void OnlinePlayerManager::ConfigurePawn(
    RTBEngine::ECS::GameObject* pawn,
    const RTBEngine::Online::OnlineUserId& ownerUserId,
    int playerSlot)
{
    if (!pawn) {
        return;
    }

    RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();
    if (!identity) {
        RTB_WARN("[OnlinePlayerManager] Pawn '" + pawn->GetName() + "' is missing NetworkIdentity.");
        return;
    }

    identity->SetOwnerUserId(ownerUserId);
    identity->SetNetworkPlayerSlot(playerSlot);

    if (ThirdPersonCharacterController* controller = pawn->GetComponent<ThirdPersonCharacterController>()) {
        if (!identity->IsLocallyControlled()) {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                // Host only needs FixedUpdate for remote simulation; skip Update/LateUpdate camera logic.
                controller->SetUpdateTickEnabled(false);
            }
            // Clients keep Update/LateUpdate enabled so animator can run after NetworkTransform in LateUpdate.
        }
    }

    auto* rigidBodyComponent = pawn->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (!rigidBodyComponent) {
        return;
    }

    // Host runs Bullet simulation for all pawns; clients are kinematic display proxies.
    if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        rigidBodyComponent->bodyType = RTBEngine::Physics::RigidBodyType::Dynamic;
    } else {
        rigidBodyComponent->bodyType = RTBEngine::Physics::RigidBodyType::Kinematic;
    }

    rigidBodyComponent->OnValidate();
}

RTBEngine::ECS::GameObject* OnlinePlayerManager::SpawnRemotePawn(
    const RTBEngine::Online::OnlineUserId& ownerUserId,
    int playerSlot,
    float spawnOffsetX)
{
    if (!localPlayerObject) {
        return nullptr;
    }

    if (!playerPrefab) {
        playerPrefab = RTBEngine::ECS::Prefab::CreateFromGameObject(localPlayerObject);
        if (!playerPrefab) {
            RTB_WARN("[OnlinePlayerManager] Failed to create player prefab from localPlayerObject.");
            return nullptr;
        }
    }

    const RTBEngine::Math::Vector3 spawnPosition =
        localPlayerObject->GetWorldPosition() + RTBEngine::Math::Vector3(spawnOffsetX, 0.0f, 0.0f);

    RTBEngine::ECS::GameObject* spawnedPawn = RTBEngine::ECS::SceneManager::GetInstance().Instantiate(
        *playerPrefab,
        spawnPosition,
        localPlayerObject->GetWorldRotation());
    if (!spawnedPawn) {
        RTB_WARN("[OnlinePlayerManager] Failed to instantiate remote player pawn.");
        return nullptr;
    }

    spawnedPawn->SetName(playerSlot == 0 ? "Remote Host Player" : "Remote Client Player");
    ConfigurePawn(spawnedPawn, ownerUserId, playerSlot);
    return spawnedPawn;
}
