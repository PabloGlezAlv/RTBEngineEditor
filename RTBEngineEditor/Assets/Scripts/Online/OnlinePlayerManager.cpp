#include "OnlinePlayerManager.h"
#include "PlayerAmmoSystem.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "CharacterGameplaySpawner.h"
#include "CharacterStatsApplier.h"
#include "OnlineDisplayNameHelper.h"
#include "OnlineGameNetMessages.h"
#include "PlayerCharacterSelection.h"
#include "PlayerNameplateUI.h"
#include "PlayerRegistry.h"
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

    std::string ResolveLocalCharacterId()
    {
        PlayerCharacterSelection& selection = PlayerCharacterSelection::GetInstance();
        selection.EnsureSelectionFromCatalog();
        return CharacterGameplaySpawner::SanitizeCharacterId(selection.GetSelectedCharacterId());
    }

    RTBEngine::ECS::GameObject* FindRemotePawnBySlot(int playerSlot)
    {
        RTBEngine::ECS::GameObject* pawn = PlayerRegistry::GetInstance().FindBySlot(playerSlot);
        if (!pawn) {
            return nullptr;
        }

        if (RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>()) {
            if (identity->IsLocallyControlled()) {
                return nullptr;
            }
        }

        return pawn;
    }

    void DisableLocalOnlyControllerFeatures(ThirdPersonCharacterController* controller)
    {
        if (!controller) {
            return;
        }

        controller->attackJoystick = nullptr;
        if (controller->cameraObject) {
            controller->cameraObject->SetActive(false);
            controller->cameraObject = nullptr;
        }
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
    authoritativePlayerBinds.clear();
    spawnedCharacterIdsBySlot.clear();

    sessionProfileSubscription =
        RTBEngine::Online::OnlineSystem::GetInstance().SubscribeToPlayerSessionProfileChanged(
            [this](const RTBEngine::Online::PlayerSessionProfileChangedEvent& event) {
                if (!event.removed) {
                    EnsureRemotePawnsSpawned();
                }
            });

    ConfigureOnlinePlayers();
}

void OnlinePlayerManager::OnUpdate(float deltaTime)
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    EnsureRemotePawnsSpawned();
    ProcessPlayerNetworkBinds();
    ProcessNetworkRoundEvents(this);
    GameNet::OnlineGameNetSubsystem::DetectAndDespawnDisconnectedPlayers();
}

void OnlinePlayerManager::OnDestroy()
{
    sessionProfileSubscription.Reset();
    authoritativePlayerBinds.clear();
    spawnedCharacterIdsBySlot.clear();
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

    if (PlayerAmmoSystem* localAmmo = localPlayerObject->GetComponent<PlayerAmmoSystem>()) {
        localAmmo->RefreshNetworkState();
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        SyncAuthoritativeRemotePlayers();
    } else {
        ProcessPlayerNetworkBinds();
        EnsureRemotePawnsSpawned();
    }
}

void OnlinePlayerManager::RegisterPlayerSessionProfiles(
    const std::vector<RTBEngine::Online::OnlineUserId>& members)
{
    if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        return;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    const std::string localCharacterId = ResolveLocalCharacterId();
    if (localCharacterId.empty()) {
        RTB_WARN("[OnlinePlayerManager] Host has no valid local character selection to sync.");
    }

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

        if (member == online.GetLocalUserId()) {
            if (localCharacterId.empty()) {
                continue;
            }

            snapshot.characterId = localCharacterId;
            GameNet::OnlineGameNetSubsystem::MergePlayerSessionSnapshot(snapshot);
            continue;
        }

        GameNet::PlayerSessionSnapshot existingSnapshot;
        if (!GameNet::OnlineGameNetSubsystem::TryGetPlayerSessionSnapshot(playerSlot, existingSnapshot) ||
            existingSnapshot.characterId.empty()) {
            continue;
        }

        existingSnapshot.displayName = displayName;
        existingSnapshot.ownerUserIdKey = member.ToString();
        GameNet::OnlineGameNetSubsystem::MergePlayerSessionSnapshot(existingSnapshot);
    }

    GameNet::OnlineGameNetSubsystem::BroadcastAllKnownPlayerSessionProfiles();
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

    const std::string characterId = ResolveLocalCharacterId();
    if (characterId.empty()) {
        RTB_WARN("[OnlinePlayerManager] Client has no valid local character selection to sync.");
        return;
    }

    RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
    RTBEngine::Online::OnlinePlayerProfile profile;
    profile.userId = online.GetLocalUserId();
    profile.playerSlot = playerSlot;
    profile.displayName = displayName;
    online.SetPlayerSessionProfile(profile);

    GameNet::OnlineGameNetSubsystem::SendPlayerSessionProfileToHost(displayName, characterId);
}

