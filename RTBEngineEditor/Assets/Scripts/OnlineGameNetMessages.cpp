#include "OnlineGameNetMessages.h"

#include "EnemyMeleeAI.h"
#include "HealthComponent.h"
#include "OnlineDisplayNameHelper.h"
#include "OnlinePlayerManager.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/Scene/NetworkIdentity.h>
#include <RTBEngine/Scene/NetworkTransform.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Online/OnlineMessageBus.h>
#include <RTBEngine/Online/OnlineMessageCodec.h>
#include <RTBEngine/Online/OnlinePlayerProfile.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include <RTBEngine/Online/IOnlineTransport.h>

#include <cmath>
#include <deque>
#include <unordered_map>
#include <unordered_set>

namespace GameNet {

    namespace {

        bool subsystemInitialized = false;
        std::unordered_map<std::string, PlayerCombatInput> latestCombatInputs;
        std::deque<ProjectileSpawnSnapshot> pendingProjectileSpawns;
        std::deque<EnemySpawnSnapshot> pendingEnemySpawns;
        std::deque<RoundStartSnapshot> pendingRoundStarts;
        std::deque<PlayerNetworkBindSnapshot> pendingPlayerNetworkBinds;
        std::unordered_map<int, float> lastBroadcastPlayerHealth;
        std::string pendingMainMenuMessage;
        std::string activeMatchNotification;
        float matchNotificationSecondsRemaining = 0.0f;

        constexpr float kMatchNotificationDuration = 5.0f;
        constexpr char kMainMenuSceneFallback[] = "Assets/Scenes/MainMenu.lua";

        void QueueMatchNotification(const std::string& message)
        {
            activeMatchNotification = message;
            matchNotificationSecondsRemaining = kMatchNotificationDuration;
        }

        void SetPendingMainMenuMessage(const std::string& message)
        {
            pendingMainMenuMessage = message;
        }

        void LeaveActiveOnlineLobby()
        {
            RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
            RTBEngine::Online::IOnlineLobby* lobby = online.GetLobby();
            if (!lobby || lobby->GetCurrentLobby().lobbyId.empty()) {
                return;
            }

            if (lobby->GetCurrentLobby().isOwner) {
                lobby->DestroyLobby();
            } else {
                lobby->LeaveLobby();
            }
        }

        void LoadMainMenuScene(const char* scenePath)
        {
            const char* targetScene = scenePath && scenePath[0] != '\0'
                ? scenePath
                : kMainMenuSceneFallback;
            RTBEngine::Core::Time::SetPaused(false);
            RTBEngine::ECS::SceneManager& sceneManager =
                RTBEngine::ECS::SceneManager::GetInstance();
            if (!sceneManager.RequestSceneLoad(targetScene)) {
                sceneManager.LoadScene(targetScene);
            }
        }

        OnlinePlayerManager* FindOnlinePlayerManager()
        {
            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            if (!scene) {
                return nullptr;
            }

            for (const auto& gameObject : scene->GetGameObjects()) {
                if (!gameObject) {
                    continue;
                }

                if (OnlinePlayerManager* manager = gameObject->GetComponent<OnlinePlayerManager>()) {
                    return manager;
                }
            }

            return nullptr;
        }

        int ResolvePlayerSlotForUser(const RTBEngine::Online::OnlineUserId& userId)
        {
            if (!userId.IsValid()) {
                return -1;
            }

            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            if (scene) {
                const std::string userKey = userId.ToString();
                for (const auto& gameObject : scene->GetGameObjects()) {
                    if (!gameObject) {
                        continue;
                    }

                    RTBEngine::ECS::NetworkIdentity* identity =
                        gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
                    if (!identity || identity->networkPlayerSlot < 0) {
                        continue;
                    }

                    if (identity->networkOwnerUserId == userKey) {
                        return identity->networkPlayerSlot;
                    }
                }
            }

            const std::vector<RTBEngine::Online::OnlineUserId> members =
                RTBEngine::Online::OnlineGameplayNet::GetOrderedLobbyMembers();
            for (std::size_t index = 0; index < members.size(); ++index) {
                if (members[index] == userId) {
                    return static_cast<int>(index);
                }
            }

            return -1;
        }

