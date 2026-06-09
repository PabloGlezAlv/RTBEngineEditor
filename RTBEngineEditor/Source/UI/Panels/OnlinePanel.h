#pragma once

#include "EditorPanel.h"

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
        void LoadSettingsIntoFields();
        EditorOnlineSettings BuildSettingsFromFields() const;
        bool ApplyOnlineSettings(const EditorOnlineSettings& settings);
        void DrawInfrastructureSection();
        void DrawStatusSection();
        void DrawMultiplayerTestSection();

        bool onlineSettingsEnabled = true;
        int lanGamePort = 27015;
        int lanDiscoveryPort = 27016;
        char relayMatchmakingUrl[256] = "http://localhost:8080/api/v1";
        char defaultStartScene[260] = "Assets/Scenes/MainMenu.lua";
        std::string lastSettingsMessage;
        bool lastSettingsSucceeded = true;

        MultiplayerTestLauncher multiplayerLauncher;
        int multiplayerPlayerCount = 2;
    };

}
