#include "EditorOnlineSettings.h"

#include <RTBEngine/Online/OnlineEditorBridge.h>
#include <RTBEngine/Online/OnlineTypes.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {

    std::string Trim(const std::string& value)
    {
        const auto first = std::find_if_not(value.begin(), value.end(),
            [](unsigned char character) {
                return std::isspace(character) != 0;
            });

        if (first == value.end()) {
            return {};
        }

        const auto last = std::find_if_not(value.rbegin(), value.rend(),
            [](unsigned char character) {
                return std::isspace(character) != 0;
            }).base();

        return std::string(first, last);
    }

    std::string ToUpper(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char character) {
                return static_cast<char>(std::toupper(character));
            });

        return value;
    }

    std::filesystem::path GetLocalAppDataDirectory()
    {
        char* value = nullptr;
        size_t valueSize = 0;
        if (_dupenv_s(&value, &valueSize, "LOCALAPPDATA") != 0 || !value) {
            return std::filesystem::current_path();
        }

        std::string result(value);
        std::free(value);
        return std::filesystem::path(result);
    }

    bool ParseBool(const std::string& value, bool defaultValue)
    {
        const std::string normalized = ToUpper(Trim(value));
        if (normalized == "1" || normalized == "TRUE" || normalized == "YES" || normalized == "ON") {
            return true;
        }

        if (normalized == "0" || normalized == "FALSE" || normalized == "NO" || normalized == "OFF") {
            return false;
        }

        return defaultValue;
    }

    RTBEngine::Online::OnlineLoginType ParseLoginType(const std::string& value)
    {
        const std::string normalized = ToUpper(Trim(value));
        if (normalized == "DEVELOPERAUTH" || normalized == "DEV_AUTH" || normalized == "DEVAUTH") {
            return RTBEngine::Online::OnlineLoginType::DeveloperAuth;
        }

        if (normalized == "ACCOUNTPORTAL" || normalized == "ACCOUNT_PORTAL") {
            return RTBEngine::Online::OnlineLoginType::AccountPortal;
        }

        return RTBEngine::Online::OnlineLoginType::DeviceId;
    }

    const char* SerializeLoginType(RTBEngine::Online::OnlineLoginType loginType)
    {
        switch (loginType) {
        case RTBEngine::Online::OnlineLoginType::DeveloperAuth:
            return "DeveloperAuth";
        case RTBEngine::Online::OnlineLoginType::AccountPortal:
            return "AccountPortal";
        case RTBEngine::Online::OnlineLoginType::DeviceId:
        default:
            return "DeviceId";
        }
    }

    RTBEngine::Online::OnlineBackendType ParseBackendType(
        const std::string& value,
        RTBEngine::Online::OnlineBackendType defaultValue)
    {
        const std::string normalized = ToUpper(Trim(value));
        if (normalized == "RELAY") {
            return RTBEngine::Online::OnlineBackendType::RelayOnline;
        }

        if (normalized == "LAN") {
            return RTBEngine::Online::OnlineBackendType::Lan;
        }

        return defaultValue;
    }

    const char* SerializeBackendType(RTBEngine::Online::OnlineBackendType backendType)
    {
        switch (backendType) {
        case RTBEngine::Online::OnlineBackendType::RelayOnline:
            return "Relay";
        case RTBEngine::Online::OnlineBackendType::Lan:
        default:
            return "LAN";
        }
    }

    std::uint16_t ParsePort(const std::string& value, std::uint16_t defaultValue)
    {
        if (value.empty()) {
            return defaultValue;
        }

        try {
            const int port = std::stoi(value);
            if (port > 0 && port <= 65535) {
                return static_cast<std::uint16_t>(port);
            }
        }
        catch (...) {
        }

        return defaultValue;
    }

}

namespace RTBEditor {

    std::filesystem::path EditorOnlineSettingsStore::GetSettingsPath()
    {
        return GetLocalAppDataDirectory() / "RTBEngineEditor" / "online_settings.ini";
    }

