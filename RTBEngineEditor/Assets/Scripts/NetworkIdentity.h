#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Online/OnlineUser.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class NetworkIdentity : public RTBEngine::ECS::Component {
public:
    NetworkIdentity() = default;
    ~NetworkIdentity() override = default;

    std::string networkOwnerUserId;
    int networkPlayerSlot = -1;

    RTB_COMPONENT(NetworkIdentity)

public:
    void SetOwnerUserId(const RTBEngine::Online::OnlineUserId& userId);
    void SetNetworkPlayerSlot(int slot);
    bool IsLocallyControlled() const;
    bool IsSimulatedByHost() const;
    std::string GetNetworkObjectKey() const;
};
