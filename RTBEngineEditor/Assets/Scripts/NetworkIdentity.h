#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Online/OnlineUser.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class NetworkIdentity : public RTBEngine::ECS::Component {
public:
    NetworkIdentity() = default;
    ~NetworkIdentity() override = default;

    std::string networkOwnerUserId;  // lobby member id (Local/NetworkPeer), set at runtime
    int networkPlayerSlot = -1;      // 0 = host, 1 = first joiner; -1 = offline/unassigned

    RTB_COMPONENT(NetworkIdentity)

public:
    void SetOwnerUserId(const RTBEngine::Online::OnlineUserId& userId);
    void SetNetworkPlayerSlot(int slot);

    // True if this human player runs camera + sends input on this machine.
    bool IsLocallyControlled() const;
    // True if this machine should run physics/movement for this pawn (host in online).
    bool IsSimulatedByHost() const;
    // Replication key shared by host and all clients, e.g. "PlayerSlot_1".
    std::string GetNetworkObjectKey() const;
};
