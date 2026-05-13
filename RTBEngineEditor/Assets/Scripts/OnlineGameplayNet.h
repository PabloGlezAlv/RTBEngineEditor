#pragma once

#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Online/OnlineUser.h>

#include <cstddef>
#include <string>

class OnlineGameplayNet {
public:
    struct TransformSnapshot {
        std::string objectKey;
        RTBEngine::Math::Vector3 position = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Quaternion rotation = RTBEngine::Math::Quaternion::Identity();
        RTBEngine::Online::OnlineUserId senderUserId;
    };

    static void Pump();

    static bool IsInOnlineLobby();
    static bool IsLobbyOwner();
    static std::size_t GetRemoteLobbyMemberCount();

    static bool BroadcastStartMatch(const std::string& scenePath);
    static bool ConsumeStartMatch(std::string& outScenePath);

    static bool BroadcastTransform(const TransformSnapshot& snapshot);
    static bool ConsumeLatestTransform(const std::string& objectKey, TransformSnapshot& outSnapshot);
};
