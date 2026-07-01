#include "OnlinePlayerManager.h"
#include "PlayerAmmoSystem.h"

#include "OnlineDisplayNameHelper.h"
#include "OnlineGameNetMessages.h"
#include "PlayerNameplateUI.h"
#include "ProjectileAttackAbility.h"
#include "RoundManager.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/NetworkTransform.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Online/OnlinePlayerProfile.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Scene/RigidBodyComponent.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Physics/RigidBody.h>

#include <algorithm>

using ThisClass = OnlinePlayerManager;

namespace {

    void RefreshNameplatesForPawn(RTBEngine::ECS::GameObject* pawn)
    {
        if (!pawn) {
            return;
        }

        if (PlayerNameplateUI* nameplate = pawn->GetComponentInChildren<PlayerNameplateUI>()) {
            nameplate->ForceRefreshDisplayName();
        }
    }

    void ProcessPlayerNetworkBinds()
    {
        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return;
        }

        GameNet::PlayerNetworkBindSnapshot bind;
        while (GameNet::OnlineGameNetSubsystem::TryConsumePlayerNetworkBind(bind)) {
            if (GameNet::OnlineGameNetSubsystem::ApplyPlayerNetworkBind(bind)) {
                continue;
            }

            // Remote pawn may not exist yet; keep the bind until ConfigureOnlinePlayers finishes.
            GameNet::OnlineGameNetSubsystem::RequeuePlayerNetworkBind(bind);
            break;
        }
    }

    void ProcessNetworkRoundEvents(OnlinePlayerManager* onlinePlayerManager)
    {
        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return;
        }

        RoundManager* roundManager = onlinePlayerManager ? onlinePlayerManager->roundManager : nullptr;
        if (!roundManager) {
            return;
        }

        GameNet::RoundCountdownSnapshot roundCountdown;
        while (GameNet::OnlineGameNetSubsystem::TryConsumeRoundCountdown(roundCountdown)) {
            roundManager->ApplyNetworkRoundCountdown(
                roundCountdown.roundNumber,
                roundCountdown.duration);
        }

        GameNet::RoundStartSnapshot roundStart;
        while (GameNet::OnlineGameNetSubsystem::TryConsumeRoundStart(roundStart)) {
            roundManager->ApplyNetworkRoundStart(roundStart.roundNumber, roundStart.enemyCount);
        }

        GameNet::EnemySpawnSnapshot enemySpawn;
        while (GameNet::OnlineGameNetSubsystem::TryConsumeEnemySpawn(enemySpawn)) {
            roundManager->ApplyNetworkEnemySpawn(
                enemySpawn.roundNumber,
                enemySpawn.spawnPointIndex,
                enemySpawn.spawnIndex,
                enemySpawn.networkId);
        }
    }

    std::string ResolveLobbyMemberDisplayName(
        const RTBEngine::Online::OnlineUserId& ownerUserId,
        int playerSlot)
    {
        RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
        if (ownerUserId == online.GetLocalUserId()) {
            if (const RTBEngine::Online::IOnlineIdentity* identity = online.GetIdentity()) {
                if (!identity->GetDisplayName().empty()) {
                    return identity->GetDisplayName();
                }
            }
        }

        const std::string lobbyName = online.GetLobbyMemberDisplayName(ownerUserId);
        if (!lobbyName.empty()) {
            return lobbyName;
        }

        return "Player " + std::to_string(playerSlot + 1);
    }

}

RTB_REGISTER_COMPONENT(OnlinePlayerManager)
    RTB_PROPERTY_GAMEOBJECT(localPlayerObject)
    RTB_PROPERTY_COMPONENT(roundManager, RoundManager)
    RTB_PROPERTY_RANGE(remoteSpawnOffsetX, 0.5f, 20.0f)
RTB_END_REGISTER(OnlinePlayerManager)

void OnlinePlayerManager::OnStart()
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    GameNet::OnlineGameNetSubsystem::Init();
    RTBEngine::Online::OnlineGameplayNet::ResetNetworkSession();
    RTBEngine::Online::OnlineSystem::GetInstance().ClearPlayerSessionProfiles();
    authoritativePlayerBinds.clear();
    authoritativePlayerSessionProfiles.clear();
    ConfigureOnlinePlayers();
}

void OnlinePlayerManager::OnUpdate(float deltaTime)
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    ProcessPlayerNetworkBinds();
    ProcessNetworkRoundEvents(this);
    GameNet::OnlineGameNetSubsystem::DetectAndDespawnDisconnectedPlayers();
}

void OnlinePlayerManager::OnDestroy()
{
    authoritativePlayerBinds.clear();
    authoritativePlayerSessionProfiles.clear();
}

