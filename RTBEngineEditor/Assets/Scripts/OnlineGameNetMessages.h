#pragma once

#include "GameNetMessageIds.h"

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Online/OnlineUser.h>

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

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

    struct PlayerNetworkBindSnapshot {
        int playerSlot = -1;
        std::uint32_t networkId = 0;
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
        static void ApplyPlayerNetworkBind(int playerSlot, std::uint32_t networkId);

        static bool BroadcastEnemySpawn(const EnemySpawnSnapshot& snapshot);
        static bool TryConsumeEnemySpawn(EnemySpawnSnapshot& outSnapshot);

        static bool BroadcastRoundStart(const RoundStartSnapshot& snapshot);
        static bool TryConsumeRoundStart(RoundStartSnapshot& outSnapshot);

        static bool BroadcastEnemyDeath(std::uint32_t networkId);
        static void ApplyEnemyDeath(std::uint32_t networkId);
        static bool HasEnemyWithNetworkId(std::uint32_t networkId);

        static bool BroadcastEnemyAttack(const EnemyAttackSnapshot& snapshot);
        static void ApplyEnemyAttack(std::uint32_t networkId, std::uint32_t attackSequence);
    };

}
