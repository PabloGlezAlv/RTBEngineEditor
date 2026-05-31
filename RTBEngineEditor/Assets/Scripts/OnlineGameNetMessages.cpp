#include "OnlineGameNetMessages.h"

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

    }

    void OnlineGameNetSubsystem::Init()
    {
        if (subsystemInitialized) {
            return;
        }

        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kPlayerCombatInput, &HandleCombatInput);
        RTBEngine::Online::OnlineMessageBus::RegisterHandler(kProjectileSpawn, &HandleProjectileSpawn);
        subsystemInitialized = true;
    }

    void OnlineGameNetSubsystem::Shutdown()
    {
        if (!subsystemInitialized) {
            return;
        }

        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kPlayerCombatInput);
        RTBEngine::Online::OnlineMessageBus::UnregisterHandler(kProjectileSpawn);
        latestCombatInputs.clear();
        pendingProjectileSpawns.clear();
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

}
