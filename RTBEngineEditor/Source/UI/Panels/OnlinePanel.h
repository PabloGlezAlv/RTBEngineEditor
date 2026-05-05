#pragma once

#include "EditorPanel.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/OnlineResult.h>
#include <RTBEngine/Online/OnlineSystem.h>

#include <string>
#include <vector>

namespace RTBEditor {

    class OnlinePanel : public EditorPanel {
    public:
        OnlinePanel();
        ~OnlinePanel() override;

        OnlinePanel(const OnlinePanel&) = delete;
        OnlinePanel& operator=(const OnlinePanel&) = delete;

        void OnUIRender(EditorContext& context) override;

    private:
        struct LoginEventEntry {
            std::string previousStatus;
            std::string currentStatus;
            std::string localUserId;
        };

        void RefreshIdentitySubscription(RTBEngine::Online::IOnlineIdentity* identity);
        void AddLoginEvent(const RTBEngine::Online::OnlineLoginStatusChangedEvent& eventData);
        void StoreActionResult(const RTBEngine::Online::OnlineResult& result);

        void DrawSystemSection(const RTBEngine::Online::OnlineSystem& onlineSystem);
        void DrawIdentitySection(RTBEngine::Online::IOnlineIdentity* identity);
        void DrawLoginEventsSection();

        RTBEngine::Online::IOnlineIdentity* subscribedIdentity = nullptr;
        RTBEngine::Core::EventSubscription loginStatusSubscription;
        std::vector<LoginEventEntry> loginEvents;

        char displayName[64] = "EditorUser";
        std::string lastActionMessage;
        bool lastActionSucceeded = true;
    };

}
