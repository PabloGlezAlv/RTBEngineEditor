#include "Project.h"
#include <RTBEngine/Core/Logger.h>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace RTBEditor {

    Project* Project::activeProject = nullptr;

    namespace {
        std::string NormalizeProjectPath(const std::string& path) {
            return std::filesystem::path(path).lexically_normal().generic_string();
        }
    }

    Project::Project() {}

    Project::~Project() {
        if (activeProject == this) {
            activeProject = nullptr;
        }
    }

    bool Project::Load(const std::filesystem::path& path) {
        const std::filesystem::path absolutePath = std::filesystem::absolute(path).lexically_normal();
        std::ifstream file(absolutePath);
        if (!file.is_open()) {
            RTB_ERROR("Failed to open project file: " + absolutePath.string());
            return false;
        }

        name = "New Project";
        startScene = "Assets/Scenes/DefaultScene.lua";
        lastOpenScene.clear();
        projectFilePath = absolutePath;
        projectDirectory = projectFilePath.parent_path();
        assetDirectory = "Assets";

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    if (key == "Name") name = value;
                    else if (key == "StartScene") startScene = NormalizeProjectPath(value);
                    else if (key == "LastOpenScene") lastOpenScene = NormalizeProjectPath(value);
                    else if (key == "AssetDirectory") assetDirectory = std::filesystem::path(value);
                }
            }
        }

        activeProject = this;

        return true;
    }

    void Project::SetStartScene(const std::string& path) {
        startScene = NormalizeProjectPath(path);
    }

    void Project::SetLastOpenScene(const std::string& path) {
        lastOpenScene = NormalizeProjectPath(path);
    }

    bool Project::Save(const std::filesystem::path& path) {
        std::filesystem::path savePath = path.empty() ? projectFilePath : std::filesystem::absolute(path).lexically_normal();
        if (savePath.empty()) {
            return false;
        }

        std::ofstream file(savePath);
        if (!file.is_open()) return false;

        file << "Name=" << name << "\n";
        file << "StartScene=" << startScene << "\n";
        if (!lastOpenScene.empty())
            file << "LastOpenScene=" << lastOpenScene << "\n";
        file << "AssetDirectory=" << assetDirectory.string() << "\n";

        projectFilePath = savePath;
        projectDirectory = projectFilePath.parent_path();

        return true;
    }

    std::filesystem::path Project::GetAssetRootPath() const {
        if (projectDirectory.empty()) {
            return assetDirectory;
        }
        return (projectDirectory / assetDirectory).lexically_normal();
    }

    std::filesystem::path Project::ResolveAssetPath(const std::filesystem::path& relativePath) const {
        return (GetAssetRootPath() / relativePath).lexically_normal();
    }

    std::string Project::GetAssetReferencePath(const std::filesystem::path& relativePath) const {
        return (assetDirectory / relativePath).lexically_normal().generic_string();
    }

    std::filesystem::path Project::GetGameScriptsProjectPath() const {
        if (projectDirectory.empty()) {
            return {};
        }
        return (projectDirectory / "GameScripts" / "GameScripts.vcxproj").lexically_normal();
    }

    std::filesystem::path Project::GetGameScriptsDllPath(const std::string& configuration) const {
        if (projectDirectory.empty()) {
            return {};
        }
        return (projectDirectory / "x64" / configuration / "GameScripts.dll").lexically_normal();
    }

    std::filesystem::path Project::GetSDKPath() const {
        if (projectDirectory.empty()) {
            return {};
        }
        return (projectDirectory / "RTBEngine_SDK").lexically_normal();
    }

}
