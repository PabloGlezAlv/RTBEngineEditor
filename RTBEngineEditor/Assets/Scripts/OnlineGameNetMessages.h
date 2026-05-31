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
    };

}