void OnlinePlayerManager::OnFixedUpdate(float /*fixedDeltaTime*/)
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    GameNet::ProjectileSpawnSnapshot spawnSnapshot;
    while (GameNet::OnlineGameNetSubsystem::TryConsumeProjectileSpawn(spawnSnapshot)) {
        ProjectileAttackAbility::SpawnFromNetworkSnapshot(spawnSnapshot);
    }
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

    RegisterPlayerSessionProfiles(members);

    const RTBEngine::Online::OnlineUserId localUserId = RTBEngine::Online::OnlineGameplayNet::GetLocalUserId();
    const std::size_t localPlayerIndex = RTBEngine::Online::OnlineGameplayNet::GetLocalPlayerIndex();
    ConfigurePawn(localPlayerObject, members[localPlayerIndex], static_cast<int>(localPlayerIndex));
    SendLocalPlayerSessionProfile();

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

    if (roundManager) {
        roundManager->RefreshTrackedPlayers();
    }

    if (PlayerAmmoSystem* localAmmo = localPlayerObject->GetComponent<PlayerAmmoSystem>()) {
        localAmmo->RefreshNetworkState();
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        for (const GameNet::PlayerNetworkBindSnapshot& bind : authoritativePlayerBinds) {
            GameNet::OnlineGameNetSubsystem::BroadcastPlayerNetworkBind(bind);
        }
    } else {
        ProcessPlayerNetworkBinds();
    }
}

void OnlinePlayerManager::RegisterPlayerSessionProfiles(
    const std::vector<RTBEngine::Online::OnlineUserId>& members)
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        return;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();

    for (std::size_t memberIndex = 0; memberIndex < members.size(); ++memberIndex) {
        const RTBEngine::Online::OnlineUserId& member = members[memberIndex];
        const int playerSlot = static_cast<int>(memberIndex);
        const std::string displayName = ResolveLobbyMemberDisplayName(member, playerSlot);

        RTBEngine::Online::OnlinePlayerProfile profile;
        profile.userId = member;
        profile.playerSlot = playerSlot;
        profile.displayName = displayName;
        online.SetPlayerSessionProfile(profile);

        GameNet::PlayerSessionSnapshot snapshot;
        snapshot.playerSlot = playerSlot;
        snapshot.ownerUserIdKey = member.ToString();
        snapshot.displayName = displayName;
        authoritativePlayerSessionProfiles.push_back(snapshot);
        GameNet::OnlineGameNetSubsystem::BroadcastPlayerSessionSnapshot(snapshot);
    }
}

void OnlinePlayerManager::SendLocalPlayerSessionProfile()
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
        RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        return;
    }

    const int playerSlot = static_cast<int>(RTBEngine::Online::OnlineGameplayNet::GetLocalPlayerIndex());
    if (playerSlot < 0) {
        return;
    }

    const std::string displayName = GameNet::ResolveLocalDisplayName();
    if (displayName.empty()) {
        return;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    RTBEngine::Online::OnlinePlayerProfile profile;
    profile.userId = online.GetLocalUserId();
    profile.playerSlot = playerSlot;
    profile.displayName = displayName;
    online.SetPlayerSessionProfile(profile);

    GameNet::OnlineGameNetSubsystem::SendPlayerSessionProfileToHost(displayName);
}

void OnlinePlayerManager::MergeAuthoritativeSessionProfile(
    const GameNet::PlayerSessionSnapshot& snapshot)
{
    if (snapshot.playerSlot < 0 || snapshot.displayName.empty()) {
        return;
    }

    for (GameNet::PlayerSessionSnapshot& profile : authoritativePlayerSessionProfiles) {
        if (profile.playerSlot == snapshot.playerSlot) {
            profile = snapshot;
            return;
        }
    }

    authoritativePlayerSessionProfiles.push_back(snapshot);
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

    if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        const std::uint32_t networkId =
            RTBEngine::Online::OnlineGameplayNet::AllocateNetworkObjectId();
        if (networkId != RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
            identity->SetNetworkId(networkId);

            if (RTBEngine::ECS::NetworkTransform* networkTransform =
                    pawn->GetComponent<RTBEngine::ECS::NetworkTransform>()) {
                networkTransform->OnValidate();
            }

            GameNet::PlayerNetworkBindSnapshot bind;
            bind.playerSlot = playerSlot;
            bind.networkId = networkId;
            authoritativePlayerBinds.push_back(bind);
            GameNet::OnlineGameNetSubsystem::BroadcastPlayerNetworkBind(bind);
        }
    }

    if (PlayerAmmoSystem* ammoSystem = pawn->GetComponent<PlayerAmmoSystem>()) {
        ammoSystem->RefreshNetworkState();
    }

    RefreshNameplatesForPawn(pawn);

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

void OnlinePlayerManager::RemovePawnFromTracking(RTBEngine::ECS::GameObject* pawn, int playerSlot)
{
    if (pawn) {
        spawnedRemotePawns.erase(
            std::remove(spawnedRemotePawns.begin(), spawnedRemotePawns.end(), pawn),
            spawnedRemotePawns.end());
    }

    authoritativePlayerBinds.erase(
        std::remove_if(
            authoritativePlayerBinds.begin(),
            authoritativePlayerBinds.end(),
            [playerSlot](const GameNet::PlayerNetworkBindSnapshot& bind) {
                return bind.playerSlot == playerSlot;
            }),
        authoritativePlayerBinds.end());

    authoritativePlayerSessionProfiles.erase(
        std::remove_if(
            authoritativePlayerSessionProfiles.begin(),
            authoritativePlayerSessionProfiles.end(),
            [playerSlot](const GameNet::PlayerSessionSnapshot& profile) {
                return profile.playerSlot == playerSlot;
            }),
        authoritativePlayerSessionProfiles.end());
}
