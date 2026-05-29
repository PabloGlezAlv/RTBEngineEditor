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

    RTBEngine::Online::OnlineLoginType ParseLoginType(const std::string& value)
    {
        return RTBEngine::Online::OnlineLoginType::DeviceId;
    }

    std::uint32_t ParseUInt32(const std::string& value)
    {
        if (value.empty()) {
            return 0;
        }

        return static_cast<std::uint32_t>(std::stoul(value));
    }

    std::uint16_t ParsePort(const std::string& value, std::uint16_t defaultValue)
    {
        if (value.empty()) {
            return defaultValue;
        }

        const std::uint32_t parsed = ParseUInt32(value);
        if (parsed > 0 && parsed <= 65535) {
            return static_cast<std::uint16_t>(parsed);
        }

        return defaultValue;
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
            if (line.empty() || line[0] == '#') continue;

            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }

            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);

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
                    else if (key == "ProductName") onlineConfig.productName = value;
                    else if (key == "ProductVersion") onlineConfig.productVersion = value;
                    else if (key == "IsServer") onlineConfig.isServer = ParseBool(value);
                    else if (key == "CacheDirectory") onlineConfig.cacheDirectory = value;
                    else if (key == "TickBudgetMilliseconds") onlineConfig.tickBudgetMilliseconds = ParseUInt32(value);
                    else if (key == "LanGamePort") onlineConfig.lanGamePort = ParsePort(value, onlineConfig.lanGamePort);
                    else if (key == "LanDiscoveryPort") onlineConfig.lanDiscoveryPort = ParsePort(value, onlineConfig.lanDiscoveryPort);
                    else if (key == "LoginType") onlineConfig.loginType = ParseLoginType(value);
                    else if (key == "LoginDisplayName") onlineConfig.loginDisplayName = value;
                }
                else if (currentSection == "Game") {
                    if (key == "Name") windowTitle = value;
                }
            }
        }

        return true;
    }

    void GameConfig::ApplyCommandLine(int argc, char* argv[])
    {
        for (int index = 1; index < argc; ++index) {
            std::string argument = argv[index] ? argv[index] : "";
            if (argument.rfind("--", 0) == 0) {
                argument.erase(0, 2);
            } else if (argument.rfind("-", 0) == 0) {
                argument.erase(0, 1);
            }

            const std::size_t separator = argument.find('=');
            if (separator == std::string::npos) {
                continue;
            }

            const std::string key = argument.substr(0, separator);
            const std::string value = argument.substr(separator + 1);

            if (key == "start-scene") startScene = value;
            else if (key == "window-title") windowTitle = value;
            else if (key == "online-enabled") onlineConfig.enabled = ParseBool(value);
            else if (key == "display-name") onlineConfig.loginDisplayName = value;
            else if (key == "lan-game-port") onlineConfig.lanGamePort = ParsePort(value, onlineConfig.lanGamePort);
            else if (key == "lan-discovery-port") onlineConfig.lanDiscoveryPort = ParsePort(value, onlineConfig.lanDiscoveryPort);
            else if (key == "cache-directory") onlineConfig.cacheDirectory = value;
        }
    }

}
