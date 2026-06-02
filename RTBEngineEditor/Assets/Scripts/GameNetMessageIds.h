#pragma once

#include <cstdint>

namespace GameNet {

    constexpr std::uint16_t kPlayerCombatInput = 64;
    constexpr std::uint16_t kProjectileSpawn = 65;
    constexpr std::uint16_t kPlayerDeathState = 66;
    constexpr std::uint16_t kPlayerRevive = 67;
    constexpr std::uint16_t kPlayerReviveRequest = 68;
    constexpr std::uint16_t kEnemySpawn = 69;
    constexpr std::uint16_t kEnemyDeathState = 70;
    constexpr std::uint16_t kRoundStart = 71;
    constexpr std::uint16_t kPlayerNetworkBind = 72;
    constexpr std::uint16_t kEnemyAttack = 73;
    constexpr std::uint16_t kPlayerHealthState = 74;
    constexpr std::uint16_t kPlayerSessionSnapshot = 75;

    constexpr std::uint8_t kCombatInputChannel = 4;
    constexpr std::uint8_t kProjectileChannel = 5;
    constexpr std::uint8_t kPlayerDeathChannel = 6;
    constexpr std::uint8_t kPlayerReviveChannel = 7;
    constexpr std::uint8_t kEnemySpawnChannel = 8;
    constexpr std::uint8_t kEnemyDeathChannel = 9;
    constexpr std::uint8_t kRoundStartChannel = 10;
    constexpr std::uint8_t kPlayerNetworkBindChannel = 11;
    constexpr std::uint8_t kEnemyAttackChannel = 12;
    constexpr std::uint8_t kPlayerHealthChannel = 13;
    constexpr std::uint8_t kPlayerSessionChannel = 14;

}
