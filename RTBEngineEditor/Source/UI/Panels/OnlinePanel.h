#pragma once

#include "EditorPanel.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Online/OnlineResult.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include "../../Core/EditorOnlineSettings.h"
#include "../../Core/MultiplayerTestLauncher.h"

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

        struct LobbyEventEntry {
            std::string previousState;
            std::string currentState;
            std::string lobbyId;
        };

        void RefreshIdentitySubscription(RTBEngine::Online::IOnlineIdentity* identity);
        void RefreshLobbySubscription(RTBEngine::Online::IOnlineLobby* lobby);
        void AddLoginEvent(const RTBEngine::Online::OnlineLoginStatusChangedEvent& eventData);
        void AddLobbyEvent(const RTBEngine::Online::OnlineLobbyStatusChangedEvent& eventData);
        void StoreIdentityActionResult(const RTBEngine::Online::OnlineResult& result);
        void StoreLobbyActionResult(const RTBEngine::Online::OnlineResult& result);

        void LoadSettingsIntoFields();
        RTBEditor::EditorOnlineSettings BuildSettingsFromFields() const;
        void DrawSettingsSection();
        void DrawSystemSection(const RTBEngine::Online::OnlineSystem& onlineSystem);
        void DrawIdentitySection(RTBEngine::Online::IOnlineIdentity* identity);
        void DrawLobbySection(RTBEngine::Online::IOnlineLobby* lobby,
                              const RTBEngine::Online::IOnlineIdentity* identity);
        void DrawMultiplayerTestSection();
        void DrawLoginEventsSection();
        void DrawLobbyEventsSection();

        RTBEngine::Online::IOnlineIdentity* subscribedIdentity = nullptr;
        RTBEngine::Online::IOnlineLobby* subscribedLobby = nullptr;
        RTBEngine::Core::EventSubscription loginStatusSubscription;
        RTBEngine::Core::EventSubscription lobbyStatusSubscription;
        std::vector<LoginEventEntry> loginEvents;
        std::vector<LobbyEventEntry> lobbyEvents;

        bool onlineSettingsEnabled = true;
        int onlineBackendIndex = 0;
        bool showClientSecret = false;
        char eosProductId[128] = "";
        char eosSandboxId[128] = "";
        char eosDeploymentId[128] = "";
        char eosClientId[128] = "";
        char eosClientSecret[256] = "";
        std::string lastSettingsMessage;
        bool lastSettingsSucceeded = true;

        char displayName[64] = "EditorUser";
        char lobbyBucketId[64] = "RTBEngine";
        int lobbyMaxMembers = 6;
        bool lobbyPublicAdvertised = true;
        bool lobbyAllowInvites = true;
        bool lobbyAllowJoinById = true;
        bool lobbyAllowHostMigration = false;
        char lobbyTargetId[128] = "";

        std::string lastIdentityActionMessage;
        bool lastIdentityActionSucceeded = true;
        std::string lastLobbyActionMessage;
        bool lastLobbyActionSucceeded = true;

        MultiplayerTestLauncher multiplayerLauncher;
        int multiplayerPlayerCount = 2;
        char multiplayerStartScene[260] = "Assets/Scenes/LobbyScene.lua";
    };

}
