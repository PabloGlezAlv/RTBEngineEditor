#include "OnlineDisplayNameHelper.h"

#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/OnlineSystem.h>

#include <string>

namespace GameNet {

    bool TryParseOnlineUserId(const std::string& text, RTBEngine::Online::OnlineUserId& outUserId)
    {
        const std::size_t separator = text.find(':');
        if (separator == std::string::npos || separator + 1 >= text.size()) {
            return false;
        }

        const std::string typeName = text.substr(0, separator);
        const std::string value = text.substr(separator + 1);
        if (typeName == RTBEngine::Online::ToString(RTBEngine::Online::OnlineUserIdType::Local)) {
            outUserId = RTBEngine::Online::OnlineUserId(RTBEngine::Online::OnlineUserIdType::Local, value);
            return true;
        }

        if (typeName == RTBEngine::Online::ToString(RTBEngine::Online::OnlineUserIdType::NetworkPeer)) {
            outUserId = RTBEngine::Online::OnlineUserId(RTBEngine::Online::OnlineUserIdType::NetworkPeer, value);
            return true;
        }

        return false;
    }

    std::string ResolveLocalDisplayName()
    {
        RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
        if (const RTBEngine::Online::IOnlineIdentity* identity = online.GetIdentity()) {
            if (identity->IsLoggedIn() && !identity->GetDisplayName().empty()) {
                return identity->GetDisplayName();
            }
        }

        const std::string& sessionName = online.GetSessionDisplayName();
        if (!sessionName.empty()) {
            return sessionName;
        }

        return "Player";
    }

    std::string ResolvePlayerDisplayName(int playerSlot)
    {
        if (playerSlot < 0) {
            return ResolveLocalDisplayName();
        }

        RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
        const std::string sessionName = online.GetPlayerDisplayName(playerSlot);
        if (!sessionName.empty()) {
            return sessionName;
        }

        if (static_cast<int>(online.GetLocalPlayerIndex()) == playerSlot) {
            return ResolveLocalDisplayName();
        }

        return "Player " + std::to_string(playerSlot + 1);
    }

    std::string ResolvePlayerDisplayName(const RTBEngine::ECS::NetworkIdentity* identity)
    {
        if (!identity) {
            return ResolveLocalDisplayName();
        }

        if (identity->networkPlayerSlot >= 0) {
            return ResolvePlayerDisplayName(identity->networkPlayerSlot);
        }

        RTBEngine::Online::OnlineSystem& online = RTBEngine::Online::OnlineSystem::GetInstance();
        if (online.IsInLobby()) {
            if (identity->IsLocallyControlled()) {
                return ResolveLocalDisplayName();
            }

            return "Player";
        }

        if (identity->IsLocallyControlled()) {
            return ResolveLocalDisplayName();
        }

        return "Player";
    }

}
