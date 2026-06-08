#pragma once

#include <RTBEngine/Online/OnlineConfig.h>
#include <RTBEngine/Online/OnlineEditorBridge.h>

#include <filesystem>
#include <string>

namespace RTBEditor {

#pragma warning(push)
#pragma warning(disable: 4251)
    struct EditorOnlineSettings {
        bool enabled = true;
        RTBEngine::Online::OnlineBackendType backendType = RTBEngine::Online::OnlineBackendType::RelayOnline;
        std::uint16_t lanGamePort = 27015;
        std::uint16_t lanDiscoveryPort = 27016;
        std::string defaultHostAddress;
        std::string relayMatchmakingUrl = "http://localhost:8080/api/v1";
        std::string loginDisplayName;
        std::string defaultStartScene = "Assets/Scenes/MainMenu.lua";
    };
#pragma warning(pop)

    class EditorOnlineSettingsStore {
    public:
        static std::filesystem::path GetSettingsPath();
        static EditorOnlineSettings Load();
        static bool Save(const EditorOnlineSettings& settings);
        static void ApplyToOnlineConfig(const EditorOnlineSettings& settings,
                                        RTBEngine::Online::OnlineConfig& config);
        static RTBEngine::Online::OnlineEditorSettingsPayload BuildPayload(
            const EditorOnlineSettings& settings);
        static bool ApplyAndInitializeOnline(const EditorOnlineSettings& settings);
    };

}
