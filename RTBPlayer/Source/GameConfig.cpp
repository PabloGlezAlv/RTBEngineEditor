#include "GameConfig.h"
#include <RTBEngine/Core/Logger.h>
#include <cstdint>
#include <fstream>
#include <sstream>

namespace {

    bool ParseBool(const std::string& value)
    {
        return value == "true" || value == "1" || value == "True" || value == "TRUE";
    }

    RTBEngine::Online::OnlineBackendType ParseOnlineBackend(const std::string& value)
    {
        if (value == "EOS" || value == "eos") {
            return RTBEngine::Online::OnlineBackendType::EOS;
        }

        return RTBEngine::Online::OnlineBackendType::Null;
    }

    std::uint32_t ParseUInt32(const std::string& value)
    {
        if (value.empty()) {
            return 0;
        }

        return static_cast<std::uint32_t>(std::stoul(value));
    }

}

namespace RTBPlayer {

    GameConfig::GameConfig() {}
    GameConfig::~GameConfig() {}

    bool GameConfig::Load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            RTB_WARN("Could not open " + path + ", using defaults");
            return false;
        }

        std::string line;
        std::string currentSection;

        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            // Check for section header
            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }

            // Parse key=value
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);

                // Trim whitespace
                while (!key.empty() && (key.back() == ' ' || key.back() == '\t')) key.pop_back();
                while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) value.erase(0, 1);

                if (currentSection == "Window") {
                    if (key == "Title") windowTitle = value;
                    else if (key == "Width") windowWidth = std::stoi(value);
                    else if (key == "Height") windowHeight = std::stoi(value);
                    else if (key == "Fullscreen") fullscreen = (value == "true" || value == "1");
                }
                else if (currentSection == "Scene") {
                    if (key == "StartScene") startScene = value;
                }
                else if (currentSection == "Online") {
                    if (key == "Enabled") onlineConfig.enabled = ParseBool(value);
                    else if (key == "FailApplicationOnError") onlineConfig.failApplicationOnError = ParseBool(value);
                    else if (key == "Backend") onlineConfig.backend = ParseOnlineBackend(value);
                    else if (key == "ProductName") onlineConfig.productName = value;
                    else if (key == "ProductVersion") onlineConfig.productVersion = value;
                    else if (key == "ProductId") onlineConfig.productId = value;
                    else if (key == "SandboxId") onlineConfig.sandboxId = value;
                    else if (key == "DeploymentId") onlineConfig.deploymentId = value;
                    else if (key == "ClientId") onlineConfig.clientId = value;
                    else if (key == "ClientSecret") onlineConfig.clientSecret = value;
                    else if (key == "IsServer") onlineConfig.isServer = ParseBool(value);
                    else if (key == "DisableOverlay") onlineConfig.disableOverlay = ParseBool(value);
                    else if (key == "CacheDirectory") onlineConfig.cacheDirectory = value;
                    else if (key == "TickBudgetMilliseconds") onlineConfig.tickBudgetMilliseconds = ParseUInt32(value);
                }
                else if (currentSection == "Game") {
                    if (key == "Name") windowTitle = value;
                }
            }
        }

        return true;
    }

}
