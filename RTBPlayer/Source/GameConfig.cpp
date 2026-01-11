#include "GameConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace RTBPlayer {

    GameConfig::GameConfig() {}
    GameConfig::~GameConfig() {}

    bool GameConfig::Load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open " << path << ", using defaults" << std::endl;
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
                else if (currentSection == "Game") {
                    if (key == "Name") windowTitle = value;
                }
            }
        }

        return true;
    }

}
