#pragma once

#include "EditorPanel.h"

#include <RTBEngine/Online/IOnlineIdentity.h>
#include <RTBEngine/Online/IOnlineLobby.h>
#include <RTBEngine/Online/OnlineResult.h>
#include <RTBEngine/Online/OnlineSystem.h>
#include "../../Core/EditorOnlineSettings.h"
#include "../../Core/MultiplayerTestLauncher.h"

#include <string>

namespace RTBEditor {

    class OnlinePanel : public EditorPanel {
    public:
        OnlinePanel();
        ~OnlinePanel() override;

        OnlinePanel(const OnlinePanel&) = delete;
        OnlinePanel& operator=(const OnlinePanel&) = delete;

        void OnUIRender(EditorContext& context) override;

    private:
        void StoreIdentityActionResult(const RTBEngine::Online::OnlineResult& result);
        void StoreLobbyActionResult(const RTBEngine::Online::OnlineResult& result);

        void LoadSettingsIntoFields();
        EditorOnlineSettings BuildSettingsFromFields() const;
        void DrawSettingsSection();
        void DrawSessionSection(RTBEngine::Online::OnlineSystem& onlineSystem);
        void DrawMultiplayerTestSection();

        bool onlineSettingsEnabled = true;
        int lanGamePort = 27015;
        int lanDiscoveryPort = 27016;
        char defaultHostAddress[128] = "";
        char relayMatchmakingUrl[256] = "http://localhost:8080/api/v1";
        char loginDisplayName[64] = "";
        char defaultStartScene[260] = "Assets/Scenes/MainMenu.lua";
        char sessionDisplayName[64] = "EditorUser";
        char lobbyCode[32] = "";
        char joinHostAddress[128] = "";
        int lobbyMaxMembers = 6;
        std::string lastSettingsMessage;
        bool lastSettingsSucceeded = true;
        std::string lastIdentityActionMessage;
        bool lastIdentityActionSucceeded = true;
        std::string lastLobbyActionMessage;
        bool lastLobbyActionSucceeded = true;

        MultiplayerTestLauncher multiplayerLauncher;
        int multiplayerPlayerCount = 2;
    };

}
