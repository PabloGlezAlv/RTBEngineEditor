#pragma once

#include <RTBEngine/ECS/NetworkIdentity.h>
#include <RTBEngine/Online/OnlineUser.h>

#include <string>

namespace GameNet {

    bool TryParseOnlineUserId(const std::string& text, RTBEngine::Online::OnlineUserId& outUserId);

    std::string ResolveLocalDisplayName();

    std::string ResolvePlayerDisplayName(int playerSlot);

    std::string ResolvePlayerDisplayName(const RTBEngine::ECS::NetworkIdentity* identity);

}