void OnlinePlayerManager::RequestRemotePawnSync()
{
    EnsureRemotePawnsSpawned();
    ProcessPlayerNetworkBinds();
}

void OnlinePlayerManager::SyncAuthoritativeRemotePlayers()
{
    EnsureRemotePawnsSpawned();

    if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        return;
    }

    for (const GameNet::PlayerNetworkBindSnapshot& bind : authoritativePlayerBinds) {
        GameNet::OnlineGameNetSubsystem::BroadcastPlayerNetworkBind(bind);
    }
}

std::string OnlinePlayerManager::ResolveCharacterIdForSlot(
    int playerSlot,
    const RTBEngine::Online::OnlineUserId& ownerUserId) const
{
    GameNet::PlayerSessionSnapshot snapshot;
    if (GameNet::OnlineGameNetSubsystem::TryGetPlayerSessionSnapshot(playerSlot, snapshot)) {
        if (const std::string resolved =
                CharacterGameplaySpawner::SanitizeCharacterId(snapshot.characterId);
            !resolved.empty()) {
            return resolved;
        }
    }

    if (ownerUserId == RTBEngine::Online::OnlineGameplayNet::GetLocalUserId()) {
        return ResolveLocalCharacterId();
    }

    return {};
}

void OnlinePlayerManager::EnsureRemotePawnsSpawned()
{
    if (!localPlayerObject) {
        return;
    }

    const std::vector<RTBEngine::Online::OnlineUserId> members =
        RTBEngine::Online::OnlineGameplayNet::GetOrderedLobbyMembers();
    if (members.size() < 2) {
        return;
    }

    const RTBEngine::Online::OnlineUserId localUserId =
        RTBEngine::Online::OnlineGameplayNet::GetLocalUserId();

    bool spawnedAnyRemotePawn = false;

    for (std::size_t memberIndex = 0; memberIndex < members.size(); ++memberIndex) {
        const RTBEngine::Online::OnlineUserId& member = members[memberIndex];
        if (member == localUserId) {
            continue;
        }

        const int playerSlot = static_cast<int>(memberIndex);
        const std::string characterId = ResolveCharacterIdForSlot(playerSlot, member);
        if (characterId.empty()) {
            continue;
        }

        const auto trackedIt = spawnedCharacterIdsBySlot.find(playerSlot);
        RTBEngine::ECS::GameObject* existingPawn = FindRemotePawnBySlot(playerSlot);
        if (trackedIt != spawnedCharacterIdsBySlot.end() &&
            trackedIt->second == characterId &&
            existingPawn) {
            continue;
        }

        if (existingPawn) {
            DespawnRemotePawnForSlot(playerSlot);
        }

        RTBEngine::ECS::GameObject* remotePawn = SpawnRemotePawn(
            member,
            playerSlot,
            characterId,
            remoteSpawnOffsetX * static_cast<float>(memberIndex));
        if (!remotePawn) {
            continue;
        }

        spawnedRemotePawns.push_back(remotePawn);
        spawnedCharacterIdsBySlot[playerSlot] = characterId;
        spawnedAnyRemotePawn = true;
    }

    if (spawnedAnyRemotePawn) {
        ProcessPlayerNetworkBinds();
    }
}

