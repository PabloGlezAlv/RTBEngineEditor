#include "EditorOnlineSettings.h"

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

    std::string GetEnvironmentVariableValue(const char* name)
    {
        char* value = nullptr;
        size_t valueSize = 0;
        if (_dupenv_s(&value, &valueSize, name) != 0 || !value) {
            return {};
        }

        std::string result(value);
        std::free(value);
        return result;
    }

    std::filesystem::path GetLocalAppDataDirectory()
    {
        const std::string localAppData = GetEnvironmentVariableValue("LOCALAPPDATA");
        if (!localAppData.empty()) {
            return std::filesystem::path(localAppData);
        }

        return std::filesystem::current_path();
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

    RTBEngine::Online::OnlineBackendType ParseBackend(const std::string& value)
    {
        const std::string normalized = ToUpper(Trim(value));
        if (normalized == "EOS") {
            return RTBEngine::Online::OnlineBackendType::EOS;
        }

        return RTBEngine::Online::OnlineBackendType::Null;
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

    const char* SerializeBackend(RTBEngine::Online::OnlineBackendType backend)
    {
        return backend == RTBEngine::Online::OnlineBackendType::EOS ? "EOS" : "Null";
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

}

namespace RTBEditor {

    bool EditorOnlineSettings::HasCompleteEosConfig() const
    {
        return !productId.empty() &&
            !sandboxId.empty() &&
            !deploymentId.empty() &&
            !clientId.empty() &&
            !clientSecret.empty();
    }

    bool EditorOnlineSettings::HasCompleteDeveloperAuthConfig() const
    {
        return loginType != RTBEngine::Online::OnlineLoginType::DeveloperAuth ||
            (!developerAuthHost.empty() && !developerAuthCredentialName.empty());
    }

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
        settings.backend = ParseBackend(readValue("Backend"));
        settings.productId = readValue("ProductId");
        settings.sandboxId = readValue("SandboxId");
        settings.deploymentId = readValue("DeploymentId");
        settings.clientId = readValue("ClientId");
        settings.clientSecret = readValue("ClientSecret");
        settings.disableOverlay = ParseBool(readValue("DisableOverlay"), settings.disableOverlay);
        settings.loginType = ParseLoginType(readValue("LoginType"));
        settings.loginDisplayName = readValue("LoginDisplayName");
        settings.developerAuthHost = readValue("DeveloperAuthHost");
        if (settings.developerAuthHost.empty()) {
            settings.developerAuthHost = "localhost:6300";
        }
        settings.developerAuthCredentialName = readValue("DeveloperAuthCredentialName");

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
        file << "# This file is stored outside the repo in LOCALAPPDATA.\n";
        file << "Enabled=" << (settings.enabled ? "true" : "false") << "\n";
        file << "Backend=" << SerializeBackend(settings.backend) << "\n";
        file << "ProductId=" << settings.productId << "\n";
        file << "SandboxId=" << settings.sandboxId << "\n";
        file << "DeploymentId=" << settings.deploymentId << "\n";
        file << "ClientId=" << settings.clientId << "\n";
        file << "ClientSecret=" << settings.clientSecret << "\n";
        file << "DisableOverlay=" << (settings.disableOverlay ? "true" : "false") << "\n";
        file << "LoginType=" << SerializeLoginType(settings.loginType) << "\n";
        file << "LoginDisplayName=" << settings.loginDisplayName << "\n";
        file << "DeveloperAuthHost=" << settings.developerAuthHost << "\n";
        file << "DeveloperAuthCredentialName=" << settings.developerAuthCredentialName << "\n";

        return file.good();
    }

    void EditorOnlineSettingsStore::ApplyToOnlineConfig(const EditorOnlineSettings& settings,
                                                        RTBEngine::Online::OnlineConfig& config)
    {
        config.enabled = settings.enabled;
        config.failApplicationOnError = false;
        config.loadingInEditor = true;
        config.backend = settings.backend;

        config.productId = settings.productId;
        config.sandboxId = settings.sandboxId;
        config.deploymentId = settings.deploymentId;
        config.clientId = settings.clientId;
        config.clientSecret = settings.clientSecret;
        config.disableOverlay = settings.disableOverlay;
        config.loginType = settings.loginType;
        config.loginDisplayName = settings.loginDisplayName;
        config.developerAuthHost = settings.developerAuthHost;
        config.developerAuthCredentialName = settings.developerAuthCredentialName;
    }

}
