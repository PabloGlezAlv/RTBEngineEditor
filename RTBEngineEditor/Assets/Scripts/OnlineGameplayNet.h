#pragma once

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Online/OnlineUser.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class OnlineGameplayNet {
public:
    struct TransformSnapshot {
        std::string objectKey;
        RTBEngine::Math::Vector3 position = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Quaternion rotation = RTBEngine::Math::Quaternion::Identity();
        RTBEngine::Online::OnlineUserId senderUserId;
    };

    struct PlayerInputSnapshot {
        RTBEngine::Online::OnlineUserId senderUserId;
        std::uint32_t sequenceNumber = 0;
        float moveX = 0.0f;
        float moveZ = 0.0f;
        bool sprint = false;
    };

    static void Pump();

    static bool IsInOnlineLobby();
    static bool IsLobbyHost();
    static bool IsLobbyOwner();
    static std::size_t GetRemoteLobbyMemberCount();

    static RTBEngine::Online::OnlineUserId GetLocalUserId();
    static RTBEngine::Online::OnlineUserId GetLobbyHostUserId();
    static std::vector<RTBEngine::Online::OnlineUserId> GetOrderedLobbyMembers();
    static std::size_t GetLocalPlayerIndex();

    static bool BroadcastStartMatch(const std::string& scenePath);
    static bool ConsumeStartMatch(std::string& outScenePath);

    static bool SendPlayerInput(const PlayerInputSnapshot& snapshot);
    static bool TryGetLatestInputForUser(
        const std::string& ownerUserIdKey,
        PlayerInputSnapshot& outSnapshot);

    static bool BroadcastTransform(const TransformSnapshot& snapshot);
    static bool TryGetLatestTransform(const std::string& objectKey, TransformSnapshot& outSnapshot);
};
