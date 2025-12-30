#include "Project.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace RTBEditor {

    Project* Project::activeProject = nullptr;

    Project::Project() {
        if (!activeProject) {
            activeProject = this;
        }
    }

    Project::~Project() {
        if (activeProject == this) {
            activeProject = nullptr;
        }
    }

    bool Project::Load(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open project file: " << path << std::endl;
            return false;
        }

        projectDirectory = path.parent_path();
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    if (key == "Name") name = value;
                    else if (key == "StartScene") startScene = value;
                    else if (key == "AssetDirectory") assetDirectory = value;
                }
            }
        }

        activeProject = this;
        return true;
    }

    bool Project::Save(const std::filesystem::path& path) {
        std::ofstream file(path);
        if (!file.is_open()) return false;

        file << "Name=" << name << "\n";
        file << "StartScene=" << startScene << "\n";
        file << "AssetDirectory=" << assetDirectory.string() << "\n";

        return true;
    }

}
