#pragma once

#include "GameNetMessageIds.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Online/OnlineUser.h>

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

class HealthComponent;

namespace GameNet {

    struct PlayerCombatInput {
        RTBEngine::Online::OnlineUserId senderUserId;
        std::uint32_t attackSequence = 0;
        float attackDirX = 0.0f;
        float attackDirZ = 0.0f;
    };

    struct ProjectileSpawnSnapshot {
        std::uint32_t spawnId = 0;
        int ownerPlayerSlot = -1;
        RTBEngine::Math::Vector3 origin = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Vector3 direction = RTBEngine::Math::Vector3::Forward();
        float speed = 8.0f;
        float maxDistance = 1.0f;
        float radius = 0.275f;
        float damage = 0.0f;
        int instigatorTeam = 0;
        bool ignoreSameTeam = true;
        bool destroyOnHit = true;
        int maxHits = 1;
    };

    struct EnemySpawnSnapshot {
        std::uint32_t networkId = 0;
        int roundNumber = 0;
        int spawnPointIndex = 0;
        int spawnIndex = 0;
    };

    struct RoundStartSnapshot {
        int roundNumber = 0;
        int enemyCount = 0;
    };

    struct RoundCountdownSnapshot {
        int roundNumber = 0;
        float duration = 0.0f;
    };

    struct PlayerNetworkBindSnapshot {
        int playerSlot = -1;
        std::uint32_t networkId = 0;
    };

    struct PlayerSessionSnapshot {
        int playerSlot = -1;
        std::string ownerUserIdKey;
        std::string displayName;
        std::string characterId;
    };

    struct PlayerHealthSnapshot {
        int playerSlot = -1;
        float normalizedHealth = 1.0f;
    };

    struct EnemyAttackSnapshot {
        std::uint32_t networkId = 0;
        std::uint32_t attackSequence = 0;
    };

    class OnlineGameNetSubsystem {
    public:
        static void Init();
        static void Shutdown();

        static bool SendCombatInput(const PlayerCombatInput& input);
        static bool TryGetLatestCombatInputForUser(
            const std::string& ownerUserIdKey,
            PlayerCombatInput& outInput);

        static bool BroadcastProjectileSpawn(const ProjectileSpawnSnapshot& snapshot);
        static bool TryConsumeProjectileSpawn(ProjectileSpawnSnapshot& outSnapshot);

        static bool BroadcastPlayerDeath(int playerSlot);
        static void ApplyPlayerDeathForSlot(int playerSlot);

        static bool BroadcastPlayerRevive(int playerSlot);
        static void ApplyPlayerReviveForSlot(int playerSlot);
        static bool RequestPlayerRevive(int playerSlot);

        static bool BroadcastPlayerNetworkBind(const PlayerNetworkBindSnapshot& snapshot);
        static bool TryConsumePlayerNetworkBind(PlayerNetworkBindSnapshot& outSnapshot);
        static void RequeuePlayerNetworkBind(const PlayerNetworkBindSnapshot& snapshot);
        static bool ApplyPlayerNetworkBind(int playerSlot, std::uint32_t networkId);
        static bool ApplyPlayerNetworkBind(const PlayerNetworkBindSnapshot& snapshot);

        static bool BroadcastPlayerSessionSnapshot(const PlayerSessionSnapshot& snapshot);
        static void ApplyPlayerSessionSnapshot(const PlayerSessionSnapshot& snapshot);
        static void MergePlayerSessionSnapshot(const PlayerSessionSnapshot& snapshot);
        static bool TryGetPlayerSessionSnapshot(int playerSlot, PlayerSessionSnapshot& outSnapshot);
        static void ClearPlayerSessionSnapshots();
        static bool SendPlayerSessionProfileToHost(
            const std::string& displayName,
            const std::string& characterId);
        static void BroadcastAllKnownPlayerSessionProfiles();
        static void HostSyncAuthoritativeRemotePlayers();

        static void TrySyncPlayerHealthFromComponent(::HealthComponent* health, float normalizedHealth);

        static bool BroadcastEnemySpawn(const EnemySpawnSnapshot& snapshot);
        static bool TryConsumeEnemySpawn(EnemySpawnSnapshot& outSnapshot);

        static bool BroadcastRoundStart(const RoundStartSnapshot& snapshot);
        static bool TryConsumeRoundStart(RoundStartSnapshot& outSnapshot);

        static bool BroadcastRoundCountdown(const RoundCountdownSnapshot& snapshot);
        static bool TryConsumeRoundCountdown(RoundCountdownSnapshot& outSnapshot);

        static bool BroadcastEnemyDeath(std::uint32_t networkId);
        static void ApplyEnemyDeath(std::uint32_t networkId);
        static bool HasEnemyWithNetworkId(std::uint32_t networkId);

        static bool BroadcastEnemyAttack(const EnemyAttackSnapshot& snapshot);
        static void ApplyEnemyAttack(std::uint32_t networkId, std::uint32_t attackSequence);

        static void ApplyPlayerDespawnForSlot(int playerSlot);
        static void HostNotifyPlayerDisconnected(int playerSlot, const std::string& displayName);
        static void DetectAndDespawnDisconnectedPlayers();

        static void RequestExitMatch(const char* mainMenuScenePath);
        static bool TryConsumePendingMainMenuMessage(std::string& outMessage);
        static void TickMatchNotifications(float deltaTime);
        static bool TryGetActiveMatchNotification(std::string& outMessage, float& outSecondsRemaining);
    };

}
