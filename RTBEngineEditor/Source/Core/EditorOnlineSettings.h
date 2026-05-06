#pragma once

#include <RTBEngine/Online/OnlineConfig.h>

#include <filesystem>
#include <string>

namespace RTBEditor {

#pragma warning(push)
#pragma warning(disable: 4251)
    struct EditorOnlineSettings {
        bool enabled = true;
        RTBEngine::Online::OnlineBackendType backend = RTBEngine::Online::OnlineBackendType::Null;
        std::string productId;
        std::string sandboxId;
        std::string deploymentId;
        std::string clientId;
        std::string clientSecret;

        bool HasCompleteEosConfig() const;
    };
#pragma warning(pop)

    class EditorOnlineSettingsStore {
    public:
        static std::filesystem::path GetSettingsPath();
        static EditorOnlineSettings Load();
        static bool Save(const EditorOnlineSettings& settings);
        static void ApplyToOnlineConfig(const EditorOnlineSettings& settings,
                                        RTBEngine::Online::OnlineConfig& config);
    };

}