        std::vector<std::uint8_t> BuildMatchPlayerLeftPayload(int playerSlot, const std::string& displayName)
        {
            std::vector<std::uint8_t> payload;
            RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, playerSlot);
            RTBEngine::Online::OnlineMessageCodec::AppendString(payload, displayName);
            return payload;
        }

        bool TryParseMatchPlayerLeftPayload(
            const RTBEngine::Online::OnlineMessageContext& context,
            int& outPlayerSlot,
            std::string& outDisplayName)
        {
            std::size_t offset = 0;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    outPlayerSlot)) {
                return false;
            }

            if (!RTBEngine::Online::OnlineMessageCodec::ReadString(
                    context.payload,
                    context.payloadSize,
                    offset,
                    outDisplayName) ||
                outDisplayName.empty()) {
                outDisplayName = "Player";
            }

            return outPlayerSlot >= 0;
        }

        void HandlePlayerLeaveNotice(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            std::string playerName;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadString(
                    context.payload, context.payloadSize, offset, playerName) ||
                playerName.empty()) {
                playerName = "Player";
            }

            const int playerSlot = ResolvePlayerSlotForUser(context.senderUserId);
            OnlineGameNetSubsystem::HostNotifyPlayerDisconnected(playerSlot, playerName);
        }

        void HandlePlayerLeft(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            int playerSlot = -1;
            std::string playerName;
            if (!TryParseMatchPlayerLeftPayload(context, playerSlot, playerName)) {
                return;
            }

            OnlineGameNetSubsystem::ApplyPlayerDespawnForSlot(playerSlot);
            QueueMatchNotification(playerName + " has left the game.");
        }

        void HandleHostAbandoned(const RTBEngine::Online::OnlineMessageContext& /*context*/)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            SetPendingMainMenuMessage("The host abandoned the match.");
            OnlineGameNetSubsystem::Shutdown();
            LeaveActiveOnlineLobby();
            LoadMainMenuScene(kMainMenuSceneFallback);
        }

        std::vector<std::uint8_t> BuildEnemySpawnPayload(const EnemySpawnSnapshot& snapshot)
        {
            std::vector<std::uint8_t> bytes;
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.networkId);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.roundNumber);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.spawnPointIndex);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.spawnIndex);
            return bytes;
        }

        std::vector<std::uint8_t> BuildRoundStartPayload(const RoundStartSnapshot& snapshot)
        {
            std::vector<std::uint8_t> bytes;
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.roundNumber);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.enemyCount);
            return bytes;
        }

        RTBEngine::ECS::GameObject* FindGameObjectByNetworkId(std::uint32_t networkId)
        {
            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            if (!scene || networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
                return nullptr;
            }

            for (const auto& gameObject : scene->GetGameObjects()) {
                if (!gameObject) {
                    continue;
                }

                RTBEngine::ECS::NetworkIdentity* identity =
                    gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
                if (identity && identity->GetNetworkId() == networkId) {
                    return gameObject.get();
                }
            }

            return nullptr;
        }

        std::vector<std::uint8_t> BuildCombatInputPayload(const PlayerCombatInput& input)
        {
            std::vector<std::uint8_t> bytes;
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, input.attackSequence);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, input.attackDirX);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, input.attackDirZ);
            return bytes;
        }

        std::vector<std::uint8_t> BuildProjectileSpawnPayload(const ProjectileSpawnSnapshot& snapshot)
        {
            std::vector<std::uint8_t> bytes;
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.spawnId);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.ownerPlayerSlot);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.origin.x);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.origin.y);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.origin.z);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.direction.x);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.direction.y);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.direction.z);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.speed);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.maxDistance);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.radius);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.damage);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.instigatorTeam);
            const std::uint8_t ignoreSameTeamFlag = snapshot.ignoreSameTeam ? 1 : 0;
            const std::uint8_t destroyOnHitFlag = snapshot.destroyOnHit ? 1 : 0;
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, ignoreSameTeamFlag);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, destroyOnHitFlag);
            RTBEngine::Online::OnlineMessageCodec::AppendValue(bytes, snapshot.maxHits);
            return bytes;
        }

        void HandleCombatInput(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            PlayerCombatInput input;
            input.senderUserId = context.senderUserId;

            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, input.attackSequence) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, input.attackDirX) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, input.attackDirZ) ||
                input.attackSequence == 0) {
                return;
            }

            latestCombatInputs[input.senderUserId.ToString()] = input;
        }

        void HandleProjectileSpawn(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            ProjectileSpawnSnapshot snapshot;
            std::uint8_t ignoreSameTeamFlag = 0;
            std::uint8_t destroyOnHitFlag = 0;

            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.spawnId) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.ownerPlayerSlot) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.origin.x) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.origin.y) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.origin.z) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.direction.x) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.direction.y) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.direction.z) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.speed) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.maxDistance) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.radius) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.damage) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.instigatorTeam) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, ignoreSameTeamFlag) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, destroyOnHitFlag) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(context.payload, context.payloadSize, offset, snapshot.maxHits) ||
                snapshot.spawnId == 0) {
                return;
            }

            snapshot.ignoreSameTeam = ignoreSameTeamFlag != 0;
            snapshot.destroyOnHit = destroyOnHitFlag != 0;
            pendingProjectileSpawns.push_back(snapshot);
        }

        RTBEngine::ECS::GameObject* FindPawnByPlayerSlot(int playerSlot)
        {
            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            if (!scene || playerSlot < 0) {
                return nullptr;
            }

            for (const auto& gameObject : scene->GetGameObjects()) {
                if (!gameObject) {
                    continue;
                }

                RTBEngine::ECS::NetworkIdentity* identity = gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
                if (identity && identity->networkPlayerSlot == playerSlot) {
                    return gameObject.get();
                }
            }

            return nullptr;
        }

        void HandlePlayerDeathState(const RTBEngine::Online::OnlineMessageContext& context)
        {
            std::size_t offset = 0;
            int playerSlot = -1;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    playerSlot) ||
                playerSlot < 0) {
                return;
            }

            OnlineGameNetSubsystem::ApplyPlayerDeathForSlot(playerSlot);
        }

        void HandlePlayerRevive(const RTBEngine::Online::OnlineMessageContext& context)
        {
            std::size_t offset = 0;
            int playerSlot = -1;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    playerSlot) ||
                playerSlot < 0) {
                return;
            }

            OnlineGameNetSubsystem::ApplyPlayerReviveForSlot(playerSlot);
        }

        void HandlePlayerReviveRequest(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            int playerSlot = -1;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    playerSlot) ||
                playerSlot < 0) {
                return;
            }

            RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(playerSlot);
            if (!pawn) {
                return;
            }

            HealthComponent* health = pawn->GetComponent<HealthComponent>();
            if (!health || !health->IsDead()) {
                return;
            }

            OnlineGameNetSubsystem::ApplyPlayerReviveForSlot(playerSlot);
            OnlineGameNetSubsystem::BroadcastPlayerRevive(playerSlot);
        }

        void HandleEnemySpawn(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            EnemySpawnSnapshot snapshot;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.networkId) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.roundNumber) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.spawnPointIndex) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.spawnIndex) ||
                snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId ||
                snapshot.roundNumber < 1 ||
                snapshot.spawnPointIndex < 0 ||
                snapshot.spawnIndex < 0) {
                return;
            }

            pendingEnemySpawns.push_back(snapshot);
        }

        void HandleRoundStart(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            RoundStartSnapshot snapshot;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.roundNumber) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.enemyCount) ||
                snapshot.roundNumber < 1 ||
                snapshot.enemyCount < 1) {
                return;
            }

            pendingRoundStarts.push_back(snapshot);
        }

        void HandleEnemyDeathState(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            std::uint32_t networkId = RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    networkId) ||
                networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
                return;
            }

            OnlineGameNetSubsystem::ApplyEnemyDeath(networkId);
        }

        void HandlePlayerNetworkBind(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            PlayerNetworkBindSnapshot snapshot;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.playerSlot) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.networkId) ||
                snapshot.playerSlot < 0 ||
                snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
                return;
            }

            pendingPlayerNetworkBinds.push_back(snapshot);
        }

        void HandlePlayerSessionSnapshot(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            PlayerSessionSnapshot snapshot;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.playerSlot) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadString(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.ownerUserIdKey) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadString(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.displayName) ||
                snapshot.playerSlot < 0 ||
                snapshot.displayName.empty()) {
                return;
            }

            OnlineGameNetSubsystem::ApplyPlayerSessionSnapshot(snapshot);
        }

        void HandlePlayerHealthState(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            PlayerHealthSnapshot snapshot;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.playerSlot) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.normalizedHealth) ||
                snapshot.playerSlot < 0) {
                return;
            }

            RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(snapshot.playerSlot);
            if (!pawn) {
                return;
            }

            HealthComponent* health = pawn->GetComponent<HealthComponent>();
            if (!health) {
                return;
            }

            const float clamped = std::clamp(snapshot.normalizedHealth, 0.0f, 1.0f);
            health->SetCurrentHealth(clamped * health->maxHealth);
        }

        void HandleEnemyAttack(const RTBEngine::Online::OnlineMessageContext& context)
        {
            if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
                return;
            }

            std::size_t offset = 0;
            EnemyAttackSnapshot snapshot;
            if (!RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.networkId) ||
                !RTBEngine::Online::OnlineMessageCodec::ReadValue(
                    context.payload,
                    context.payloadSize,
                    offset,
                    snapshot.attackSequence) ||
                snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId ||
                snapshot.attackSequence == 0) {
                return;
            }

            OnlineGameNetSubsystem::ApplyEnemyAttack(snapshot.networkId, snapshot.attackSequence);
        }

    }

    void OnlineGameNetSubsystem::Init()
    {
        if (subsystemInitialized) {
            return;
        }

        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerCombatInput, &HandleCombatInput);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kProjectileSpawn, &HandleProjectileSpawn);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerDeathState, &HandlePlayerDeathState);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerRevive, &HandlePlayerRevive);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerReviveRequest, &HandlePlayerReviveRequest);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kEnemySpawn, &HandleEnemySpawn);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kRoundStart, &HandleRoundStart);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kEnemyDeathState, &HandleEnemyDeathState);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerNetworkBind, &HandlePlayerNetworkBind);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kEnemyAttack, &HandleEnemyAttack);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerHealthState, &HandlePlayerHealthState);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerSessionSnapshot, &HandlePlayerSessionSnapshot);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kMatchPlayerLeaveNotice, &HandlePlayerLeaveNotice);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kMatchPlayerLeft, &HandlePlayerLeft);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kMatchHostAbandoned, &HandleHostAbandoned);
        subsystemInitialized = true;
    }

    void OnlineGameNetSubsystem::Shutdown()
    {
        if (!subsystemInitialized) {
            return;
        }

        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerCombatInput);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kProjectileSpawn);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerDeathState);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerRevive);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerReviveRequest);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kEnemySpawn);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kRoundStart);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kEnemyDeathState);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerNetworkBind);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kEnemyAttack);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerHealthState);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerSessionSnapshot);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kMatchPlayerLeaveNotice);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kMatchPlayerLeft);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kMatchHostAbandoned);
        pendingMainMenuMessage.clear();
        activeMatchNotification.clear();
        matchNotificationSecondsRemaining = 0.0f;
        latestCombatInputs.clear();
        pendingProjectileSpawns.clear();
        pendingEnemySpawns.clear();
        pendingRoundStarts.clear();
        pendingPlayerNetworkBinds.clear();
        lastBroadcastPlayerHealth.clear();
        RTBEngine::Online::OnlineSystem::GetInstance().ClearPlayerSessionProfiles();
        RTBEngine::Online::OnlineGameplayNet::ResetNetworkSession();
        subsystemInitialized = false;
    }

    bool OnlineGameNetSubsystem::SendCombatInput(const PlayerCombatInput& input)
    {
        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return false;
        }

        Init();
        return RTBEngine::Online::OnlineMessageBus::SendToHost(
            kPlayerCombatInput,
            BuildCombatInputPayload(input),
            kCombatInputChannel,
            RTBEngine::Online::OnlinePacketReliability::Unreliable);
    }

    bool OnlineGameNetSubsystem::TryGetLatestCombatInputForUser(
        const std::string& ownerUserIdKey,
        PlayerCombatInput& outInput)
    {
        const auto it = latestCombatInputs.find(ownerUserIdKey);
        if (it == latestCombatInputs.end()) {
            return false;
        }

        outInput = it->second;
        return true;
    }

    bool OnlineGameNetSubsystem::BroadcastProjectileSpawn(const ProjectileSpawnSnapshot& snapshot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return false;
        }

        Init();
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kProjectileSpawn,
            BuildProjectileSpawnPayload(snapshot),
            kProjectileChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    bool OnlineGameNetSubsystem::TryConsumeProjectileSpawn(ProjectileSpawnSnapshot& outSnapshot)
    {
        if (pendingProjectileSpawns.empty()) {
            return false;
        }

        outSnapshot = pendingProjectileSpawns.front();
        pendingProjectileSpawns.pop_front();
        return true;
    }

    bool OnlineGameNetSubsystem::BroadcastPlayerDeath(int playerSlot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() || playerSlot < 0) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, playerSlot);
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kPlayerDeathState,
            payload,
            kPlayerDeathChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    void OnlineGameNetSubsystem::ApplyPlayerDeathForSlot(int playerSlot)
    {
        RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(playerSlot);
        if (!pawn) {
            return;
        }

        HealthComponent* health = pawn->GetComponent<HealthComponent>();
        if (!health) {
            return;
        }

        if (!health->IsDead()) {
            HealthComponent::DamageContext context;
            health->TakeDamage(health->maxHealth + 1.0f, context);
            return;
        }

        if (ThirdPersonCharacterController* controller =
                pawn->GetComponent<ThirdPersonCharacterController>()) {
            controller->ForceDeathState();
        }
    }

    bool OnlineGameNetSubsystem::BroadcastPlayerRevive(int playerSlot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() || playerSlot < 0) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, playerSlot);
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kPlayerRevive,
            payload,
            kPlayerReviveChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    void OnlineGameNetSubsystem::ApplyPlayerReviveForSlot(int playerSlot)
    {
        RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(playerSlot);
        if (!pawn) {
            return;
        }

        HealthComponent* health = pawn->GetComponent<HealthComponent>();
        if (!health) {
            return;
        }

        if (health->IsDead()) {
            health->Revive();
        }

        if (ThirdPersonCharacterController* controller =
                pawn->GetComponent<ThirdPersonCharacterController>()) {
            controller->ReviveFromDeath();
        }
    }

    bool OnlineGameNetSubsystem::RequestPlayerRevive(int playerSlot)
    {
        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() || playerSlot < 0) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, playerSlot);
        return RTBEngine::Online::OnlineMessageBus::SendToHost(
            kPlayerReviveRequest,
            payload,
            kPlayerReviveChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    bool OnlineGameNetSubsystem::BroadcastPlayerNetworkBind(const PlayerNetworkBindSnapshot& snapshot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() ||
            snapshot.playerSlot < 0 ||
            snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, snapshot.playerSlot);
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, snapshot.networkId);
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kPlayerNetworkBind,
            payload,
            kPlayerNetworkBindChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    bool OnlineGameNetSubsystem::BroadcastPlayerSessionSnapshot(const PlayerSessionSnapshot& snapshot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() ||
            snapshot.playerSlot < 0 ||
            snapshot.displayName.empty()) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, snapshot.playerSlot);
        RTBEngine::Online::OnlineMessageCodec::AppendString(payload, snapshot.ownerUserIdKey);
        RTBEngine::Online::OnlineMessageCodec::AppendString(payload, snapshot.displayName);
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kPlayerSessionSnapshot,
            payload,
            kPlayerSessionChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    void OnlineGameNetSubsystem::ApplyPlayerSessionSnapshot(const PlayerSessionSnapshot& snapshot)
    {
        if (snapshot.playerSlot < 0 || snapshot.displayName.empty()) {
            return;
        }

        RTBEngine::Online::OnlinePlayerProfile profile;
        profile.playerSlot = snapshot.playerSlot;
        profile.displayName = snapshot.displayName;

        if (!snapshot.ownerUserIdKey.empty()) {
            TryParseOnlineUserId(snapshot.ownerUserIdKey, profile.userId);
        }

        RTBEngine::Online::OnlineSystem::GetInstance().SetPlayerSessionProfile(profile);
    }

    bool OnlineGameNetSubsystem::TryConsumePlayerNetworkBind(PlayerNetworkBindSnapshot& outSnapshot)
    {
        if (pendingPlayerNetworkBinds.empty()) {
            return false;
        }

        outSnapshot = pendingPlayerNetworkBinds.front();
        pendingPlayerNetworkBinds.pop_front();
        return true;
    }

    void OnlineGameNetSubsystem::ApplyPlayerNetworkBind(int playerSlot, std::uint32_t networkId)
    {
        PlayerNetworkBindSnapshot snapshot;
        snapshot.playerSlot = playerSlot;
        snapshot.networkId = networkId;
        ApplyPlayerNetworkBind(snapshot);
    }

    void OnlineGameNetSubsystem::ApplyPlayerNetworkBind(const PlayerNetworkBindSnapshot& snapshot)
    {
        RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(snapshot.playerSlot);
        if (!pawn || snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
            return;
        }

        RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return;
        }

        identity->SetNetworkId(snapshot.networkId);

        if (RTBEngine::ECS::NetworkTransform* networkTransform =
                pawn->GetComponent<RTBEngine::ECS::NetworkTransform>()) {
            networkTransform->OnValidate();
        }
    }

    void OnlineGameNetSubsystem::TrySyncPlayerHealthFromComponent(
        ::HealthComponent* health,
        float normalizedHealth)
    {
        if (!health ||
            !RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
            !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return;
        }

        RTBEngine::ECS::GameObject* pawn = health->GetOwner();
        if (!pawn) {
            return;
        }

        RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity || identity->networkPlayerSlot < 0) {
            return;
        }

        const int playerSlot = identity->networkPlayerSlot;
        const float clamped = std::clamp(normalizedHealth, 0.0f, 1.0f);
        constexpr float kHealthBroadcastEpsilon = 0.004f;
        const auto previousIt = lastBroadcastPlayerHealth.find(playerSlot);
        if (previousIt != lastBroadcastPlayerHealth.end() &&
            std::fabs(previousIt->second - clamped) < kHealthBroadcastEpsilon) {
            return;
        }

        lastBroadcastPlayerHealth[playerSlot] = clamped;
        Init();

        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, playerSlot);
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, clamped);
        RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kPlayerHealthState,
            payload,
            kPlayerHealthChannel,
            RTBEngine::Online::OnlinePacketReliability::Unreliable);
    }

    bool OnlineGameNetSubsystem::BroadcastEnemySpawn(const EnemySpawnSnapshot& snapshot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() ||
            snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId ||
            snapshot.roundNumber < 1 ||
            snapshot.spawnPointIndex < 0 ||
            snapshot.spawnIndex < 0) {
            return false;
        }

        Init();
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kEnemySpawn,
            BuildEnemySpawnPayload(snapshot),
            kEnemySpawnChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    bool OnlineGameNetSubsystem::TryConsumeEnemySpawn(EnemySpawnSnapshot& outSnapshot)
    {
        if (pendingEnemySpawns.empty()) {
            return false;
        }

        outSnapshot = pendingEnemySpawns.front();
        pendingEnemySpawns.pop_front();
        return true;
    }

    bool OnlineGameNetSubsystem::BroadcastRoundStart(const RoundStartSnapshot& snapshot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() ||
            snapshot.roundNumber < 1 ||
            snapshot.enemyCount < 1) {
            return false;
        }

        Init();
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kRoundStart,
            BuildRoundStartPayload(snapshot),
            kRoundStartChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    bool OnlineGameNetSubsystem::TryConsumeRoundStart(RoundStartSnapshot& outSnapshot)
    {
        if (pendingRoundStarts.empty()) {
            return false;
        }

        outSnapshot = pendingRoundStarts.front();
        pendingRoundStarts.pop_front();
        return true;
    }

    bool OnlineGameNetSubsystem::BroadcastEnemyDeath(std::uint32_t networkId)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() ||
            networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, networkId);
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kEnemyDeathState,
            payload,
            kEnemyDeathChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);
    }

    void OnlineGameNetSubsystem::ApplyEnemyDeath(std::uint32_t networkId)
    {
        RTBEngine::ECS::GameObject* enemy = FindGameObjectByNetworkId(networkId);
        if (!enemy) {
            return;
        }

        HealthComponent* health = enemy->GetComponent<HealthComponent>();
        if (!health || health->IsDead()) {
            return;
        }

        HealthComponent::DamageContext context;
        health->TakeDamage(health->maxHealth + 1.0f, context);
    }

    bool OnlineGameNetSubsystem::HasEnemyWithNetworkId(std::uint32_t networkId)
    {
        return FindGameObjectByNetworkId(networkId) != nullptr;
    }

    bool OnlineGameNetSubsystem::BroadcastEnemyAttack(const EnemyAttackSnapshot& snapshot)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() ||
            snapshot.networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId ||
            snapshot.attackSequence == 0) {
            return false;
        }

        Init();
        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, snapshot.networkId);
        RTBEngine::Online::OnlineMessageCodec::AppendValue(payload, snapshot.attackSequence);
        return RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kEnemyAttack,
            payload,
            kEnemyAttackChannel,
            RTBEngine::Online::OnlinePacketReliability::Unreliable);
    }

    void OnlineGameNetSubsystem::ApplyEnemyAttack(std::uint32_t networkId, std::uint32_t attackSequence)
    {
        RTBEngine::ECS::GameObject* enemy = FindGameObjectByNetworkId(networkId);
        if (!enemy) {
            return;
        }

        EnemyMeleeAI* meleeAI = enemy->GetComponent<EnemyMeleeAI>();
        if (!meleeAI) {
            return;
        }

        meleeAI->PlayReplicatedAttack(attackSequence);
    }

    void OnlineGameNetSubsystem::ApplyPlayerDespawnForSlot(int playerSlot)
    {
        if (playerSlot < 0) {
            return;
        }

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) {
            return;
        }

        RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(playerSlot);
        if (!pawn) {
            return;
        }

        RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (identity && identity->IsLocallyControlled()) {
            return;
        }

        if (identity && !identity->networkOwnerUserId.empty()) {
            latestCombatInputs.erase(identity->networkOwnerUserId);
        }

        lastBroadcastPlayerHealth.erase(playerSlot);

        RTBEngine::Online::OnlineSystem::GetInstance().RemovePlayerSessionProfile(playerSlot);

        if (OnlinePlayerManager* playerManager = FindOnlinePlayerManager()) {
            playerManager->RemovePawnFromTracking(pawn, playerSlot);
        }

        scene->RemoveGameObject(pawn);
    }

    void OnlineGameNetSubsystem::HostNotifyPlayerDisconnected(
        int playerSlot,
        const std::string& displayName)
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsLobbyHost() || playerSlot < 0) {
            return;
        }

        Init();

        const std::string resolvedName = !displayName.empty()
            ? displayName
            : RTBEngine::Online::OnlineSystem::GetInstance().GetPlayerDisplayName(playerSlot);
        const std::string notificationName = resolvedName.empty() ? "Player" : resolvedName;

        ApplyPlayerDespawnForSlot(playerSlot);

        const std::vector<std::uint8_t> broadcastPayload =
            BuildMatchPlayerLeftPayload(playerSlot, notificationName);
        RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
            kMatchPlayerLeft,
            broadcastPayload,
            kMatchExitChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);

        QueueMatchNotification(notificationName + " has left the game.");
    }

    void OnlineGameNetSubsystem::DetectAndDespawnDisconnectedPlayers()
    {
        if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby() ||
            !RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return;
        }

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) {
            return;
        }

        std::unordered_set<std::string> activeMemberKeys;
        for (const RTBEngine::Online::OnlineUserId& member :
                RTBEngine::Online::OnlineGameplayNet::GetOrderedLobbyMembers()) {
            if (member.IsValid()) {
                activeMemberKeys.insert(member.ToString());
            }
        }

        std::vector<int> disconnectedSlots;
        for (const auto& gameObject : scene->GetGameObjects()) {
            if (!gameObject) {
                continue;
            }

            RTBEngine::ECS::NetworkIdentity* identity =
                gameObject->GetComponent<RTBEngine::ECS::NetworkIdentity>();
            if (!identity || identity->networkPlayerSlot < 0 || identity->IsLocallyControlled()) {
                continue;
            }

            if (identity->networkOwnerUserId.empty()) {
                continue;
            }

            if (activeMemberKeys.find(identity->networkOwnerUserId) == activeMemberKeys.end()) {
                disconnectedSlots.push_back(identity->networkPlayerSlot);
            }
        }

        for (int playerSlot : disconnectedSlots) {
            const std::string displayName =
                RTBEngine::Online::OnlineSystem::GetInstance().GetPlayerDisplayName(playerSlot);
            HostNotifyPlayerDisconnected(
                playerSlot,
                displayName.empty() ? "Player" : displayName);
        }
    }

    void OnlineGameNetSubsystem::RequestExitMatch(const char* mainMenuScenePath)
    {
        Init();

        if (!RTBEngine::Online::OnlineGameplayNet::IsInOnlineLobby()) {
            Shutdown();
            LoadMainMenuScene(mainMenuScenePath);
            return;
        }

        const std::string displayName = ResolveLocalDisplayName();

        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            RTBEngine::Online::OnlineMessageBus::BroadcastToClients(
                kMatchHostAbandoned,
                {},
                kMatchExitChannel,
                RTBEngine::Online::OnlinePacketReliability::Reliable);
            Shutdown();
            LeaveActiveOnlineLobby();
            LoadMainMenuScene(mainMenuScenePath);
            return;
        }

        std::vector<std::uint8_t> payload;
        RTBEngine::Online::OnlineMessageCodec::AppendString(payload, displayName);
        RTBEngine::Online::OnlineMessageBus::SendToHost(
            kMatchPlayerLeaveNotice,
            payload,
            kMatchExitChannel,
            RTBEngine::Online::OnlinePacketReliability::Reliable);

        Shutdown();
        LeaveActiveOnlineLobby();
        LoadMainMenuScene(mainMenuScenePath);
    }

    bool OnlineGameNetSubsystem::TryConsumePendingMainMenuMessage(std::string& outMessage)
    {
        if (pendingMainMenuMessage.empty()) {
            return false;
        }

        outMessage = pendingMainMenuMessage;
        pendingMainMenuMessage.clear();
        return true;
    }

    void OnlineGameNetSubsystem::TickMatchNotifications(float deltaTime)
    {
        if (matchNotificationSecondsRemaining <= 0.0f) {
            return;
        }

        matchNotificationSecondsRemaining -= deltaTime;
        if (matchNotificationSecondsRemaining <= 0.0f) {
            matchNotificationSecondsRemaining = 0.0f;
            activeMatchNotification.clear();
        }
    }

    bool OnlineGameNetSubsystem::TryGetActiveMatchNotification(
        std::string& outMessage,
        float& outSecondsRemaining)
    {
        if (activeMatchNotification.empty() || matchNotificationSecondsRemaining <= 0.0f) {
            return false;
        }

        outMessage = activeMatchNotification;
        outSecondsRemaining = matchNotificationSecondsRemaining;
        return true;
    }

}
