#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <cstdint>
#include <string>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
        class NetworkIdentity;
    }
}

namespace PlayerCombatNet {

    void SendLocalInput(
        RTBEngine::Scene::GameObject* owner,
        const RTBEngine::Math::Vector3& desiredMove,
        bool isRunning,
        std::uint32_t& inoutInputSequenceNumber,
        std::uint32_t networkAttackSequence,
        const RTBEngine::Math::Vector3& pendingNetworkAttackDirection);

    bool TryConsumeRemoteAttackDirection(
        RTBEngine::Scene::GameObject* owner,
        RTBEngine::Scene::NetworkIdentity* networkIdentity,
        std::uint32_t& inoutLastProcessedRemoteAttackSequence,
        RTBEngine::Math::Vector3& outAttackDirection);

}
