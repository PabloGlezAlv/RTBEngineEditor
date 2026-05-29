#include "NetworkIdentity.h"

#include "OnlineGameplayNet.h"

#include <RTBEngine/ECS/GameObject.h>

using ThisClass = NetworkIdentity;

RTB_REGISTER_COMPONENT(NetworkIdentity)
    RTB_PROPERTY(networkOwnerUserId)
    RTB_PROPERTY(networkPlayerSlot)
RTB_END_REGISTER(NetworkIdentity)

void NetworkIdentity::SetOwnerUserId(const RTBEngine::Online::OnlineUserId& userId)
{
    networkOwnerUserId = userId.IsValid() ? userId.ToString() : std::string();
}

void NetworkIdentity::SetNetworkPlayerSlot(int slot)
{
    networkPlayerSlot = slot;
}

bool NetworkIdentity::IsLocallyControlled() const
{
    if (!OnlineGameplayNet::IsInOnlineLobby()) {
        return true;
    }

    // Prefer slot index: identical on host and client (unlike Local vs NetworkPeer ids).
    if (networkPlayerSlot >= 0) {
        return static_cast<int>(OnlineGameplayNet::GetLocalPlayerIndex()) == networkPlayerSlot;
    }

    if (networkOwnerUserId.empty()) {
        return true;
    }

    return networkOwnerUserId == OnlineGameplayNet::GetLocalUserId().ToString();
}

bool NetworkIdentity::IsSimulatedByHost() const
{
    if (!OnlineGameplayNet::IsInOnlineLobby()) {
        return true;
    }

    return OnlineGameplayNet::IsLobbyHost();
}

std::string NetworkIdentity::GetNetworkObjectKey() const
{
    if (networkPlayerSlot >= 0) {
        return "PlayerSlot_" + std::to_string(networkPlayerSlot);
    }

    if (!networkOwnerUserId.empty()) {
        return networkOwnerUserId;
    }

    if (!owner) {
        return std::string();
    }

    return owner->GetUUID().empty() ? owner->GetName() : owner->GetUUID();
}
