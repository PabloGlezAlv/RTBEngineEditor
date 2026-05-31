#pragma once

#include <cstdint>

namespace GameNet {

    constexpr std::uint16_t kPlayerCombatInput = 64;
    constexpr std::uint16_t kProjectileSpawn = 65;
    constexpr std::uint16_t kPlayerDeathState = 66;
    constexpr std::uint16_t kPlayerRevive = 67;
    constexpr std::uint16_t kPlayerReviveRequest = 68;

    constexpr std::uint8_t kCombatInputChannel = 4;
    constexpr std::uint8_t kProjectileChannel = 5;
    constexpr std::uint8_t kPlayerDeathChannel = 6;
    constexpr std::uint8_t kPlayerReviveChannel = 7;

}
