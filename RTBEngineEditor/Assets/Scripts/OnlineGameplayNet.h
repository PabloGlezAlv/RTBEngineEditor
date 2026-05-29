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
    // Authoritative world state for a replicated pawn (host -> clients).
    struct TransformSnapshot {
        std::string objectKey;  // e.g. "PlayerSlot_0"
        RTBEngine::Math::Vector3 position = RTBEngine::Math::Vector3::Zero();
        RTBEngine::Math::Quaternion rotation = RTBEngine::Math::Quaternion::Identity();
        RTBEngine::Online::OnlineUserId senderUserId;
    };

    // Client locomotion intent (client -> host). sequenceNumber reserved for prediction.
    struct PlayerInputSnapshot {
        RTBEngine::Online::OnlineUserId senderUserId;
        std::uint32_t sequenceNumber = 0;
        float moveX = 0.0f;  // planar world X component, normalized
        float moveZ = 0.0f;
        bool sprint = false;
    };

    // Drain incoming UDP packets from IOnlineTransport (call every frame).
    static void Pump();

    static bool IsInOnlineLobby();
    static bool IsLobbyHost();
    static bool IsLobbyOwner();
    static std::size_t GetRemoteLobbyMemberCount();

    static RTBEngine::Online::OnlineUserId GetLocalUserId();
    static RTBEngine::Online::OnlineUserId GetLobbyHostUserId();
    // Stable ordering: lobby owner first, then joiners.
    static std::vector<RTBEngine::Online::OnlineUserId> GetOrderedLobbyMembers();
    static std::size_t GetLocalPlayerIndex();

    // Host-only: tell all clients to load the gameplay scene.
    static bool BroadcastStartMatch(const std::string& scenePath);
    static bool ConsumeStartMatch(std::string& outScenePath);

    // Client-only: send WASD intent to the lobby host.
    static bool SendPlayerInput(const PlayerInputSnapshot& snapshot);
    // Host-only: read last input received from a remote peer (key = networkOwnerUserId string).
    static bool TryGetLatestInputForUser(
        const std::string& ownerUserIdKey,
        PlayerInputSnapshot& outSnapshot);

    // Host-only: broadcast authoritative transform for one pawn.
    static bool BroadcastTransform(const TransformSnapshot& snapshot);
    // Client-side: read last transform for objectKey (does not erase from buffer).
    static bool TryGetLatestTransform(const std::string& objectKey, TransformSnapshot& outSnapshot);
};
