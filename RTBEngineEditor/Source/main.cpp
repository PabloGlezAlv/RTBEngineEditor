// RTBEngineEditor - main.cpp
#include "Core/EditorApplication.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <Windows.h>

namespace {

    using EnvironmentMap = std::unordered_map<std::string, std::string>;

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

    std::string Unquote(std::string value)
    {
        if (value.size() < 2) {
            return value;
        }

        const char first = value.front();
        const char last = value.back();
        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            return value.substr(1, value.size() - 2);
        }

        return value;
    }

    std::string GetEnvironmentVariableString(const char* name)
    {
        const DWORD requiredSize = GetEnvironmentVariableA(name, nullptr, 0);
        if (requiredSize == 0) {
            return {};
        }

        std::string value(requiredSize, '\0');
        const DWORD writtenSize = GetEnvironmentVariableA(name, value.data(), requiredSize);
        if (writtenSize == 0) {
            return {};
        }

        value.resize(writtenSize);
        return value;
    }

    std::filesystem::path FindLocalEnvFile()
    {
        for (std::filesystem::path directory = std::filesystem::current_path();;
             directory = directory.parent_path()) {
            const std::filesystem::path candidate = directory / ".env.local";
            if (std::filesystem::exists(candidate)) {
                return candidate;
            }

            const std::filesystem::path parent = directory.parent_path();
            if (parent == directory) {
                break;
            }
        }

        return {};
    }

    EnvironmentMap LoadLocalEnvFile()
    {
        EnvironmentMap values;
        const std::filesystem::path envFilePath = FindLocalEnvFile();
        if (envFilePath.empty()) {
            return values;
        }

        std::ifstream file(envFilePath);
        if (!file.is_open()) {
            return values;
        }

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
            const std::string value = Unquote(Trim(trimmedLine.substr(separator + 1)));
            if (!key.empty()) {
                values[key] = value;
            }
        }

        return values;
    }

    std::string GetConfigValue(const EnvironmentMap& localEnv, const char* name)
    {
        // Real environment variables override .env.local for CI and ad-hoc tests.
        const std::string environmentValue = GetEnvironmentVariableString(name);
        if (!environmentValue.empty()) {
            return environmentValue;
        }

        const auto it = localEnv.find(name);
        return it != localEnv.end() ? it->second : std::string();
    }

    std::string ToUpper(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char character) {
                return static_cast<char>(std::toupper(character));
            });
        return value;
    }

    void ConfigureEditorOnline(RTBEngine::Core::ApplicationConfig& config)
    {
        // Keep editor diagnostics available without requiring EOS credentials.
        config.online.enabled = true;
        config.online.failApplicationOnError = false;
        config.online.loadingInEditor = true;
        config.online.backend = RTBEngine::Online::OnlineBackendType::Null;

        const EnvironmentMap localEnv = LoadLocalEnvFile();
        const std::string requestedBackend = ToUpper(GetConfigValue(localEnv, "RTB_ONLINE_BACKEND"));
        if (requestedBackend == "DISABLED" || requestedBackend == "OFF" || requestedBackend == "NONE") {
            config.online.enabled = false;
            return;
        }

        if (requestedBackend != "EOS") {
            return;
        }

        // EOS can be tested locally through environment variables or .env.local.
        config.online.backend = RTBEngine::Online::OnlineBackendType::EOS;
        config.online.productId = GetConfigValue(localEnv, "RTB_EOS_PRODUCT_ID");
        config.online.sandboxId = GetConfigValue(localEnv, "RTB_EOS_SANDBOX_ID");
        config.online.deploymentId = GetConfigValue(localEnv, "RTB_EOS_DEPLOYMENT_ID");
        config.online.clientId = GetConfigValue(localEnv, "RTB_EOS_CLIENT_ID");
        config.online.clientSecret = GetConfigValue(localEnv, "RTB_EOS_CLIENT_SECRET");
    }

}

int main(int argc, char* argv[]) {
    char executablePath[MAX_PATH] = {};
    DWORD pathLength = GetModuleFileNameA(nullptr, executablePath, MAX_PATH);
    if (pathLength > 0 && pathLength < MAX_PATH) {
        std::filesystem::current_path(std::filesystem::path(executablePath).parent_path());
    }

    RTBEngine::Core::ApplicationConfig config;
    config.window.title = "RTBEngine - Editor Mode";
    config.window.width = 1600;
    config.window.height = 900;
    config.window.maximized = true;
    config.initialScenePath = ""; // Start with empty scene
    ConfigureEditorOnline(config);

    RTBEditor::EditorApplication editor;
    if (editor.Initialize(config)) {
        editor.Run();
    }

    return 0;
}
