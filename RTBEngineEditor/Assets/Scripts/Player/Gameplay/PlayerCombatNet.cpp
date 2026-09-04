#include "PlayerCombatNet.h"

#include "OnlineGameNetMessages.h"

#include <RTBEngine/Online/OnlineGameplayNet.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/NetworkIdentity.h>

#include <cmath>

namespace PlayerCombatNet {
    namespace {
        constexpr float kDirectionEpsilon = 0.0001f;

        bool HasPlanarDirection(const RTBEngine::Math::Vector3& value)
        {
            return std::abs(value.x) > kDirectionEpsilon || std::abs(value.z) > kDirectionEpsilon;
        }
    }

    void SendLocalInput(
        RTBEngine::Scene::GameObject* owner,
        const RTBEngine::Math::Vector3& desiredMove,
        bool isRunning,
        std::uint32_t& inoutInputSequenceNumber,
        std::uint32_t networkAttackSequence,
        const RTBEngine::Math::Vector3& pendingNetworkAttackDirection)
    {
        if (RTBEngine::Online::OnlineGameplayNet::IsLobbyHost()) {
            return;
        }

        RTBEngine::Online::OnlineGameplayNet::PlayerInputSnapshot snapshot;
        snapshot.senderUserId = RTBEngine::Online::OnlineGameplayNet::GetLocalUserId();
        snapshot.sequenceNumber = ++inoutInputSequenceNumber;
        snapshot.moveX = desiredMove.x;
        snapshot.moveZ = desiredMove.z;
        snapshot.sprint = isRunning;
        RTBEngine::Online::OnlineGameplayNet::SendPlayerInput(snapshot);

        if (networkAttackSequence > 0) {
            GameNet::PlayerCombatInput combatInput;
            combatInput.senderUserId = snapshot.senderUserId;
            combatInput.attackSequence = networkAttackSequence;
            combatInput.attackDirX = pendingNetworkAttackDirection.x;
            combatInput.attackDirZ = pendingNetworkAttackDirection.z;
            GameNet::OnlineGameNetSubsystem::SendCombatInput(combatInput);
        }
    }

    bool TryConsumeRemoteAttackDirection(
        RTBEngine::Scene::GameObject* owner,
        RTBEngine::Scene::NetworkIdentity* networkIdentity,
        std::uint32_t& inoutLastProcessedRemoteAttackSequence,
        RTBEngine::Math::Vector3& outAttackDirection)
    {
        outAttackDirection = RTBEngine::Math::Vector3::Zero();
        if (!networkIdentity || networkIdentity->networkOwnerUserId.empty()) {
            return false;
        }

        GameNet::PlayerCombatInput remoteCombat;
        if (!GameNet::OnlineGameNetSubsystem::TryGetLatestCombatInputForUser(
                networkIdentity->networkOwnerUserId,
                remoteCombat)) {
            return false;
        }

        if (remoteCombat.attackSequence == 0 ||
            remoteCombat.attackSequence == inoutLastProcessedRemoteAttackSequence) {
            return false;
        }

        inoutLastProcessedRemoteAttackSequence = remoteCombat.attackSequence;

        RTBEngine::Math::Vector3 attackDirection(
            remoteCombat.attackDirX,
            0.0f,
            remoteCombat.attackDirZ);
        if (!HasPlanarDirection(attackDirection)) {
            return false;
        }

        attackDirection.Normalize();
        outAttackDirection = attackDirection;
        return true;
    }

}