void OnlinePlayerManager::DespawnRemotePawnForSlot(int playerSlot)
{
    RTBEngine::ECS::GameObject* pawn = FindRemotePawnBySlot(playerSlot);
    if (!pawn) {
        spawnedCharacterIdsBySlot.erase(playerSlot);
        return;
    }

    PlayerRegistry::GetInstance().Unregister(pawn);
    spawnedRemotePawns.erase(
        std::remove(spawnedRemotePawns.begin(), spawnedRemotePawns.end(), pawn),
        spawnedRemotePawns.end());
    spawnedCharacterIdsBySlot.erase(playerSlot);

    authoritativePlayerBinds.erase(
        std::remove_if(
            authoritativePlayerBinds.begin(),
            authoritativePlayerBinds.end(),
            [playerSlot](const GameNet::PlayerNetworkBindSnapshot& bind) {
                return bind.playerSlot == playerSlot;
            }),
        authoritativePlayerBinds.end());

    if (RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene()) {
        scene->RemoveGameObject(pawn);
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
        PlayerRegistry::GetInstance().RegisterPlayerPawn(pawn);
        return;
    }

    if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
        rigidBodyComponent->bodyType = RTBEngine::Physics::RigidBodyType::Dynamic;
    } else {
        rigidBodyComponent->bodyType = RTBEngine::Physics::RigidBodyType::Kinematic;
    }

    rigidBodyComponent->OnValidate();

    PlayerRegistry::GetInstance().RegisterPlayerPawn(pawn);
}

RTBEngine::ECS::GameObject* OnlinePlayerManager::SpawnRemotePawn(
    const RTBEngine::Online::OnlineUserId& ownerUserId,
    int playerSlot,
    const std::string& characterId,
    float spawnOffsetX)
{
    if (!localPlayerObject || characterId.empty()) {
        return nullptr;
    }

    const std::string resolvedCharacterId = CharacterGameplaySpawner::SanitizeCharacterId(characterId);
    CharacterDefinition* definition = CharacterCatalog::GetInstance().GetById(resolvedCharacterId);
    if (!definition) {
        RTB_WARN("[OnlinePlayerManager] Unknown character id '" + resolvedCharacterId +
                 "' for remote player slot " + std::to_string(playerSlot) + ".");
        return nullptr;
    }

    const RTBEngine::Math::Vector3 spawnPosition =
        localPlayerObject->GetWorldPosition() + RTBEngine::Math::Vector3(spawnOffsetX, 0.0f, 0.0f);

    RTBEngine::ECS::GameObject* spawnedPawn = CharacterGameplaySpawner::InstantiateFromDefinition(
        *definition,
        spawnPosition,
        localPlayerObject->GetWorldRotation());
    if (!spawnedPawn) {
        RTB_WARN("[OnlinePlayerManager] Failed to instantiate remote player pawn for '" +
                 resolvedCharacterId + "'.");
        return nullptr;
    }

    CharacterStatsApplier::ApplyDefinition(spawnedPawn, *definition);

    if (ThirdPersonCharacterController* controller =
            spawnedPawn->GetComponent<ThirdPersonCharacterController>()) {
        DisableLocalOnlyControllerFeatures(controller);
    }

    spawnedPawn->SetName(playerSlot == 0 ? "Remote Host Player" : "Remote Client Player");
    ConfigurePawn(spawnedPawn, ownerUserId, playerSlot);
    return spawnedPawn;
}

void OnlinePlayerManager::RemovePawnFromTracking(RTBEngine::ECS::GameObject* pawn, int playerSlot)
{
    if (pawn) {
        PlayerRegistry::GetInstance().Unregister(pawn);
        spawnedRemotePawns.erase(
            std::remove(spawnedRemotePawns.begin(), spawnedRemotePawns.end(), pawn),
            spawnedRemotePawns.end());
    }

    spawnedCharacterIdsBySlot.erase(playerSlot);

    authoritativePlayerBinds.erase(
        std::remove_if(
            authoritativePlayerBinds.begin(),
            authoritativePlayerBinds.end(),
            [playerSlot](const GameNet::PlayerNetworkBindSnapshot& bind) {
                return bind.playerSlot == playerSlot;
            }),
        authoritativePlayerBinds.end());
}