    EditorOnlineSettings EditorOnlineSettingsStore::Load()
    {
        EditorOnlineSettings settings;
        const std::filesystem::path settingsPath = GetSettingsPath();

        std::ifstream file(settingsPath);
        if (!file.is_open()) {
            Save(settings);
            return settings;
        }

        std::unordered_map<std::string, std::string> values;
        std::string line;
        while (std::getline(file, line)) {
            const std::string trimmedLine = Trim(line);
            if (trimmedLine.empty() || trimmedLine[0] == '#') {
                continue;
            }

            const std::size_t separator = trimmedLine.find('=');
            if (separator == std::string::npos) {
                continue;
            }

            const std::string key = Trim(trimmedLine.substr(0, separator));
            const std::string value = Trim(trimmedLine.substr(separator + 1));
            if (!key.empty()) {
                values[key] = value;
            }
        }

        auto readValue = [&values](const char* key) -> std::string {
            const auto it = values.find(key);
            return it != values.end() ? it->second : std::string();
        };

        settings.enabled = ParseBool(readValue("Enabled"), settings.enabled);
        settings.backendType = ParseBackendType(readValue("BackendType"), settings.backendType);
        settings.lanGamePort = ParsePort(readValue("LanGamePort"), settings.lanGamePort);
        settings.lanDiscoveryPort = ParsePort(readValue("LanDiscoveryPort"), settings.lanDiscoveryPort);
        settings.defaultHostAddress = readValue("DefaultHostAddress");
        settings.relayMatchmakingUrl = readValue("RelayMatchmakingUrl");
        if (settings.relayMatchmakingUrl.empty()) {
            settings.relayMatchmakingUrl = "http://localhost:8080/api/v1";
        }
        settings.loginDisplayName = readValue("LoginDisplayName");
        settings.defaultStartScene = readValue("DefaultStartScene");
        if (settings.defaultStartScene.empty()) {
            settings.defaultStartScene = "Assets/Scenes/MainMenu.lua";
        }

        return settings;
    }

    bool EditorOnlineSettingsStore::Save(const EditorOnlineSettings& settings)
    {
        const std::filesystem::path settingsPath = GetSettingsPath();
        std::error_code error;
        std::filesystem::create_directories(settingsPath.parent_path(), error);
        if (error) {
            return false;
        }

        std::ofstream file(settingsPath, std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        file << "# RTBEngineEditor local online settings\n";
        file << "Enabled=" << (settings.enabled ? "true" : "false") << "\n";
        file << "BackendType=" << SerializeBackendType(settings.backendType) << "\n";
        file << "LanGamePort=" << settings.lanGamePort << "\n";
        file << "LanDiscoveryPort=" << settings.lanDiscoveryPort << "\n";
        file << "DefaultHostAddress=" << settings.defaultHostAddress << "\n";
        file << "RelayMatchmakingUrl=" << settings.relayMatchmakingUrl << "\n";
        file << "LoginDisplayName=" << settings.loginDisplayName << "\n";
        file << "DefaultStartScene=" << settings.defaultStartScene << "\n";

        return file.good();
    }

    void EditorOnlineSettingsStore::ApplyToOnlineConfig(const EditorOnlineSettings& settings,
                                                        RTBEngine::Online::OnlineConfig& config)
    {
        config.enabled = settings.enabled;
        config.failApplicationOnError = false;
        config.loadingInEditor = true;
        config.backendType = settings.backendType;
        config.lanGamePort = settings.lanGamePort;
        config.lanDiscoveryPort = settings.lanDiscoveryPort;
        config.defaultHostAddress = settings.defaultHostAddress;
        config.relayMatchmakingUrl = settings.relayMatchmakingUrl;
        config.loginType = RTBEngine::Online::OnlineLoginType::DeviceId;
        config.loginDisplayName = settings.loginDisplayName;
    }

    RTBEngine::Online::OnlineEditorSettingsPayload EditorOnlineSettingsStore::BuildPayload(
        const EditorOnlineSettings& settings)
    {
        RTBEngine::Online::OnlineEditorSettingsPayload payload{};
        payload.enabled = settings.enabled;
        payload.backendType = RTBEngine::Online::IsRelayBackend(settings.backendType) ? 1 : 0;
        payload.lanGamePort = settings.lanGamePort;
        payload.lanDiscoveryPort = settings.lanDiscoveryPort;

        const auto copyToBuffer = [](char* destination, std::size_t capacity, const std::string& value) {
            if (capacity == 0) {
                return;
            }

            std::snprintf(destination, capacity, "%s", value.c_str());
        };

        copyToBuffer(payload.relayMatchmakingUrl, sizeof(payload.relayMatchmakingUrl), settings.relayMatchmakingUrl);
        copyToBuffer(payload.defaultHostAddress, sizeof(payload.defaultHostAddress), settings.defaultHostAddress);
        copyToBuffer(payload.loginDisplayName, sizeof(payload.loginDisplayName), settings.loginDisplayName);
        return payload;
    }

    bool EditorOnlineSettingsStore::ApplyAndInitializeOnline(const EditorOnlineSettings& settings)
    {
        if (!Save(settings)) {
            return false;
        }

        return RTBEngine::Online::InitializeOnlineFromEditorSettings(BuildPayload(settings));
    }

}
