#include "OnlineGameNetMessages.h"

#include "HealthComponent.h"
#include "ThirdPersonCharacterController.h"

#include <RTBEngine/ECS/NetworkIdentity.h>
#include <RTBEngine/ECS/NetworkTransform.h>
#include <RTBEngine/ECS/Scene.h>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Online/OnlineMessageBus.h>
#include <RTBEngine/Online/OnlineMessageCodec.h>
#include <RTBEngine/Online/IOnlineTransport.h>

#include <deque>
#include <unordered_map>

namespace GameNet {

    namespace {

        bool subsystemInitialized = false;
        std::unordered_map<std::string, PlayerCombatInput> latestCombatInputs;
        std::deque<ProjectileSpawnSnapshot> pendingProjectileSpawns;
        std::deque<EnemySpawnSnapshot> pendingEnemySpawns;
        std::deque<RoundStartSnapshot> pendingRoundStarts;
        std::deque<PlayerNetworkBindSnapshot> pendingPlayerNetworkBinds;

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
        latestCombatInputs.clear();
        pendingProjectileSpawns.clear();
        pendingEnemySpawns.clear();
        pendingRoundStarts.clear();
        pendingPlayerNetworkBinds.clear();
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
        RTBEngine::ECS::GameObject* pawn = FindPawnByPlayerSlot(playerSlot);
        if (!pawn || networkId == RTBEngine::Online::OnlineGameplayNet::kInvalidNetworkObjectId) {
            return;
        }

        RTBEngine::ECS::NetworkIdentity* identity = pawn->GetComponent<RTBEngine::ECS::NetworkIdentity>();
        if (!identity) {
            return;
        }

        identity->SetNetworkId(networkId);

        if (RTBEngine::ECS::NetworkTransform* networkTransform =
                pawn->GetComponent<RTBEngine::ECS::NetworkTransform>()) {
            networkTransform->OnValidate();
        }
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

}
